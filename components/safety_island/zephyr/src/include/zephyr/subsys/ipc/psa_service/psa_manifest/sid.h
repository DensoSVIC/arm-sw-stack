/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PSA_SID_H__
#define __PSA_SID_H__

#ifdef __cplusplus
extern "C" {
#endif

/******** TFM_SP_PS ********/
#define TFM_PROTECTED_STORAGE_SERVICE_SID                          (0x00000060U)
#define TFM_PROTECTED_STORAGE_SERVICE_VERSION                      (1U)
#define TFM_PROTECTED_STORAGE_SERVICE_HANDLE                       (0x40000101U)

/* Invalid UID */
#define TFM_PS_INVALID_UID 0

/* PS / ITS message types that distinguish PS services. */
#define TFM_PS_ITS_SET                1001
#define TFM_PS_ITS_GET                1002
#define TFM_PS_ITS_GET_INFO           1003
#define TFM_PS_ITS_REMOVE             1004
#define TFM_PS_ITS_GET_SUPPORT        1005

/******** TFM_SP_ITS ********/
#define TFM_INTERNAL_TRUSTED_STORAGE_SERVICE_SID                   (0x00000070U)
#define TFM_INTERNAL_TRUSTED_STORAGE_SERVICE_VERSION               (1U)
#define TFM_INTERNAL_TRUSTED_STORAGE_SERVICE_HANDLE                (0x40000102U)

#ifdef __cplusplus
}
#endif

#endif /* __PSA_SID_H__ */
