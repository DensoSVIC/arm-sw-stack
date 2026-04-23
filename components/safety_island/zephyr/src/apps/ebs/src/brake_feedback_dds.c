#include "brake_feedback_dds.h"
#include "sensor_dds.h"
#include "VehicleEmergencyStamped.h"

#ifdef CONFIG_GPIO_RCAR
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/gpio.h>
#endif

#include "config.h"

#include <stdio.h>

K_EVENT_DEFINE(BRAKE_DDS_SETUP_EVENT)

#ifdef CONFIG_GPIO_RCAR
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
static const struct led_dt_spec led0 = LED_DT_SPEC_GET(LED0_NODE);
static const struct led_dt_spec led1 = LED_DT_SPEC_GET(LED1_NODE);
static const struct led_dt_spec led2 = LED_DT_SPEC_GET(LED2_NODE);

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
#endif

enum BRAKE_DDS_EVENTS 
{
    BRAKE_DDS_NO_DDS = 0b01,
    BRAKE_DDS_CONNECTED = 0b10
};

enum SENSOR_DDS_EVENTS 
{
    SENSOR_DDS_NO_DDS = 0b01,
    SENSOR_DDS_CONNECTED = 0b10
};

uint8_t brake_dds_ubOutBuffer[256];
ucdrBuffer brake_dds_ubOut;

uxrSession brake_dds_session;
uxrUDPTransport brake_dds_transport;

uint8_t brake_dds_outbuf[BRAKE_BUFFER_SIZE * BRAKE_STREAM_HISTORY];
uint8_t brake_dds_inbuf[BRAKE_BUFFER_SIZE * BRAKE_STREAM_HISTORY];

uxrObjectId brake_dds_uxrid_entity;
uxrObjectId brake_dds_uxrid_topic;
uxrObjectId brake_dds_uxrid_sub;
uxrObjectId brake_dds_uxrid_pub;
uxrObjectId brake_dds_uxrid_reader;
uxrObjectId brake_dds_uxrid_writer;
uxrObjectId brake_dds_uxrid_participant; 

uxrStreamId brake_dds_out;
uxrStreamId brake_dds_in;

extern bool new_msg;
extern bool sensor_brake_signal;
extern float sensor_mps;

extern uxrSession sensor_dds_session;
extern ucdrBuffer sensor_dds_ubOut;
extern uxrObjectId sensor_dds_uxrid_writer;
extern uxrStreamId sensor_dds_out;

extern struct k_event SENSOR_DDS_SETUP_EVENT;

// K_THREAD_DEFINE(brake_dds_setup_tid, 5000, brake_dds_setup, NULL, NULL, NULL, 5, K_ESSENTIAL, 0);
// K_THREAD_DEFINE(brake_dds_process_tid, 5000, brake_dds_process, NULL, NULL, NULL, 5, K_ESSENTIAL, 0);
// K_THREAD_DEFINE(brake_dds_check_tid, 5000, brake_dds_check, NULL, NULL, NULL, 5, K_ESSENTIAL, 0);

K_MUTEX_DEFINE(BRAKE_DDS_LOCK);

