#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef CONFIG_GPIO_RCAR
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/gpio.h>
#endif

#include "brake_feedback_dds.h"
#include "sensor_dds.h"
#ifdef CONFIG_GPIO_RCAR
#include "gpio4_button.h"
#endif

#define MAX_IFACES 2
#define STREAM_HISTORY  8
#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU* STREAM_HISTORY

extern struct k_event SENSOR_DDS_SETUP_EVENT;
extern struct k_event BRAKE_DDS_SETUP_EVENT;

enum SENSOR_DDS_EVENTS 
{
    SENSOR_DDS_NO_DDS = 0b01,
    SENSOR_DDS_CONNECTED = 0b10
};

enum BRAKE_DDS_EVENTS 
{
    BRAKE_DDS_NO_DDS = 0b01,
    BRAKE_DDS_CONNECTED = 0b10
};

K_THREAD_DEFINE(brake_dds_setup_tid, 10000, brake_dds_setup, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(brake_dds_process_tid, 10000, brake_dds_process, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(brake_dds_check_tid, 10000, brake_dds_check, NULL, NULL, NULL, 5, 0, 0);

K_THREAD_DEFINE(sensor_dds_setup_tid, 10000, sensor_dds_setup, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(sensor_dds_process_tid, 10000, sensor_dds_process, NULL, NULL, NULL, 5, 0, 0);

#ifdef CONFIG_GPIO_RCAR
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)

static const struct led_dt_spec led0 = LED_DT_SPEC_GET(LED0_NODE);
static const struct led_dt_spec led1 = LED_DT_SPEC_GET(LED1_NODE);
static const struct led_dt_spec led2 = LED_DT_SPEC_GET(LED2_NODE);
#endif

struct iface_list {
    struct net_if *ifaces[MAX_IFACES];
    int count;
};

static void net_if_cb(struct net_if *iface, void *user_data)
{
    struct iface_list *ctx = (struct iface_list *)user_data;
    
    if (ctx->count < MAX_IFACES) {
        ctx->ifaces[ctx->count++] = iface;
    }
}

static int setup_iface(struct net_if *iface, const char *addr, const char *gw, 
                       const char *netmask, uint16_t tag)
{
    struct in_addr inaddr;

    if (net_addr_pton(AF_INET, addr, &inaddr)) {
        printf("Invalid address: %s\n", addr);
        return 1;
    }

    if (!net_if_ipv4_addr_add(iface, &inaddr, NET_ADDR_MANUAL, 0)) {
        printf("Cannot add %s to interface %p\n", addr, iface);
        return 1;
    }

    if (net_addr_pton(AF_INET, gw, &inaddr)) {
        printf("Invalid address: %s\n", gw);
        return 1;
    }
    net_if_ipv4_set_gw(iface, &inaddr);

    if (net_addr_pton(AF_INET, netmask, &inaddr)) {
        printf("Invalid address: %s\n", netmask);
        return 1;
    }
    net_if_ipv4_set_netmask(iface, &inaddr);

#if defined(CONFIG_NET_VLAN)
    if (tag > 0) {
        int ret = net_eth_vlan_enable(iface, tag);
        if (ret < 0) {
            printf("Cannot set VLAN tag %d to interface %p\n", tag, iface);
            return 1;
        }
    }
#endif

    return 0;
}

int main(void)
{
    struct iface_list ifs = {0};
    net_if_foreach(net_if_cb, &ifs);
    if (ifs.count >= 1 && sizeof(CONFIG_NET_IFACE1_ADDR) > 1) {
        int ret = setup_iface(
            ifs.ifaces[0],
            CONFIG_NET_IFACE1_ADDR,
            CONFIG_NET_IFACE1_GW,
            CONFIG_NET_IFACE1_NETMASK,
            CONFIG_NET_IFACE1_VLAN
        );
        if (ret) {
            return 1;
        }
    }

    if (ifs.count >= 2 && sizeof(CONFIG_NET_IFACE2_ADDR) > 1) {
        int ret = setup_iface(
            ifs.ifaces[1],
            CONFIG_NET_IFACE2_ADDR,
            CONFIG_NET_IFACE2_GW,
            CONFIG_NET_IFACE2_NETMASK,
            CONFIG_NET_IFACE2_VLAN
        );
        if (ret) {
            return 1;
        }
    }

    #ifdef CONFIG_GPIO_RCAR
    led_off_dt(&led0);
    led_off_dt(&led1);
    led_off_dt(&led2);

    button_setup();
    #endif

    k_event_set(&SENSOR_DDS_SETUP_EVENT, SENSOR_DDS_NO_DDS);
    k_event_set(&BRAKE_DDS_SETUP_EVENT, BRAKE_DDS_NO_DDS);

    while(1) { 
        
        if(k_thread_join(brake_dds_setup_tid, K_NO_WAIT) != -EBUSY) 
        {
            printf("brake_dds_setup_tid thread exited!!!\n");
        }

        if(k_thread_join(brake_dds_process_tid, K_NO_WAIT) != -EBUSY) 
        {
            printf("brake_dds_process_tid thread exited!!!\n");
        }

        if(k_thread_join(brake_dds_check_tid, K_NO_WAIT) != -EBUSY) 
        {
            printf("brake_dds_check_tid thread exited!!!\n");
        }

        if(k_thread_join(sensor_dds_setup_tid, K_NO_WAIT) != -EBUSY) 
        {
            printf("sensor_dds_setup_tid thread exited!!!\n");
        }

        if(k_thread_join(sensor_dds_process_tid, K_NO_WAIT) != -EBUSY) 
        {
            printf("sensor_dds_process_tid thread exited!!!\n");
        }

        k_sleep(K_SECONDS(2)); 

        printf("[%07lli] Alive!\n", uxr_millis());
    }

    return 0;
}


