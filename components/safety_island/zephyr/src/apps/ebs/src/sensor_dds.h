#pragma once

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <uxr/client/util/ping.h>
#include <uxr/client/util/time.h>

#include <zephyr/kernel.h>

#define SENSOR_STREAM_HISTORY 8
#define SENSOR_BUFFER_SIZE    1024

void sensor_dds_setup(void *, void *, void *);
void sensor_dds_process(void *, void *, void *);

void sensor_on_msg(uxrSession *sess, uxrObjectId obj_id, uint16_t req_id, 
    uxrStreamId stream, struct ucdrBuffer *ub, uint16_t len, void *_);