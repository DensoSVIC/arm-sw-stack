/*
 * Based on: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/interface/include/psa/crypto_client_struct.h?h=TF-Mv1.8.0
 * In open-source project: TF-M/trusted-firmware-m
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2020-2021 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes: None
 *
 */

/**
 * \file psa/crypto_client_struct.h
 *
 * \brief PSA cryptography client key attribute definitions
 *
 * \note This file may not be included directly. Applications must
 * include psa/crypto.h.
 *
 * This file contains the definitions of some data structures with
 * PSA crypto client specific definitions. This is for implementations
 * with isolation between the Client applications and the Crypto
 * Server module, it is expected that the front-end and the back-end
 * would have different versions of the data structure.
 */
#ifndef PSA_CRYPTO_CLIENT_STRUCT_H
#define PSA_CRYPTO_CLIENT_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/* This is the client view of the `key_attributes` structure. Only
 * fields which need to be set by the PSA crypto client are present.
 * The PSA crypto service will maintain a different version of the
 * data structure internally. */
struct psa_client_key_attributes_s
{
	uint16_t type;
	uint16_t bits;
	uint32_t lifetime;
	psa_key_id_t id;
	uint32_t usage;
	uint32_t alg;
};

#define PSA_CLIENT_KEY_ATTRIBUTES_INIT {0, 0, 0, 0, 0, 0}

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_CLIENT_STRUCT_H */
