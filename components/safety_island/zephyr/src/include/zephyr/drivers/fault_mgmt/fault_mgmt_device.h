/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_FAULT_MGMT_H_
#define ZEPHYR_INCLUDE_DRIVERS_FAULT_MGMT_H_

#include <zephyr/device.h>

/**
 * @defgroup fault_mgmt_device Fault Device Driver
 * @ingroup fault_mgmt
 *
 * @brief Fault Device Driver
 *
 * @{
 */

/**
 * @brief Metadata of a single fault
 */
struct fault_mgmt_fault {
	/** @brief The device from which the fault originates */
	device_handle_t handle;
	/** @brief The device-specific protection mechanism ID that generated the fault */
	uint32_t prot_id;
};

/** @cond INTERNAL_HIDDEN */
#define FAULT_MGMT_FAULT_CRITICAL_MASK      BIT(31)
#define FAULT_MGMT_FAULT_PROTECTION_ID_MASK GENMASK(30, 0)
/** @endcond */

/**
 * @brief Determine the criticality of a fault struct
 */
#define FAULT_MGMT_FAULT_IS_CRITICAL(fault)                                                        \
	(((fault)->prot_id & FAULT_MGMT_FAULT_CRITICAL_MASK) > 0)

/**
 * @brief Read the protection mechanism ID from the prot_id field of a fault struct
 */
#define FAULT_MGMT_FAULT_PROTECTION_ID(fault)                                                      \
	((fault)->prot_id & FAULT_MGMT_FAULT_PROTECTION_ID_MASK)

/**
 * @brief An invalid protection mechanism ID
 */
#define FAULT_MGMT_FAULT_PROTECTION_ID_INVALID 0

/**
 * @brief The fault device callback type
 *
 * @param dev The root device from which the fault originates
 * @param fault A description of the fault
 * @param user_data User-specific context
 */
typedef void (*fault_mgmt_fault_callback_t)(const struct device *dev,
					    const struct fault_mgmt_fault *fault, void *user_data);

/**
 * @brief The API that a fault device must implement
 */
__subsystem struct fault_mgmt_device_api {
	/**
	 * @brief Inject a fault into the device
	 *
	 * @param dev     A pointer to the fault device
	 * @param prot_id The protection mechanism ID into which to inject the fault
	 * @retval 0 On success.
	 * @retval -errno Error code on failure
	 */
	int (*inject)(const struct device *dev, uint32_t prot_id);

	/** @brief Enable or disable a protection mechanism
	 *
	 * @param dev     A pointer to the fault device
	 * @param prot_id The protection mechanism ID to enable or disable
	 * @param enabled @c true to enable or @c false to disable
	 * @retval 0 On success.
	 * @retval -errno Error code on failure
	 */
	int (*set_enabled)(const struct device *dev, uint32_t prot_id, bool enabled);

	/** @brief Set the criticality of fault
	 *
	 * @param dev     A pointer to the fault device
	 * @param prot_id The protection mechanism ID to enable or disable
	 * @param critical @c true to set as critical or @c false to set as non-critical
	 * @retval 0 On success.
	 * @retval -errno Error code on failure
	 */
	int (*set_critical)(const struct device *dev, uint32_t prot_id, bool critical);

	/** @brief Set the device's fault callback
	 *
	 * @param dev A pointer to the fault device
	 * @param callback A function pointer to be called when a fault is reported
	 * @param user_data User-specific context to be passed to the @p callback
	 */
	int (*fault_callback_set)(const struct device *dev, fault_mgmt_fault_callback_t callback,
				  void *user_data);

	/** @brief Obtain a list of upstream fault devices
	 *
	 * @param dev A pointer to the fault device
	 * @param count An out pointer to store the number of upstream devices
	 * @return The head of an array of @p count upstream fault devices
	 */
	const struct device **(*upstream_devices)(const struct device *dev, size_t *count);

	/** @brief Obtain the attached safety state device
	 *
	 * @param dev A pointer to the fault device
	 * @retval dev A pointer to the safety state device
	 * @retval NULL if no safety state device is attached
	 */
	const struct device *(*safety_device)(const struct device *dev);
};

/**
 * @brief A convenience macro to access a fault device's API
 */
#define FAULT_MGMT_DEV_API(dev) ((const struct fault_mgmt_device_api *const)(dev)->api)

/** @} */

#endif /* ZEPHYR_INCLUDE_DRIVERS_FAULT_MGMT_H_ */