void brake_dds_setup(void *, void *, void *)
{
    while(1)
    {
        k_event_wait(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_NO_DDS, false, K_FOREVER);

        printf("Starting Brake DDS transport init\n");

        ucdr_init_buffer(&brake_dds_ubOut, brake_dds_ubOutBuffer, 256);
        ucdr_reset_buffer(&brake_dds_ubOut);

        if (!uxr_init_udp_transport(&brake_dds_transport, UXR_IPv4, BRAKE_FEEDBACK_DDS_IP_ADDR, BRAKE_FEEDBACK_DDS_IP_PORT))
        {
            printf("Attempted Brake DDS transport init ...\n");
            k_sleep(K_SECONDS(5));
            continue;
        } else {
            printf("Finished Brake DDS transport init\n");
        }

        printf("Starting Brake DDS session init\n");
        
        uxr_init_session(&brake_dds_session, &brake_dds_transport.comm, 0xDECAFBAD);

        if (!uxr_create_session(&brake_dds_session))
        {
            printf("Attempted Brake DDS session init ...\n");
            goto remove_session;
        } else {
            printf("Finished Brake DDS session init\n");
        }

        uxr_set_topic_callback(&brake_dds_session, brake_on_msg, NULL);

        if (!uxr_ping_agent_session(&brake_dds_session, 100, 1))
        {
            printf("Brake DDS - Sorry, no agent available\n");
            goto remove_session;
        }

        //Setup subscribers
        brake_dds_out = uxr_create_output_reliable_stream(
            &brake_dds_session, brake_dds_outbuf, BRAKE_BUFFER_SIZE, BRAKE_STREAM_HISTORY);

        brake_dds_in = uxr_create_input_reliable_stream(
            &brake_dds_session, brake_dds_inbuf, BRAKE_BUFFER_SIZE, BRAKE_STREAM_HISTORY);

        brake_dds_uxrid_participant = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
        brake_dds_uxrid_topic  = uxr_object_id(0x01, UXR_TOPIC_ID);
        brake_dds_uxrid_pub    = uxr_object_id(0x01, UXR_PUBLISHER_ID);
        brake_dds_uxrid_writer = uxr_object_id(0x01, UXR_DATAWRITER_ID);
        uint8_t     status[4];
        uint16_t    requests[4] = {
            uxr_buffer_create_participant_xml(&brake_dds_session, brake_dds_out, brake_dds_uxrid_participant, 0,
                "<dds>"
                    "<participant>"
                        "<rtps>"
                            "<name>default_xrce_participant</name>"
                        "</rtps>"
                    "</participant>"
                "</dds>"
                , UXR_REPLACE),
            uxr_buffer_create_topic_xml(&brake_dds_session, brake_dds_out, brake_dds_uxrid_topic, brake_dds_uxrid_participant,
                "<dds>"
                    "<topic>"
                        "<name>rt/control/command/emergency_cmd2</name>"
                        "<dataType>tier4_vehicle_msgs::msg::dds_::VehicleEmergencyStamped_</dataType>"
                    "</topic>"
                "</dds>",
                    UXR_REPLACE),
            uxr_buffer_create_publisher_xml(&brake_dds_session, brake_dds_out, brake_dds_uxrid_pub, brake_dds_uxrid_participant, "", UXR_REPLACE),
            uxr_buffer_create_datawriter_xml(&brake_dds_session, brake_dds_out, brake_dds_uxrid_writer, brake_dds_uxrid_pub,
                "<dds>"
                    "<data_writer>"
                        "<topic>"
                            "<kind>NO_KEY</kind>"
                            "<name>rt/control/command/emergency_cmd2</name>"
                            "<dataType>tier4_vehicle_msgs::msg::dds_::VehicleEmergencyStamped_</dataType>"
                        "</topic>"
                    "</data_writer>"
                "</dds>"
                , UXR_REPLACE)
        };

        if (!uxr_run_session_until_all_status(&brake_dds_session, 100, requests, status, 4)) {
            printf("failed to create entities: participant: %i topic: %i "
                "sub: %i reader: %i\n",
                status[0], status[1], status[2], status[3]);
            goto remove_session;
        }

        printf("Brake DDS Success! Agent is up within a session\n");

        k_event_set(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_CONNECTED);
        continue;

    remove_session:
        // Delete resources
        uxr_delete_session(&brake_dds_session);

        uxr_close_udp_transport(&brake_dds_transport);
        
    }
}

void brake_dds_check(void *, void *, void *)
{
    int brake_fail_count = 0;

    while (1)
    {
        k_event_wait(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_CONNECTED, false, K_FOREVER);

        k_mutex_lock(&BRAKE_DDS_LOCK, K_FOREVER);
        bool brake_connected = uxr_ping_agent_session(&brake_dds_session, 20, 5);
        k_mutex_unlock(&BRAKE_DDS_LOCK);

        if (!brake_connected)
        {
            brake_fail_count++;

            if (brake_fail_count >= 2)
            {
                k_sched_lock();
                uxr_delete_session(&brake_dds_session);
                uxr_close_udp_transport(&brake_dds_transport);

                k_event_set(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_NO_DDS);
                k_sched_unlock();

                printf("Brake Session Disconnected\n");
            }
        } else {
            brake_fail_count = 0;
        }

        k_yield();
    }
}

