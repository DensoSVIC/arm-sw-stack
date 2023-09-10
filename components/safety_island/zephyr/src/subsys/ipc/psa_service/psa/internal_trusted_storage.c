/*
 * Based on: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/interface/src/tfm_its_api.c?h=TF-Mv1.8.0
 * In open-source project: TF-M/trusted-firmware-m
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2019-2021 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes:
 * 1) Cast some size_t variables whose address is used in the message to uint32_t
 *    due to difference in the architecture between RSS (32 bits) and Safety Island (64 bits).
 */

#include "zephyr/subsys/ipc/psa_service/psa/client.h"
#include "zephyr/subsys/ipc/psa_service/psa/internal_trusted_storage.h"
#include "zephyr/subsys/ipc/psa_service/psa_backend.h"

psa_status_t psa_its_set(psa_storage_uid_t uid, size_t data_length, const void *p_data,
			 psa_storage_create_flags_t create_flags)
{
	psa_status_t status;

	struct psa_invec in_vec[] = {{.base = &uid, .len = sizeof(uid)},
				     {.base = p_data, .len = data_length},
				     {.base = &create_flags, .len = sizeof(create_flags)}};

	status = psa_call(PSA_INTERNAL_TRUSTED_STORAGE_SERVICE_HANDLE, PSA_ITS_SET, in_vec,
			  IOVEC_LEN(in_vec), NULL, 0);

	return status;
}

psa_status_t psa_its_get(psa_storage_uid_t uid, size_t data_offset, size_t data_size, void *p_data,
			 size_t *p_data_length)
{
	psa_status_t status;

	uint32_t offset = (uint32_t)data_offset;

	struct psa_invec in_vec[] = {{.base = &uid, .len = sizeof(uid)},
				     {.base = &offset, .len = sizeof(offset)}};

	struct psa_outvec out_vec[] = {{.base = p_data, .len = data_size}};

	if (p_data_length == NULL) {
		return PSA_ERROR_INVALID_ARGUMENT;
	}

	status = psa_call(PSA_INTERNAL_TRUSTED_STORAGE_SERVICE_HANDLE, PSA_ITS_GET, in_vec,
			  IOVEC_LEN(in_vec), out_vec, IOVEC_LEN(out_vec));

	if (status == PSA_SUCCESS) {
		*p_data_length = out_vec[0].len;
	}

	return status;
}

psa_status_t psa_its_get_info(psa_storage_uid_t uid, struct psa_storage_info_t *p_info)
{
	psa_status_t status;

	struct psa_invec in_vec[] = {{.base = &uid, .len = sizeof(uid)}};

	struct psa_outvec out_vec[] = {{.base = p_info, .len = sizeof(*p_info)}};

	status = psa_call(PSA_INTERNAL_TRUSTED_STORAGE_SERVICE_HANDLE, PSA_ITS_GET_INFO, in_vec,
			  IOVEC_LEN(in_vec), out_vec, IOVEC_LEN(out_vec));

	return status;
}

psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
	psa_status_t status;

	struct psa_invec in_vec[] = {{.base = &uid, .len = sizeof(uid)}};

	status = psa_call(PSA_INTERNAL_TRUSTED_STORAGE_SERVICE_HANDLE, PSA_ITS_REMOVE, in_vec,
			  IOVEC_LEN(in_vec), NULL, 0);

	return status;
}
