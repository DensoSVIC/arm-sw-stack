/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PSA_BACKEND_H_
#define PSA_BACKEND_H_

#include <zephyr/device.h>

#define MHU3_OUTBAND_BUF_HEADER_SIZE UINT64_C(0x4)
#define MHU3_OUTBAND_BUF_SIZE        UINT64_C(0x2000)
#define MHU3_MAX_MSG_SIZE            (MHU3_OUTBAND_BUF_SIZE - MHU3_OUTBAND_BUF_HEADER_SIZE)

__subsystem struct psa_service_backend {
	int (*send)(const struct device *dev, const uint8_t *send_buffer, size_t size);
	int (*receive)(const struct device *dev, uint8_t *receive_buffer, size_t *size);
};

#endif /* PSA_BACKEND_H_ */
