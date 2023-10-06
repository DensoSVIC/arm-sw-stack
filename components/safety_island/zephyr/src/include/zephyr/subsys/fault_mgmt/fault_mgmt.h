/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_H_
#define FAULT_MGMT_H_

#include <zephyr/device.h>
#include <zephyr/sys/iterable_sections.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_device.h"

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

/**
 * @brief Obtain the safety state device attached to a fault device
 *
 * @param dev A pointer to the fault device
 * @retval dev A pointer to the safety state device
 * @retval NULL if no safety state device is attached
 */
const struct device *fault_mgmt_safety_device(const struct device *dev);

/**
 * @brief Fault handler initialization function
 *
 * @param root_dev A pointer to the root device to be initialized
 * @retval 0 On success
 * @retval -errno  Error code on failure, terminates subsystem initialization
 */
typedef int (*fault_mgmt_handler_init)(const struct device *root_dev);

/**
 * @brief Fault handler function called for each fault received
 *
 * @param root_dev A pointer to the root device to which the fault was delivered
 * @param fault A struct with metadata about the fault
 */
typedef void (*fault_mgmt_handler_handle)(const struct device *root_dev,
					  const struct fault_mgmt_fault *fault);

/** @cond INTERNAL_HIDDEN */
struct fault_mgmt_handler {
	fault_mgmt_handler_init init;
	fault_mgmt_handler_handle handle;
};
#define FAULT_MGMT_HANDLER_NAME(priority) handler_##priority
/** @endcond */

/**
 * @brief Macro to statically register a fault handler
 *
 * Each enabled fault handler must have a unique priority.
 *
 * @param priority Determines the placement in the fault handler list
 * @param _init The handler's initialization function of type @ref fault_mgmt_handler_init, or @c
 * NULL
 * @param _handle The handler's fault handler of type @ref fault_mgmt_handler_handle, or @c NULL
 */
#define FAULT_MGMT_HANDLER_DEFINE(priority, _init, _handle)                                        \
	STRUCT_SECTION_ITERABLE(fault_mgmt_handler, FAULT_MGMT_HANDLER_NAME(priority)) = {         \
		.init = _init,                                                                     \
		.handle = _handle,                                                                 \
	}

/** @} */

#endif /* FAULT_MGMT_H_ */
