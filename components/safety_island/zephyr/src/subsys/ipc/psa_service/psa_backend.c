/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa_backend_priv.h"
#include "zephyr/subsys/ipc/psa_service/psa/client.h"
#include "zephyr/subsys/ipc/psa_service/psa_backend.h"

#define PSA_DEVICE DT_NODELABEL(psa)

static void mbox_callback_process(struct k_work *item)
{
	struct psa_backend_data_t *dev_data =
		CONTAINER_OF(item, struct psa_backend_data_t, mbox_work);
	struct k_poll_signal *receive_signal = &dev_data->receive_signal;

	/* Signal reception of reply from RSS */
	k_poll_signal_raise(receive_signal, 0);
}

static void mbox_callback(const struct device *instance, uint32_t channel, void *user_data,
			  struct mbox_msg *msg_data)
{
	struct psa_backend_data_t *dev_data = user_data;

	k_work_submit(&dev_data->mbox_work);
}

static int mbox_init(const struct psa_backend_config_t *conf, struct psa_backend_data_t *dev_data)
{
	int err;

	k_work_init(&dev_data->mbox_work, mbox_callback_process);

	err = mbox_register_callback(&conf->mbox_rx, mbox_callback, dev_data);
	if (err != 0) {
		return err;
	}

	return mbox_set_enabled(&conf->mbox_rx, 1);
}

static int psa_backend_init(const struct device *dev)
{
	const struct psa_backend_config_t *conf = dev->config;
	struct psa_backend_data_t *dev_data = dev->data;

	struct k_poll_signal *receive_signal = &dev_data->receive_signal;

	/* Initialize Receive Signal */
	k_poll_signal_init(receive_signal);

	/* Initialize Receive Event */
	k_poll_event_init(&dev_data->receive_event[0], K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY,
			  receive_signal);

	/* Initialize mailbox */
	return mbox_init(conf, dev_data);
}

psa_status_t psa_backend_send(const struct device *dev, const uint8_t *send_buffer, size_t size)
{
	const struct psa_backend_config_t *conf = dev->config;
	int err;

	if (conf->tx_shm_size < MHU3_OUTBAND_BUF_HEADER_SIZE + size) {
		return PSA_ERROR_INSUFFICIENT_STORAGE;
	}

	/*
	 * First 4 bytes represents the header which currently stores the size
	 * of msg. Rest of buffer holds the actual message.
	 */
	(void)memcpy((void *)conf->tx_shm_addr, (void *)&size, MHU3_OUTBAND_BUF_HEADER_SIZE);

	/* Copy the message */
	(void)memcpy((void *)(conf->tx_shm_addr + MHU3_OUTBAND_BUF_HEADER_SIZE),
		     (void *)send_buffer, size);

	err = mbox_send(&conf->mbox_tx, NULL);
	if (err) {
		return PSA_ERROR_COMMUNICATION_FAILURE;
	}

	return PSA_SUCCESS;
}

psa_status_t psa_backend_receive(const struct device *dev, uint8_t *receive_buffer, size_t *size)
{
	const struct psa_backend_config_t *conf = dev->config;
	struct psa_backend_data_t *dev_data = dev->data;

	struct k_poll_event *receive_event = dev_data->receive_event;

	size_t len = 0U;

	/* Wait for reply to be received */
	/* TODO: Set a valid reply timeout */
	k_poll(receive_event, 1, K_FOREVER);

	/* Copy reply length */
	memcpy((void *)&len, (void *)conf->rx_shm_addr, MHU3_OUTBAND_BUF_HEADER_SIZE);

	if (len == 0) {
		/* Unlikely, No data in buffer */
		return PSA_ERROR_GENERIC_ERROR;
	}
	/* Verify that reply is not bigger than available buffer */
	else if (len > *size) {
		return PSA_ERROR_INSUFFICIENT_STORAGE;
	}

	*size = len;

	/* Copy the message */
	(void)memcpy((void *)receive_buffer,
		     (void *)(conf->rx_shm_addr + MHU3_OUTBAND_BUF_HEADER_SIZE), len);

	/* Reset reception event */
	receive_event->signal->signaled = 0;
	receive_event->state = K_POLL_STATE_NOT_READY;

	return PSA_SUCCESS;
}

static const struct psa_service_backend psa_service_backend_api = {
	.send = psa_backend_send,
	.receive = psa_backend_receive,
};

static const struct psa_backend_config_t psa_backend_config = {
	.tx_shm_size = DT_REG_SIZE(DT_PHANDLE(PSA_DEVICE, tx_region)),
	.tx_shm_addr = DT_REG_ADDR(DT_PHANDLE(PSA_DEVICE, tx_region)),
	.rx_shm_size = DT_REG_SIZE(DT_PHANDLE(PSA_DEVICE, rx_region)),
	.rx_shm_addr = DT_REG_ADDR(DT_PHANDLE(PSA_DEVICE, rx_region)),
	.mbox_tx = MBOX_DT_CHANNEL_GET(PSA_DEVICE, tx),
	.mbox_rx = MBOX_DT_CHANNEL_GET(PSA_DEVICE, rx),
};

static struct psa_backend_data_t psa_backend_data;

DEVICE_DT_DEFINE(PSA_DEVICE, &psa_backend_init, NULL, &psa_backend_data, &psa_backend_config,
		 POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &psa_service_backend_api);
