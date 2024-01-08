/*
 * Based on: https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/drivers/arm/rss/rss_comms_protocol_embed.h?h=v2.9.0
 * In open-source project: TF-A/trusted-firmware-a
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2022 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes: None
 */

#ifndef __RSS_COMMS_PROTOCOL_EMBED_H__
#define __RSS_COMMS_PROTOCOL_EMBED_H__

#include <zephyr/toolchain.h>

#include "zephyr/subsys/ipc/psa_service/psa/client.h"

#define RSS_COMMS_PAYLOAD_MAX_SIZE (0x40 + 0x1000)

struct __packed rss_embed_msg_t {
	psa_handle_t handle;
	uint32_t ctrl_param; /* type, in_len, out_len */
	uint16_t io_size[PSA_MAX_IOVEC];
	uint8_t trailer[RSS_COMMS_PAYLOAD_MAX_SIZE];
};

struct __packed rss_embed_reply_t {
	int32_t return_val;
	uint16_t out_size[PSA_MAX_IOVEC];
	uint8_t trailer[RSS_COMMS_PAYLOAD_MAX_SIZE];
};

psa_status_t rss_protocol_embed_serialize_msg(psa_handle_t handle, int16_t type,
					      const struct psa_invec *in_vec, uint8_t in_len,
					      const struct psa_outvec *out_vec, uint8_t out_len,
					      struct rss_embed_msg_t *msg, size_t *msg_len);

psa_status_t rss_protocol_embed_deserialize_reply(struct psa_outvec *out_vec, uint8_t out_len,
						  psa_status_t *return_val,
						  const struct rss_embed_reply_t *reply,
						  size_t reply_size);

#endif /* __RSS_COMMS_PROTOCOL_EMBED_H__ */
