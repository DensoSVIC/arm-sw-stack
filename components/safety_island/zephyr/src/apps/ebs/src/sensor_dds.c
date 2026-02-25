#include "sensor_dds.h"
#include <stdio.h>

#include "config.h"

K_EVENT_DEFINE(SENSOR_DDS_SETUP_EVENT)

enum SENSOR_DDS_EVENTS 
{
    SENSOR_DDS_NO_DDS = 0b01,
    SENSOR_DDS_CONNECTED = 0b10
};

bool new_msg = false;
bool sensor_brake_signal = false;
float sensor_mps = 0.0;

uint8_t sensor_dds_ubOutBuffer[256];
uint8_t sensor_dds_outbuf[SENSOR_BUFFER_SIZE * SENSOR_STREAM_HISTORY];
uint8_t sensor_dds_inbuf[SENSOR_BUFFER_SIZE * SENSOR_STREAM_HISTORY];

ucdrBuffer sensor_dds_ubOut;

uxrSession sensor_dds_session;
uxrUDPTransport sensor_dds_transport;

uxrObjectId sensor_dds_uxrid_entity;
uxrObjectId sensor_dds_uxrid_topic;
uxrObjectId sensor_dds_uxrid_sub;
uxrObjectId sensor_dds_uxrid_pub;
uxrObjectId sensor_dds_uxrid_reader;
uxrObjectId sensor_dds_uxrid_writer;

uxrStreamId sensor_dds_out;
uxrStreamId sensor_dds_in;

// K_THREAD_DEFINE(sensor_dds_setup_tid, 5000, sensor_dds_setup, NULL, NULL, NULL, 5, K_ESSENTIAL, 0);
// K_THREAD_DEFINE(sensor_dds_process_tid, 5000, sensor_dds_process, NULL, NULL, NULL, 5, K_ESSENTIAL, 0);

