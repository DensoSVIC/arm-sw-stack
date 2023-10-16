/*
 * Based on: https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/drivers/arm/rss/rss_comms.c?h=v2.9.0
 * In open-source project: TF-A/trusted-firmware-a
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2022 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes:
 * 1) Remove initialization function as this is done by Zephyr MHU driver.
 * 2) Replace mhu_send_data and mhu_receive_data with the PSA backend APIs.
 * 3) Use a semaphore to synchronize access of multiple threads.
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(psa_service, CONFIG_PSA_SERVICE_LOG_LEVEL);

#include <zephyr/kernel.h>
#include "zephyr/subsys/ipc/psa_service/psa_backend.h"
#include "rss_comms/rss_comms_protocol.h"

static struct device *dev;
static struct k_sem psa_call_sem;

/* Union as message space and reply space are never used at the same time,
 * and this saves space as we can overlap them.
 */
union __packed __aligned(4) rss_comms_io_buffer_t
{
	struct serialized_rss_comms_msg_t msg;
	struct serialized_rss_comms_reply_t reply;
};

static uint8_t select_protocol_version(const struct psa_invec *in_vec, size_t in_len,
				       const struct psa_outvec *out_vec, size_t out_len)
{
	size_t embed_msg_min_size;
	size_t embed_reply_min_size;
	size_t in_size_total = 0;
	size_t out_size_total = 0;
	size_t i;

	for (i = 0U; i < in_len; ++i) {
		in_size_total += in_vec[i].len;
	}
	for (i = 0U; i < out_len; ++i) {
		out_size_total += out_vec[i].len;
	}

	embed_msg_min_size = sizeof(struct serialized_rss_comms_header_t) +
			     sizeof(struct rss_embed_msg_t) - RSS_COMMS_PAYLOAD_MAX_SIZE;

	embed_reply_min_size = sizeof(struct serialized_rss_comms_header_t) +
			       sizeof(struct rss_embed_reply_t) - RSS_COMMS_PAYLOAD_MAX_SIZE;

	/* Use embed if we can pack into one message and reply, else use
	 * pointer_access. The underlying MHU transport protocol uses a
	 * single uint32_t to track the length, so the amount of data that
	 * can be in a message is 4 bytes less than MHU3_MAX_MSG_SIZE
	 * reports.
	 *
	 * TODO tune this with real performance numbers, it's possible a
	 * pointer_access message is less performant than multiple embed
	 * messages due to ATU configuration costs to allow access to the
	 * pointers.
	 */
	if ((embed_msg_min_size + in_size_total > MHU3_MAX_MSG_SIZE - sizeof(uint32_t)) ||
	    (embed_reply_min_size + out_size_total > MHU3_MAX_MSG_SIZE - sizeof(uint32_t))) {
		return RSS_COMMS_PROTOCOL_POINTER_ACCESS;
	}

	return RSS_COMMS_PROTOCOL_EMBED;
}

psa_status_t psa_call(psa_handle_t handle, int32_t type, const struct psa_invec *in_vec,
		      size_t in_len, struct psa_outvec *out_vec, size_t out_len)
{
	/* Take semaphore to prevent other threads
	 * on the cluster from running psa_call
	 */
	k_sem_take(&psa_call_sem, K_FOREVER);

	static union rss_comms_io_buffer_t io_buf;
	psa_status_t status;
	static uint8_t seq_num = 1U;
	size_t msg_size;
	size_t reply_size = sizeof(io_buf.reply);
	psa_status_t return_val;
	size_t idx;

	const struct psa_service_backend *api = dev->api;

	if (type > INT16_MAX || type < INT16_MIN || in_len > PSA_MAX_IOVEC ||
	    out_len > PSA_MAX_IOVEC) {
		return_val = PSA_ERROR_INVALID_ARGUMENT;
		goto clean;
	}

	io_buf.msg.header.seq_num = seq_num;
	/* No need to distinguish callers
	 * (currently concurrent calls are not supported).
	 */
	io_buf.msg.header.client_id = 1U;
	io_buf.msg.header.protocol_ver = select_protocol_version(in_vec, in_len, out_vec, out_len);

	/* Serialize message before sending to RSS */
	status = rss_protocol_serialize_msg(handle, type, in_vec, in_len, out_vec, out_len,
					    &io_buf.msg, &msg_size);
	if (status != PSA_SUCCESS) {
		LOG_ERR("rss_protocol_serialize_msg failed with status %d\n", status);
		return_val = status;
		goto clean;
	}

	LOG_DBG("[RSS-COMMS] Sending message\n");
	LOG_DBG("handle=0x%x\n", handle);
	LOG_DBG("type=%d\n", type);
	LOG_DBG("protocol_ver=%u\n", io_buf.msg.header.protocol_ver);
	LOG_DBG("seq_num=%u\n", io_buf.msg.header.seq_num);
	LOG_DBG("client_id=%u\n", io_buf.msg.header.client_id);
	for (idx = 0; idx < in_len; idx++) {
		LOG_DBG("in_vec[%lu].len=%lu\n", idx, in_vec[idx].len);
		LOG_DBG("in_vec[%lu].buf=%p\n", idx, (void *)in_vec[idx].base);
	}

	/* Send the message */
	status = api->send(dev, (uint8_t *)&io_buf.msg, msg_size);
	if (status != PSA_SUCCESS) {
		LOG_ERR("[RSS-COMMS] PSA transmission failed with status %d\n", status);
		return_val = status;
		goto clean;
	}

	/* Receive the reply */
	status = api->receive(dev, (uint8_t *)&io_buf.reply, &reply_size);
	if (status != PSA_SUCCESS) {
		LOG_ERR("[RSS-COMMS] PSA reply reception failed with status %d\n", status);
		return_val = status;
		goto clean;
	}

	LOG_DBG("[RSS-COMMS] Received reply\n");
	LOG_DBG("protocol_ver=%u\n", io_buf.reply.header.protocol_ver);
	LOG_DBG("seq_num=%u\n", io_buf.reply.header.seq_num);
	LOG_DBG("client_id=%u\n", io_buf.reply.header.client_id);

	/* Deserialize reply from RSS */
	status = rss_protocol_deserialize_reply(out_vec, out_len, &return_val, &io_buf.reply,
						reply_size);

	if (status != PSA_SUCCESS) {
		LOG_ERR("rss_protocol_deserialize_reply failed with status %d\n", status);
		return_val = status;
		goto clean;
	}

	LOG_DBG("return_val=%d\n", return_val);
	for (idx = 0U; idx < out_len; idx++) {
		LOG_DBG("out_vec[%lu].len=%lu\n", idx, out_vec[idx].len);
		LOG_DBG("out_vec[%lu].buf=%p\n", idx, (void *)out_vec[idx].base);
	}

	/* Increment Sequence number (Roll over is allowed) */
	seq_num++;

clean:
	/* Clear the MHU message buffer to remove assets from memory */
	memset(&io_buf, 0x0, sizeof(io_buf));
	/* Give semaphore to allow other threads
	 * on the cluster to run psa_call
	 */
	k_sem_give(&psa_call_sem);

	return return_val;
}

static int psa_service_init(void)
{
	/* Initialize Semaphore */
	k_sem_init(&psa_call_sem, 1, 1);

	/* Set the PSA device */
	dev = DEVICE_DT_GET(DT_NODELABEL(psa));

	if (!device_is_ready(dev)) {
		LOG_ERR("[RSS-COMMS] IPC PSA device is not ready\n");
		return -ENODEV;
	}

	return 0;
}

SYS_INIT(psa_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
