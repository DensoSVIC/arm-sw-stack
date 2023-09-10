/*
 * Based on: https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/drivers/arm/rss/rss_comms_protocol.c?h=v2.9.0
 * In open-source project: TF-A/trusted-firmware-a
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2022 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes: None
 */

#include <assert.h>

#include "rss_comms_protocol.h"

psa_status_t rss_protocol_serialize_msg(psa_handle_t handle, int16_t type,
					const struct psa_invec *in_vec, uint8_t in_len,
					const struct psa_outvec *out_vec, uint8_t out_len,
					struct serialized_rss_comms_msg_t *msg, size_t *msg_len)
{
	psa_status_t status;

	assert(msg != NULL);
	assert(msg_len != NULL);
	assert(in_vec != NULL);

	switch (msg->header.protocol_ver) {
	case RSS_COMMS_PROTOCOL_EMBED:
		status = rss_protocol_embed_serialize_msg(handle, type, in_vec, in_len, out_vec,
							  out_len, &msg->msg.embed, msg_len);
		if (status != PSA_SUCCESS) {
			return status;
		}
		break;
	case RSS_COMMS_PROTOCOL_POINTER_ACCESS:
		status = rss_protocol_pointer_access_serialize_msg(
			handle, type, in_vec, in_len, out_vec, out_len, &msg->msg.pointer_access,
			msg_len);
		if (status != PSA_SUCCESS) {
			return status;
		}
		break;
	default:
		return PSA_ERROR_NOT_SUPPORTED;
	}

	*msg_len += sizeof(struct serialized_rss_comms_header_t);

	return PSA_SUCCESS;
}

psa_status_t rss_protocol_deserialize_reply(struct psa_outvec *out_vec, uint8_t out_len,
					    psa_status_t *return_val,
					    const struct serialized_rss_comms_reply_t *reply,
					    size_t reply_size)
{
	assert(reply != NULL);
	assert(return_val != NULL);

	switch (reply->header.protocol_ver) {
	case RSS_COMMS_PROTOCOL_EMBED:
		return rss_protocol_embed_deserialize_reply(out_vec, out_len, return_val,
							    &reply->reply.embed, reply_size);
	case RSS_COMMS_PROTOCOL_POINTER_ACCESS:
		return rss_protocol_pointer_access_deserialize_reply(
			out_vec, out_len, return_val, &reply->reply.pointer_access, reply_size);
	default:
		return PSA_ERROR_NOT_SUPPORTED;
	}

	return PSA_SUCCESS;
}