void sensor_dds_setup(void *, void *, void *) 
{
    while(1)
    {
        k_event_wait(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_NO_DDS, false, K_FOREVER);

        printf("Starting R-Car DDS transport init\n");

        ucdr_init_buffer(&sensor_dds_ubOut, sensor_dds_ubOutBuffer, 256);
        ucdr_reset_buffer(&sensor_dds_ubOut);

        
        if (!uxr_init_udp_transport(&sensor_dds_transport, UXR_IPv4, SENSOR_DDS_IP_ADDR, SENSOR_DDS_IP_PORT))
        {
            printf("Attempted R-Car DDS transport init ...\n");
            k_sleep(K_SECONDS(5));
            continue;
        } else {
            printf("Finished R-Car DDS transport init\n");
        }

        printf("Starting R-Car DDS session init\n");

        uxr_init_session(&sensor_dds_session, &sensor_dds_transport.comm, 0xDECAFBAD);

        if (!uxr_create_session(&sensor_dds_session))
        {
            printf("Attempted R-Car dds session init ...\n");
            goto remove_session;
        } else {
            printf("Finished R-Car dds session init\n");
        }

        uxr_set_topic_callback(&sensor_dds_session, sensor_on_msg, NULL);

        if (!uxr_ping_agent_session(&sensor_dds_session, 100, 1))
        {
            printf("R-Car DDS Sorry, no agent available\n");
            goto remove_session;
        }

        //Setup subscribers
        sensor_dds_out = uxr_create_output_reliable_stream( \
            &sensor_dds_session, sensor_dds_outbuf, SENSOR_BUFFER_SIZE, SENSOR_STREAM_HISTORY);

        sensor_dds_in = uxr_create_input_reliable_stream( \
            &sensor_dds_session, sensor_dds_inbuf, SENSOR_BUFFER_SIZE, SENSOR_STREAM_HISTORY);

        sensor_dds_uxrid_entity = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
        sensor_dds_uxrid_topic  = uxr_object_id(0x01, UXR_TOPIC_ID);
        sensor_dds_uxrid_sub    = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
        sensor_dds_uxrid_pub    = uxr_object_id(0x01, UXR_PUBLISHER_ID);
        sensor_dds_uxrid_reader = uxr_object_id(0x01, UXR_DATAREADER_ID);
        sensor_dds_uxrid_writer = uxr_object_id(0x01, UXR_DATAWRITER_ID);
        uint8_t     status[6];
        uint16_t    requests[6] = {
            uxr_buffer_create_participant_xml(
                    &sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_entity, 0, //
                    "	<dds>"
                    "		<participant>"
                    "			<rtps>"
                    "				<name>subscriber</name>"
                    "			</rtps>"
                    "		</participant>"
                    "	</dds>",
                    UXR_REPLACE),
            uxr_buffer_create_topic_xml(&sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_topic, sensor_dds_uxrid_entity, 
                    "	<dds>"
                    "		<topic>"
                    "			<name>messages_in</name>"
                    "			<dataType>inputMsg</dataType>"
                    "		</topic>"
                    "		<topic>"
                    "			<name>messages_out</name>"
                    "			<dataType>outputMsg</dataType>"
                    "		</topic>"
                    "	</dds>",
                    UXR_REPLACE),
            uxr_buffer_create_subscriber_xml(&sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_sub, sensor_dds_uxrid_entity, "", UXR_REPLACE),
            uxr_buffer_create_publisher_xml(&sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_pub, sensor_dds_uxrid_entity, "", UXR_REPLACE),
            uxr_buffer_create_datareader_xml(
                    &sensor_dds_session, sensor_dds_in, sensor_dds_uxrid_reader, sensor_dds_uxrid_sub, 
                "	<dds>"
                "		<data_reader>"
                "			<topic>"
                "				<kind>NO_KEY</kind>"
                "				<name>messages_in</name>"
                "				<dataType>inputMsg</dataType>"
                "			</topic>"
                "		</data_reader>"
                "	</dds>",
                    UXR_REPLACE),
            uxr_buffer_create_datawriter_xml(
                &sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_writer, sensor_dds_uxrid_pub, //
                "	<dds>"
                "		<data_writer>"
                "			<topic>"
                "				<kind>NO_KEY</kind>"
                "				<name>messages_out</name>"
                "				<dataType>outputMsg</dataType>"
                "			</topic>"
                "		</data_writer>"
                "	</dds>",
                UXR_REPLACE),
            };

        if (!uxr_run_session_until_all_status(&sensor_dds_session, 100, requests, status, 6)) {
            printf("failed to create entities: participant: %i topic: %i "
                "sub: %i reader: %i\n",
                status[0], status[1], status[2], status[3]);
            goto remove_session;
        }

        uxrDeliveryControl delivery_control = {0};
        delivery_control.max_samples        = UXR_MAX_SAMPLES_UNLIMITED;
        uxr_buffer_request_data(&sensor_dds_session, sensor_dds_out, sensor_dds_uxrid_reader, sensor_dds_in, &delivery_control);
        
        printf("R-Car DDS Success! Agent is up within a session\n");

        k_event_set(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_CONNECTED);
        continue;

    remove_session:
        // Delete resources
        uxr_delete_session(&sensor_dds_session);

        uxr_close_udp_transport(&sensor_dds_transport);

        k_yield();
    }
}

void sensor_dds_process(void *, void *, void *) 
{
    int sensor_dds_fail_count = 0;
    bool sensor_dds_connected;

    while(1)
    {
        k_event_wait(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_CONNECTED, false, K_FOREVER);

        sensor_dds_connected = uxr_run_session_timeout(&sensor_dds_session, 20);

        if (!sensor_dds_connected)
        {
            sensor_dds_fail_count++;

            if (sensor_dds_fail_count >= 10)
            {
                k_sched_lock();
                uxr_delete_session(&sensor_dds_session);
                uxr_close_udp_transport(&sensor_dds_transport);

                k_event_set(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_NO_DDS);

                k_sched_unlock();

                printf("R-Car Session Disconnected\n");
            }
        } else {
            sensor_dds_fail_count = 0;
        }

        k_yield();
    }
}

void sensor_on_msg(uxrSession *sess, uxrObjectId obj_id, uint16_t req_id, \
    uxrStreamId stream, struct ucdrBuffer *ub, uint16_t len, void *_)
{
    (void)sess;
	(void)req_id;
	(void)stream;
	(void)_;
	(void)len;

	// printf("on_msg");

	if (obj_id.type == UXR_DATAREADER_ID && obj_id.id == 0x1)
	{
		bool brake_apply;
        float mps;
		ucdr_deserialize_endian_bool(ub, UCDR_LITTLE_ENDIANNESS, &brake_apply);
		ucdr_deserialize_endian_float(ub, UCDR_LITTLE_ENDIANNESS, &mps);

		// printf("on_msg %d", brake_apply);

		sensor_brake_signal = brake_apply;
        sensor_mps = mps;
        new_msg = true;
        printf("Recv'd DDS Message. Signal: %x, MPS: %f\n", brake_apply, mps);
	}
}
