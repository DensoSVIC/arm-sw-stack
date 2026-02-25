#pragma once

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <uxr/client/util/ping.h>
#include <uxr/client/util/time.h>

#include <zephyr/kernel.h>

#define BRAKE_STREAM_HISTORY 8
#define BRAKE_BUFFER_SIZE    1024

void brake_dds_setup(void *, void *, void *);
void brake_dds_check(void *, void *, void *);
void brake_dds_process(void *, void *, void *);

void brake_on_msg(uxrSession *sess, uxrObjectId obj_id, uint16_t req_id, 
    uxrStreamId stream, struct ucdrBuffer *ub, uint16_t len, void *_);
