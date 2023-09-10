/*
 * Based on: https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/include/lib/psa/psa/error.h?h=v2.9.0
 * In open-source project: TF-A/trusted-firmware-a
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2019-2021 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes: None
 */

#ifndef PSA_ERROR_H
#define PSA_ERROR_H

#include <stdint.h>

typedef int32_t psa_status_t;

/* General success status code. */
#define PSA_SUCCESS                     ((psa_status_t)0)
/* The action was completed successfully and requires a system reboot to complete installation. */
#define PSA_SUCCESS_REBOOT              ((psa_status_t)1)
/* The action was completed successfully and requires
 * a restart of the component to complete installation.
 */
#define PSA_SUCCESS_RESTART             ((psa_status_t)2)
/* Connection dropped due to PROGRAMMER ERROR. */
#define PSA_ERROR_PROGRAMMER_ERROR      ((psa_status_t)-129)
/* Connection to the service is not permitted. */
#define PSA_ERROR_CONNECTION_REFUSED    ((psa_status_t)-130)
/* Connection to the service is not possible. */
#define PSA_ERROR_CONNECTION_BUSY       ((psa_status_t)-131)
/* An error not related to a specific failure cause. */
#define PSA_ERROR_GENERIC_ERROR         ((psa_status_t)-132)
/* The operation is denied by a policy. */
#define PSA_ERROR_NOT_PERMITTED         ((psa_status_t)-133)
/* The operation or a parameter is not supported. */
#define PSA_ERROR_NOT_SUPPORTED         ((psa_status_t)-134)
/* One or more parameters are invalid. */
#define PSA_ERROR_INVALID_ARGUMENT      ((psa_status_t)-135)
/* A handle parameter is not valid. */
#define PSA_ERROR_INVALID_HANDLE        ((psa_status_t)-136)
/* The operation is not valid in the current state. */
#define PSA_ERROR_BAD_STATE             ((psa_status_t)-137)
/* An output buffer parameter is too small. */
#define PSA_ERROR_BUFFER_TOO_SMALL      ((psa_status_t)-138)
/* An identifier or index is already in use. */
#define PSA_ERROR_ALREADY_EXISTS        ((psa_status_t)-139)
/* An identified resource does not exist. */
#define PSA_ERROR_DOES_NOT_EXIST        ((psa_status_t)-140)
/* There is not enough runtime memory. */
#define PSA_ERROR_INSUFFICIENT_MEMORY   ((psa_status_t)-141)
/* There is not enough persistent storage. */
#define PSA_ERROR_INSUFFICIENT_STORAGE  ((psa_status_t)-142)
/* A data source has insufficient capacity left. */
#define PSA_ERROR_INSUFFICIENT_DATA     ((psa_status_t)-143)
/* Failure within the service. */
#define PSA_ERROR_SERVICE_FAILURE       ((psa_status_t)-144)
/* Communication failure with another component. */
#define PSA_ERROR_COMMUNICATION_FAILURE ((psa_status_t)-145)
/* Storage failure that may have led to data loss. */
#define PSA_ERROR_STORAGE_FAILURE       ((psa_status_t)-146)
/* General hardware failure. */
#define PSA_ERROR_HARDWARE_FAILURE      ((psa_status_t)-147)
/* A signature, MAC or hash is incorrect. */
#define PSA_ERROR_INVALID_SIGNATURE     ((psa_status_t)-149)
/* A status code that indicates that the firmware of another component requires updating. */
#define PSA_ERROR_DEPENDENCY_NEEDED     ((psa_status_t)-156)

#endif /* PSA_ERROR_H */