void brake_dds_process(void *, void *, void *)
{
    bool prev_brake_signal = false;
    bool confirmed_delivery = false;


    while(1)
    {
        
        #ifdef CONFIG_GPIO_RCAR
        bool brake = gpio_pin_get_dt(&button0);
        #else
        bool brake = false;
        #endif

        if (prev_brake_signal && (sensor_mps >= 0.05 || sensor_mps <= -0.05))
            brake = true;

        int64_t time_triggered = uxr_millis();

        bool brake_session_up = k_event_test(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_CONNECTED) == BRAKE_DDS_CONNECTED;
        bool sensor_session_up = k_event_test(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_CONNECTED) == SENSOR_DDS_CONNECTED;

        if (sensor_session_up)
        {
            brake = sensor_brake_signal || brake;
        }

        #ifdef CONFIG_GPIO_RCAR
        if (brake) {
            led_on_dt(&led0);
            led_on_dt(&led1);
            led_on_dt(&led2);
        } else {
            led_off_dt(&led0);
            led_off_dt(&led1);
            led_off_dt(&led2);
        }
        #endif

        if(new_msg) {
            tier4_vehicle_msgs_msg_VehicleEmergencyStamped topic = {
                {0, 0}, brake
            };

            if (brake_session_up)
            {	
                ucdr_reset_buffer(&brake_dds_ubOut);
                uint32_t topic_size = tier4_vehicle_msgs_msg_VehicleEmergencyStamped_size_of_topic(&topic, 0);
                uxr_prepare_output_stream(&brake_dds_session, brake_dds_out, brake_dds_uxrid_writer, &brake_dds_ubOut, topic_size);
                tier4_vehicle_msgs_msg_VehicleEmergencyStamped_serialize_topic(&brake_dds_ubOut, &topic);
            }

            if (sensor_session_up)
            {
                ucdr_reset_buffer(&sensor_dds_ubOut);

                ucdr_serialize_endian_bool(&sensor_dds_ubOut, UCDR_LITTLE_ENDIANNESS, brake);
                ucdr_serialize_endian_int32_t(&sensor_dds_ubOut, UCDR_LITTLE_ENDIANNESS, time_triggered);

                uxr_prepare_output_stream(&sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_writer, &sensor_dds_ubOut, 8);
            }
            
            if (brake)
            {
                if (brake_session_up)
                {
                    printf("[%07lli] Brake On!\n", time_triggered);
                } else {
                    printf("[%07lli] Brake On!  - No DDS Connection\n", time_triggered);
                }
            }  else {
                if (brake_session_up)
                {
                    printf("[%07lli] Brake Off\n", time_triggered);
                } else {
                    printf("[%07lli] Brake Off  - No DDS Connection\n", time_triggered);
                }
            }

            prev_brake_signal = brake;
            new_msg = false;
        }

        if (brake_session_up || !confirmed_delivery)
        {
            k_mutex_lock(&BRAKE_DDS_LOCK, K_FOREVER);
            confirmed_delivery = uxr_run_session_until_confirm_delivery(&brake_dds_session, 50);
            k_mutex_unlock(&BRAKE_DDS_LOCK);
        }

        k_yield();
    }
}

void brake_on_msg(uxrSession *sess, uxrObjectId obj_id, uint16_t req_id, \
    uxrStreamId stream, struct ucdrBuffer *ub, uint16_t len, void *_)
{
    (void)sess;
	(void)req_id;
	(void)stream;
	(void)_;
	(void)len;

	// printf("on_msg");

	printf("Unexpected Brake DDS msg\n");
}
