/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/logging/log.h>

#include "ipm_mhuv3.h"

#define DT_DRV_COMPAT arm_mhuv3

#define LOG_LEVEL CONFIG_IPM_LOG_LEVEL
LOG_MODULE_REGISTER(ipm_mhuv3);

#define MHUV3_DB_CHS_PER_WINDOW	32

static int ipm_mhuv3_doorbell_send(const struct device *dev, uint32_t ch_index,
				   const void *data, int size)
{
	ARG_UNUSED(data);
	ARG_UNUSED(size);

	const struct mhuv3_pdbcw_reg *pr = MHUV3_PDBCW(dev);
	uint32_t idx, bit;

	idx = ch_index / MHUV3_DB_CHS_PER_WINDOW;
	bit = ch_index % MHUV3_DB_CHS_PER_WINDOW;

	if (!sys_test_bit((mem_addr_t)&(pr[idx].pdbcw_st), bit)) {
		sys_set_bit((mem_addr_t)&(pr[idx].pdbcw_set), bit);
		return 0;
	}

	return -EBUSY;
}

static void ipm_mhuv3_doorbell_recv(const struct device *dev, uint32_t ch_index)
{
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	struct ipm_mhuv3_device_data *driver_data = MHUV3_DEV_DATA(dev);
	uint32_t idx, bit;

	if ((driver_data != NULL) && (driver_data->callback != NULL)) {
		driver_data->callback(dev, driver_data->user_data, ch_index, NULL);
	}

	idx = ch_index / MHUV3_DB_CHS_PER_WINDOW;
	bit = ch_index % MHUV3_DB_CHS_PER_WINDOW;
	sys_set_bit((mem_addr_t)&(mr[idx].mdbcw_clr), bit);
}

static int ipm_mhuv3_send(const struct device *dev, int wait, uint32_t ch_id,
			  const void *data, int size)
{
	ARG_UNUSED(wait);
	struct chan_info *p_channel = &(MHUV3_DEV_DATA(dev)->tx_ch_array[ch_id]);

	if (!p_channel->send) {
		LOG_ERR("MHUv3 send fail, channel %d no send func\n", ch_id);
		return -EINVAL;
	}

	return p_channel->send(dev, ch_id, data, size);
}

static void ipm_mhuv3_register_cb(const struct device *dev, ipm_callback_t cb,
				  void *user_data)
{
	struct ipm_mhuv3_device_data *driver_data = MHUV3_DEV_DATA(dev);

	driver_data->callback = cb;
	driver_data->user_data = user_data;
}

static int ipm_mhuv3_max_data_size_get(const struct device *dev)
{
	ARG_UNUSED(dev);
	return IPM_MHUV3_MAX_DATA_SIZE;
}
static uint32_t ipm_mhuv3_max_send_id(const struct device *dev)
{
	return MHUV3_DEV_DATA(dev)->tx_num_ch;
}

static int ipm_mhuv3_set_enabled(const struct device *dev, int enable)
{
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	int num_windows = sys_read32((mem_addr_t)&mcr->mbx_dbch_cfg0) + 1;
	mem_addr_t mdbcw_msk_reg;

	for (int i = 0; i < num_windows; i++) {
		mdbcw_msk_reg = enable ? (mem_addr_t)&(mr[i].mdbcw_msk_clr) :
					 (mem_addr_t)&(mr[i].mdbcw_msk_set);
		sys_write32(0xFFFFFFFF, mdbcw_msk_reg);
	}

	return 0;
}

static bool ipm_mhuv3_has_dbch_int(const struct device *dev)
{
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);

	/*
	 * The length is 4, indicating the status of the receiver channel
	 * combined interrupt for the doorbell channels:
	 * mcr->mbx_dbch_int_st[0] has status for doorbell channels 0 ~ 31
	 * mcr->mbx_dbch_int_st[1] has status for doorbell channels 32 ~ 63
	 * mcr->mbx_dbch_int_st[2] has status for doorbell channels 64 ~ 95
	 * mcr->mbx_dbch_int_st[3] has status for doorbell channels 96 ~ 127
	 *
	 * Each one of the 128 doorbell channel contains 32 bit channel. The 32
	 * bit channel share one int st.
	 */
	if (sys_read32((mem_addr_t)&(mcr->mbx_dbch_int_st[0])) ||
	    sys_read32((mem_addr_t)&(mcr->mbx_dbch_int_st[1])) ||
	    sys_read32((mem_addr_t)&(mcr->mbx_dbch_int_st[2])) ||
	    sys_read32((mem_addr_t)&(mcr->mbx_dbch_int_st[3]))) {
		return true;
	}

	return false;
}

