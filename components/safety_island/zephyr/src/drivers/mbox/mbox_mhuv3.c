/*
 * SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "mbox_mhuv3.h"

#define MBOX_RX		0
#define MBOX_TX		1
#define DBE_SPT		0xf

#define MHUV3_DB_CHS_PER_WINDOW		32

#define LOG_LEVEL	CONFIG_MBOX_LOG_LEVEL
LOG_MODULE_REGISTER(mbox_mhuv3);

static int mbox_mhuv3_send(const struct device *dev, uint32_t channel,
			   const struct mbox_msg *msg)
{
	const struct mhuv3_pdbcw_reg *pr = MHUV3_PDBCW(dev);
	uint32_t idx, bit;

	if (msg) {
		LOG_WRN("Sending data not supported");
	}

	idx = channel / MHUV3_DB_CHS_PER_WINDOW;
	bit = channel % MHUV3_DB_CHS_PER_WINDOW;

	if (!sys_test_bit((mem_addr_t)&pr[idx].pdbcw_st, bit)) {
		sys_set_bit((mem_addr_t)&pr[idx].pdbcw_set, bit);
		return 0;
	}

	return -EBUSY;
}

static int mbox_mhuv3_register_cb(const struct device *dev, uint32_t channel,
				  mbox_callback_t cb, void *user_data)
{
	struct mbox_mhuv3_data *data = MHUV3_DEV_DATA(dev);

	if (channel >= CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM) {
		return -EINVAL;
	}

	data->cb[channel] = cb;
	data->user_data[channel] = user_data;

	return 0;
}

static int mbox_mhuv3_mtu_get(const struct device *dev)
{
	ARG_UNUSED(dev);
	return 0;
}

static uint32_t mbox_mhuv3_max_channels_get(const struct device *dev)
{
	ARG_UNUSED(dev);
	return CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM;
}

static int mbox_mhuv3_set_enabled(const struct device *dev, uint32_t channel,
				  bool enable)
{
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	mem_addr_t mdbcw_msk_reg;
	uint32_t idx, bit;

	__ASSERT_NO_MSG(channel < MHUV3_DB_CHS_PER_WINDOW);

	idx = channel / MHUV3_DB_CHS_PER_WINDOW;
	bit = channel % MHUV3_DB_CHS_PER_WINDOW;

	mdbcw_msk_reg = enable ? (mem_addr_t)&mr[idx].mdbcw_msk_clr :
				 (mem_addr_t)&mr[idx].mdbcw_msk_set;
	sys_set_bit(mdbcw_msk_reg, bit);

	return 0;
}

static bool mbox_mhuv3_has_dbch_int(const struct device *dev)
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
	if (sys_read32((mem_addr_t)&mcr->mbx_dbch_int_st[0]) ||
	    sys_read32((mem_addr_t)&mcr->mbx_dbch_int_st[1]) ||
	    sys_read32((mem_addr_t)&mcr->mbx_dbch_int_st[2]) ||
	    sys_read32((mem_addr_t)&mcr->mbx_dbch_int_st[3])) {
		return true;
	}

	return false;
}

static void mbox_mhuv3_dbch_handler(const struct device *dev)
{
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	const struct mbox_mhuv3_data *data = MHUV3_DEV_DATA(dev);
	uint32_t num_chans = MHUV3_DEV_DATA(dev)->num_ch;
	uint32_t num_windows = num_chans / MHUV3_DB_CHS_PER_WINDOW;

	for (uint32_t idx = 0; idx < num_windows; idx++) {
		int dbch_int_st_idx = idx / MHUV3_DB_CHS_PER_WINDOW;
		int dbch_int_st_sub_idx = idx % MHUV3_DB_CHS_PER_WINDOW;
		uint32_t dbch_int_st, dbch_st_msk, ch_idx;
		mem_addr_t addr;
		int sub_idx;

		/* Quickly check the idx window's signal */
		addr = (mem_addr_t)&mcr->mbx_dbch_int_st[dbch_int_st_idx];
		dbch_int_st = sys_read32(addr);
		if (!(dbch_int_st & (1UL << dbch_int_st_sub_idx))) {
			continue;
		}

		addr = (mem_addr_t)&mr[idx].mdbcw_st_msk;
		dbch_st_msk = sys_read32(addr);
		while (dbch_st_msk) {
			/* Get one bit channel from the mdbcw_st_msk*/
			sub_idx = __builtin_ctz(dbch_st_msk);
			/* Clear it */
			dbch_st_msk &= ~(1UL << sub_idx);
			/* Calculate the bit channel idx */
			ch_idx = sub_idx + idx * MHUV3_DB_CHS_PER_WINDOW;
			/* idx < num_windows, thus ch_idx < num_chans*/
			if (!data->cb[ch_idx]) {
				LOG_WRN("Empty info for MHU channel: %u\n",
					ch_idx);
				continue;
			}
			/* Call channel call back */
			data->cb[ch_idx](dev, ch_idx,
					 data->user_data[ch_idx], NULL);
			addr = (mem_addr_t)&mr[dbch_int_st_idx].mdbcw_clr;
			sys_set_bit(addr, sub_idx);
		}
	}
}

static void mbox_mhuv3_isr(const struct device *dev)
{
	if (mbox_mhuv3_has_dbch_int(dev)) {
		mbox_mhuv3_dbch_handler(dev);
	}
}

