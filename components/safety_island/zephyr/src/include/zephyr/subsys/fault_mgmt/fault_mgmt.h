/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_H_
#define FAULT_MGMT_H_

#include <zephyr/device.h>

/**
 * @defgroup fault_mgmt Fault management
 *
 * @brief Fault management subsystem
 *
 * @{
 */

/**
 * @brief Callback definition for @ref fault_mgmt_device_foreach
 *
 * A callback of this type is used for fault device tree iteration.
 *
 * @param dev A fault device
 * @param depth The depth of @p dev in the fault device tree
 * @param index The index of @p dev, relative to its parent device
 * @param cookie User-specific context
 */
typedef int (*fault_mgmt_device_callback)(const struct device *dev, size_t depth, size_t index,
					  void *cookie);

/**
 * @brief Iterate over the fault device tree
 *
 * Perform a depth-first iteration over the tree of fault devices, calling @p callback for each
 * device.
 *
 * @param callback The fault device into which to inject the fault
 * @param cookie   User-specific context for @p callback
 * @retval 0 On success.
 * @retval -errno Error code from @p callback on failure
 */
int fault_mgmt_device_foreach(fault_mgmt_device_callback callback, void *cookie);

/**
 * @brief Inject a fault into a a fault device
 *
 * In order to validate fault devices, protection mechanisms can simulate the occurrence of a fault.
 *
 * @param dev     The fault device into which to inject the fault
 * @param prot_id The protection ID of the fault to inject
 * @retval 0 On success.
 * @retval -errno Error code on failure
 */
int fault_mgmt_inject(const struct device *dev, uint32_t prot_id);

/**
 * @brief Enable or disable a fault device's protection mechanism
 *
 * A device's protection mechanisms may be enabled or disabled at runtime.
 *
 * @param dev     The fault device of the protection mechanism
 * @param prot_id The protection ID of the fault to enable or disable
 * @param enabled @c true to enable or @c false to disable
 * @retval 0 On success.
 * @retval -errno Error code on failure
 */
int fault_mgmt_set_enabled(const struct device *dev, uint32_t prot_id, bool enabled);

/**
 * @brief Configure the criticality of a fault device's protection mechanism
 *
 * @param dev     The fault device of the protection mechanism
 * @param prot_id An opaque device-specific protection ID
 * @param critical @c true to set as critical or @c false to set as non-critical
 * @retval 0 On success.
 * @retval -errno Error code on failure
 */
int fault_mgmt_set_critical(const struct device *dev, uint32_t prot_id, bool critical);

/** @} */

#endif /* FAULT_MGMT_H_ */