static void ipm_mhuv3_dbch_handler(const struct device *dev)
{
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	uint32_t num_chans = MHUV3_DEV_DATA(dev)->rx_num_ch;
	uint32_t num_windows = num_chans / MHUV3_DB_CHS_PER_WINDOW;
	struct chan_info *p_channel;

	for (int idx = 0; idx < num_windows; idx++) {
		int dbch_int_st_idx = idx / MHUV3_DB_CHS_PER_WINDOW;
		int dbch_int_st_sub_idx = idx % MHUV3_DB_CHS_PER_WINDOW;
		uint32_t dbch_int_st;
		uint32_t dbch_st_msk;
		int sub_idx;
		int ch_idx;

		/* Quickly check the idx window's signal */
		dbch_int_st = sys_read32((mem_addr_t)&(mcr->mbx_dbch_int_st[dbch_int_st_idx]));
		if (!(dbch_int_st & (1UL << dbch_int_st_sub_idx))) {
			continue;
		}

		dbch_st_msk = sys_read32((mem_addr_t)&(mr[idx].mdbcw_st_msk));
		while (dbch_st_msk) {
			/* Get one bit channel from the mdbcw_st_msk*/
			sub_idx = __builtin_ctz(dbch_st_msk);
			/* Clear it */
			dbch_st_msk &= ~(1UL << sub_idx);
			/* Calculate the bit channel idx */
			ch_idx = sub_idx + idx * MHUV3_DB_CHS_PER_WINDOW;
			/* idx < num_windows, thus ch_idx < num_chans*/
			p_channel = &(MHUV3_DEV_DATA(dev)->rx_ch_array[ch_idx]);
			if (p_channel == NULL || p_channel->recv == NULL) {
				LOG_ERR("Empty info for MHU channel: %u\n", ch_idx);
				continue;
			}
			/* Call channel recv call back */
			p_channel->recv(dev, ch_idx);
		}
	}
}

static void ipm_mhuv3_isr(const struct device *dev)
{
	if (ipm_mhuv3_has_dbch_int(dev)) {
		ipm_mhuv3_dbch_handler(dev);
	}
}

static void ipm_mhuv3_chan_init(struct chan_info *ch_array, int length,
				mhuv3_send_t send_cb, mhuv3_recv_t recv_cb)
{
	for (int i = 0; i < length; i++) {
		ch_array[i].recv = recv_cb;
		ch_array[i].send = send_cb;
	}
}

static int ipm_mhuv3_doorbell_rx_init(const struct device *dev)
{
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	uint32_t num_windows = sys_read32((mem_addr_t)&mcr->mbx_dbch_cfg0) + 1;
	uint32_t rx_num_ch = num_windows * MHUV3_DB_CHS_PER_WINDOW;

	/* Disable all the channel windows */
	for (int i = 0; i < num_windows; i++) {
		sys_write32(0xFFFFFFFF, (mem_addr_t)&(mr[i].mdbcw_msk_set));
	}

	if (rx_num_ch > CONFIG_IPM_MHUV3_MAX_LOGICAL_CH_NUM) {
		LOG_ERR("Invalid rx channel nums: %u, (MAX: %u)\n",
			rx_num_ch, CONFIG_IPM_MHUV3_MAX_LOGICAL_CH_NUM);
		return -EINVAL;
	}

	ipm_mhuv3_chan_init(MHUV3_DEV_DATA(dev)->rx_ch_array, rx_num_ch,
			    NULL, ipm_mhuv3_doorbell_recv);
	MHUV3_DEV_DATA(dev)->rx_num_ch = rx_num_ch;

	return 0;
}

