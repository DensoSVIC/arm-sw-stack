/*
 * Based on: https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/tree/include/lib/psa/psa/client.h?h=v2.9.0
 * In open-source project: TF-A/trusted-firmware-a
 *
 * Original file: SPDX-FileCopyrightText: <text>Copyright 2018-2021 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 * Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
 * and/or its affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Changes: None
 */

#ifndef PSA_CLIENT_H
#define PSA_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#include "error.h"

#ifndef IOVEC_LEN
#define IOVEC_LEN(arr) ((uint32_t)ARRAY_SIZE(arr))
#endif
/*********************** PSA Client Macros and Types *************************/
/**
 * The version of the PSA Framework API that is being used to build the calling
 * firmware. Only part of features of FF-M v1.1 have been implemented. FF-M v1.1
 * is compatible with v1.0.
 */
#define PSA_FRAMEWORK_VERSION       (0x0101u)
/**
 * Return value from psa_version() if the requested RoT Service is not present
 * in the system.
 */
#define PSA_VERSION_NONE            (0u)
/**
 * The zero-value null handle can be assigned to variables used in clients and
 * RoT Services, indicating that there is no current connection or message.
 */
#define PSA_NULL_HANDLE             ((psa_handle_t)0)
/**
 * Tests whether a handle value returned by psa_connect() is valid.
 */
#define PSA_HANDLE_IS_VALID(handle) ((psa_handle_t)(handle) > 0)
/**
 * Converts the handle value returned from a failed call psa_connect() into
 * an error code.
 */
#define PSA_HANDLE_TO_ERROR(handle) ((psa_status_t)(handle))
/**
 * Maximum number of input and output vectors for a request to psa_call().
 */
#define PSA_MAX_IOVEC               (4u)
/**
 * An IPC message type that indicates a generic client request.
 */
#define PSA_IPC_CALL                (0)
typedef int32_t psa_handle_t;
/**
 * A read-only input memory region provided to an RoT Service.
 */
struct psa_invec {
	const void *base; /*!< the start address of the memory buffer */
	size_t len;       /*!< the size in bytes                      */
};
/**
 * A writable output memory region provided to an RoT Service.
 */
struct psa_outvec {
	void *base; /*!< the start address of the memory buffer */
	size_t len; /*!< the size in bytes                      */
};

/*************************** PSA Client API **********************************/
/**
 * \brief Call an RoT Service on an established connection.
 *
 * \note  FF-M 1.0 proposes 6 parameters for psa_call but the secure gateway ABI
 *        support at most 4 parameters. TF-M chooses to encode 'in_len',
 *        'out_len', and 'type' into a 32-bit integer to improve efficiency.
 *        Compared with struct-based encoding, this method saves extra memory
 *        check and memory copy operation. The disadvantage is that the 'type'
 *        range has to be reduced into a 16-bit integer. So with this encoding,
 *        the valid range for 'type' is 0-32767.
 *
 * \param[in] handle            A handle to an established connection.
 * \param[in] type              The request type.
 *                              Must be zero( \ref PSA_IPC_CALL) or positive.
 * \param[in] in_vec            Array of input \ref psa_invec structures.
 * \param[in] in_len            Number of input \ref psa_invec structures.
 * \param[in,out] out_vec       Array of output \ref psa_outvec structures.
 * \param[in] out_len           Number of output \ref psa_outvec structures.
 *
 * \retval >=0                  RoT Service-specific status value.
 * \retval <0                   RoT Service-specific error code.
 * \retval PSA_ERROR_PROGRAMMER_ERROR The connection has been terminated by the
 *                              RoT Service. The call is a PROGRAMMER ERROR if
 *                              one or more of the following are true:
 * \arg                           An invalid handle was passed.
 * \arg                           The connection is already handling a request.
 * \arg                           type < 0.
 * \arg                           An invalid memory reference was provided.
 * \arg                           in_len + out_len > PSA_MAX_IOVEC.
 * \arg                           The message is unrecognized by the RoT
 *                                Service or incorrectly formatted.
 */
psa_status_t psa_call(psa_handle_t handle, int32_t type, const struct psa_invec *in_vec,
		      size_t in_len, struct psa_outvec *out_vec, size_t out_len);

#endif /* PSA_CLIENT_H */