static int mbox_mhuv3_rx_init(const struct device *dev)
{
	const struct mbox_mhuv3_config *cfg = MHUV3_DEV_CFG(dev);
	const struct mhuv3_mbx_ctrl_reg *mcr = MHUV3_MBX_CTRL(dev);
	const struct mhuv3_mdbcw_reg *mr = MHUV3_MDBCW(dev);
	uint32_t num_windows = sys_read32((mem_addr_t)&mcr->mbx_dbch_cfg0) + 1;
	uint32_t rx_num_ch = num_windows * MHUV3_DB_CHS_PER_WINDOW;

	if (!(sys_read32((mem_addr_t)&mcr->mbx_feat_spt0) & DBE_SPT)) {
		return 0;
	}

	/* Disable all the channel windows */
	for (int i = 0; i < num_windows; i++) {
		sys_write32(0xFFFFFFFF, (mem_addr_t)&mr[i].mdbcw_msk_set);
	}

	if (rx_num_ch > CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM) {
		LOG_ERR("Invalid rx channel nums: %u, (MAX: %u)\n",
			rx_num_ch, CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM);
		return -EINVAL;
	}

	MHUV3_DEV_DATA(dev)->num_ch = rx_num_ch;

	if (cfg->rx_irq_config) {
		cfg->rx_irq_config(dev);
	}

	LOG_DBG("MHU Init Rx: Base:0x%llx Rx Channels Supported:%u\n",
	        (uint64_t)cfg->base, rx_num_ch);

	return 0;
}

static int mbox_mhuv3_tx_init(const struct device *dev)
{
	const struct mbox_mhuv3_config *cfg = MHUV3_DEV_CFG(dev);
	const struct mhuv3_pbx_ctrl_reg *pcr = MHUV3_PBX_CTRL(dev);
	uint32_t num_windows = sys_read32((mem_addr_t)&pcr->pbx_dbch_cfg0) + 1;
	uint32_t tx_num_ch = num_windows * MHUV3_DB_CHS_PER_WINDOW;

	if (!(sys_read32((mem_addr_t)&pcr->pbx_feat_spt0) & DBE_SPT)) {
		return 0;
	}

	if (tx_num_ch > CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM) {
		LOG_ERR("Invalid tx channel nums: %u, (MAX: %u)\n",
			tx_num_ch, CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM);
		return -EINVAL;
	}

	MHUV3_DEV_DATA(dev)->num_ch = tx_num_ch;

	LOG_DBG("MHU Init Tx: Base:0x%llx Tx Channels Supported:%u\n",
		(uint64_t)cfg->base, tx_num_ch);

	return 0;
}

static const struct mbox_driver_api mbox_mhuv3_driver_api = {
	.send = mbox_mhuv3_send,
	.register_callback = mbox_mhuv3_register_cb,
	.mtu_get = mbox_mhuv3_mtu_get,
	.max_channels_get = mbox_mhuv3_max_channels_get,
	.set_enabled = mbox_mhuv3_set_enabled,
};

#define DT_DRV_COMPAT	arm_mhuv3_rx

#define MBOX_MHUV3_RX_INIT(n)						\
static void mbox_mhuv3_irq_config_rx_##n(const struct device *d);	\
static const struct mbox_mhuv3_config mbox_mhuv3_cfg_rx_##n = {		\
	.base = (uint8_t *)DT_INST_REG_ADDR(n),				\
	.rx_irq_config = mbox_mhuv3_irq_config_rx_##n,			\
};									\
struct mbox_mhuv3_data mbox_mhuv3_data_rx_##n = {			\
	.cb = {NULL},							\
	.user_data = {NULL},						\
	.num_ch = 0,							\
};									\
DEVICE_DT_INST_DEFINE(n,						\
		      &mbox_mhuv3_rx_init,				\
		      NULL,						\
		      &mbox_mhuv3_data_rx_##n, &mbox_mhuv3_cfg_rx_##n,	\
		      PRE_KERNEL_1,					\
		      CONFIG_KERNEL_INIT_PRIORITY_DEVICE,		\
		      &mbox_mhuv3_driver_api);				\
static void mbox_mhuv3_irq_config_rx_##n(const struct device *d)	\
{									\
	IRQ_CONNECT(DT_INST_IRQ_BY_NAME(n, rx, irq),			\
		    DT_INST_IRQ_BY_NAME(n, rx, priority),		\
		    mbox_mhuv3_isr,					\
		    DEVICE_DT_INST_GET(n),				\
		    0);							\
	irq_enable(DT_INST_IRQ_BY_NAME(n, rx, irq));			\
}

DT_INST_FOREACH_STATUS_OKAY(MBOX_MHUV3_RX_INIT);

#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT	arm_mhuv3_tx

#define MBOX_MHUV3_TX_INIT(n)						\
static const struct mbox_mhuv3_config mbox_mhuv3_cfg_tx_##n = {		\
	.base = (uint8_t *)DT_INST_REG_ADDR(n),				\
	.rx_irq_config = NULL,						\
};									\
struct mbox_mhuv3_data mbox_mhuv3_data_tx_##n = {			\
	.cb = {NULL},							\
	.user_data = {NULL},						\
	.num_ch = 0,							\
};									\
DEVICE_DT_INST_DEFINE(n,						\
		      &mbox_mhuv3_tx_init,				\
		      NULL,						\
		      &mbox_mhuv3_data_tx_##n, &mbox_mhuv3_cfg_tx_##n,	\
		      PRE_KERNEL_1,					\
		      CONFIG_KERNEL_INIT_PRIORITY_DEVICE,		\
		      &mbox_mhuv3_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MBOX_MHUV3_TX_INIT);
