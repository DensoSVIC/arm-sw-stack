/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBOX_MHUV3_H
#define MBOX_MHUV3_H

#include <stdint.h>
#include <zephyr/drivers/mbox.h>

#define INVALID_CH_INDEX	(0xFFFFFFFF)

/* Device structure operation */
#define MHUV3_DEV_CFG(dev) \
	((const struct mbox_mhuv3_config * const)(dev)->config)

#define MHUV3_DEV_DATA(dev) \
	((struct mbox_mhuv3_data *)(dev)->data)

/* Rx reg operation */
#define MHUV3_MBX_CTRL(dev) \
	((struct mhuv3_mbx_ctrl_reg *)((MHUV3_DEV_CFG(dev))->base))

#define MHUV3_MDBCW(dev) \
	((struct mhuv3_mdbcw_reg *)((MHUV3_DEV_CFG(dev))->base + 0x1000))

/* Tx reg operation */
#define MHUV3_PBX_CTRL(dev) \
	((struct mhuv3_pbx_ctrl_reg *)((MHUV3_DEV_CFG(dev))->base))

#define MHUV3_PDBCW(dev) \
	((struct mhuv3_pdbcw_reg *)((MHUV3_DEV_CFG(dev))->base + 0x1000))

/* MHUv3 Mailbox regs pages, Mailbox is the receiver */
struct mhuv3_mbx_ctrl_reg {
	/* Mailbox control page */
	volatile uint32_t mhu_blk_id;
	volatile uint8_t reserved1[0x10 - 0x04];
	volatile uint32_t mbx_feat_spt0;
	volatile uint32_t mbx_feat_spt1;
	volatile uint8_t reserved2[0x20 - 0x18];
	volatile uint32_t mbx_dbch_cfg0;
	volatile uint8_t reserved3[0x30 - 0x24];
	volatile uint32_t mbx_ffch_cfg0;
	volatile uint8_t reserved4[0x40 - 0x34];
	volatile uint32_t mbx_fch_cfg0;
	volatile uint8_t reserved5[0x50 - 0x44];
	volatile uint32_t mbx_dch_cfg0;
	volatile uint8_t reserved6[0x100 - 0x54];
	volatile uint32_t mbx_ctrl;
	volatile uint8_t reserved7[0x140 - 0x104];
	volatile uint32_t mbx_fch_ctrl;
	volatile uint32_t mbx_fcg_int_en;
	volatile uint8_t reserved8[0x150 - 0x148];
	volatile uint32_t mbx_dma_ctrl;
	volatile uint32_t mbx_dma_st;
	volatile uint64_t mbx_dma_cdl_base;
	volatile uint32_t mbx_dma_cdl_prop;
	volatile uint8_t reserved9[0x400 - 0x164];
	volatile uint32_t mbx_dbch_int_st[(0x410 - 0x400) >> 2];
	volatile uint32_t mbx_ffch_int_st[(0x420 - 0x410) >> 2];
	volatile uint32_t mbx_fcg_int_st;
	volatile uint8_t reserved10[0x430 - 0x424];
	volatile uint32_t mbx_fch_grp_int_st[(0x4b0 - 0x430) >> 2];
	volatile uint32_t mbx_dch_int_st;
	volatile uint8_t reserved11[0xfc8 - 0x4b4];
	volatile uint32_t iidr;
	volatile uint32_t aidr;
	volatile uint32_t impl_def_id[4 * 11];
} __packed;

struct mhuv3_mdbcw_reg {
	/* Mailbox Doorbell Channel Window regs */
	volatile uint32_t mdbcw_st;
	volatile uint32_t mdbcw_st_msk;
	volatile uint32_t mdbcw_clr;
	volatile uint8_t reserved1[0x10 - 0x0c];
	volatile uint32_t mdbcw_msk_st;
	volatile uint32_t mdbcw_msk_set;
	volatile uint32_t mdbcw_msk_clr;
	volatile uint32_t mdbcw_ctrl;
} __packed;

/* MHUv3 Postbox regs pages, postbox is the sender */
struct mhuv3_pbx_ctrl_reg {
	/* Postbox control page */
	volatile uint32_t mhu_blk_id;
	volatile uint8_t reserved1[0x10 - 0x04];
	volatile uint32_t pbx_feat_spt0;
	volatile uint32_t pbx_feat_spt1;
	volatile uint8_t reserved2[0x20 - 0x18];
	volatile uint32_t pbx_dbch_cfg0;
	volatile uint8_t reserved3[0x30 - 0x24];
	volatile uint32_t pbx_ffch_cfg0;
	volatile uint8_t reserved4[0x40 - 0x34];
	volatile uint32_t pbx_fch_cfg0;
	volatile uint8_t reserved5[0x50 - 0x44];
	volatile uint32_t pbx_dch_cfg0;
	volatile uint8_t reserved6[0x100 - 0x54];
	volatile uint32_t pbx_ctrl;
	volatile uint8_t reserved7[0x150 - 0x104];
	volatile uint32_t pbx_dma_ctrl;
	volatile uint32_t pbx_dma_st;
	volatile uint64_t pbx_dma_cdl_base;
	volatile uint32_t pbx_dma_cdl_prop;
	volatile uint8_t reserved8[0x400 - 0x164];
	volatile uint32_t pbx_dbch_int_st[(0x410 - 0x400) >> 2];
	volatile uint32_t pbx_ffch_int_st[(0x430 - 0x410) >> 2];
	volatile uint32_t pbx_dch_int_st;
	volatile uint8_t reserved9[0xfc8 - 0x434];
	volatile uint32_t iidr;
	volatile uint32_t aidr;
	volatile uint32_t impl_def_id[4 * 11];
} __packed;

struct mhuv3_pdbcw_reg {
	/* Postbox Doorbell Channel Window regs */
	volatile uint32_t pdbcw_st;
	volatile uint8_t reserved1[0xc - 0x4];
	volatile uint32_t pdbcw_set;
	volatile uint32_t pdbcw_int_st;
	volatile uint32_t pdbcw_int_clr;
	volatile uint32_t pdbcw_int_en;
	volatile uint32_t pdbcw_ctrl;
} __packed;

struct mbox_mhuv3_data {
	mbox_callback_t cb[CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM];
	void *user_data[CONFIG_MBOX_MHUV3_MAX_LOGICAL_CH_NUM];
	uint32_t num_ch;
};

struct mbox_mhuv3_config {
	volatile uint8_t *base;
	void (*rx_irq_config)(const struct device *d);
};

#endif /* MBOX_MHUV3_H */