static int ipm_mhuv3_doorbell_tx_init(const struct device *dev)
{
	const struct mhuv3_pbx_ctrl_reg *pcr = MHUV3_PBX_CTRL(dev);
	uint32_t num_windows = sys_read32((mem_addr_t)&pcr->pbx_dbch_cfg0) + 1;
	uint32_t tx_num_ch = num_windows * MHUV3_DB_CHS_PER_WINDOW;

	if (tx_num_ch > CONFIG_IPM_MHUV3_MAX_LOGICAL_CH_NUM) {
		LOG_ERR("Invalid tx channel nums: %u, (MAX: %u)\n",
			tx_num_ch, CONFIG_IPM_MHUV3_MAX_LOGICAL_CH_NUM);
		return -EINVAL;
	}

	ipm_mhuv3_chan_init(MHUV3_DEV_DATA(dev)->tx_ch_array, tx_num_ch,
			    ipm_mhuv3_doorbell_send, NULL);
	MHUV3_DEV_DATA(dev)->tx_num_ch = tx_num_ch;

	return 0;
}

static int ipm_mhuv3_doorbell_init(const struct device *dev)
{
	int ret = 0;
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_pbx_ctrl_reg *pcr = MHUV3_PBX_CTRL(dev);

	if (sys_read32((mem_addr_t)&mcr->mbx_feat_spt0) & 0xF) {
		ret = ipm_mhuv3_doorbell_rx_init(dev);
	}

	if (ret != 0) {
		return ret;
	}

	if (sys_read32((mem_addr_t)&pcr->pbx_feat_spt0) & 0xF) {
		ret = ipm_mhuv3_doorbell_tx_init(dev);
	}

	return ret;
}

static int ipm_mhuv3_init(const struct device *dev)
{
	const struct ipm_mhuv3_device_config *config = MHUV3_DEV_CFG(dev);
	int ret = 0;

	LOG_DBG("MHU init tx: 0x%llx, rx: 0x%llx\n",
		(uint64_t)config->tx_base, (uint64_t)config->rx_base);

	ret = ipm_mhuv3_doorbell_init(dev);
	if (ret != 0) {
		LOG_ERR("IPM MHUv3 Doorbell init failed, ret:%d\n", ret);
		return ret;
	}

	if (config->rx_irq_config_func)
		config->rx_irq_config_func(dev);
	return ret;
}

static const struct ipm_driver_api ipm_mhuv3_driver_api = {
	.send = ipm_mhuv3_send,
	.register_callback = ipm_mhuv3_register_cb,
	.max_data_size_get = ipm_mhuv3_max_data_size_get,
	.max_id_val_get = ipm_mhuv3_max_send_id,
	.set_enabled = ipm_mhuv3_set_enabled,
};

#define IPM_MHUV3_INIT(n)						\
static void ipm_mhuv3_irq_config_func_rx_##n(const struct device *d);	\
static const struct ipm_mhuv3_device_config ipm_mhuv3_cfg_##n = {	\
	.rx_base = (uint8_t *)DT_INST_REG_ADDR_BY_NAME(n, rx),		\
	.tx_base = (uint8_t *)DT_INST_REG_ADDR_BY_NAME(n, tx),		\
	.rx_irq_config_func = ipm_mhuv3_irq_config_func_rx_##n,		\
};									\
struct ipm_mhuv3_device_data ipm_mhuv3_data_##n = {			\
	.callback = NULL,						\
	.user_data = NULL,						\
	.tx_num_ch = 0,							\
	.rx_num_ch = 0,							\
};									\
DEVICE_DT_INST_DEFINE(n,						\
		      &ipm_mhuv3_init,					\
		      NULL,						\
		      &ipm_mhuv3_data_##n, &ipm_mhuv3_cfg_##n,		\
		      POST_KERNEL,					\
		      CONFIG_KERNEL_INIT_PRIORITY_DEVICE,		\
		      &ipm_mhuv3_driver_api);				\
static void ipm_mhuv3_irq_config_func_rx_##n(const struct device *d)	\
{									\
	IRQ_CONNECT(DT_INST_IRQ_BY_NAME(n, rx, irq),			\
		    DT_INST_IRQ_BY_NAME(n, rx, priority),		\
		    ipm_mhuv3_isr,					\
		    DEVICE_DT_INST_GET(n),				\
		    0);							\
	irq_enable(DT_INST_IRQ_BY_NAME(n, rx, irq));			\
}

DT_INST_FOREACH_STATUS_OKAY(IPM_MHUV3_INIT)
