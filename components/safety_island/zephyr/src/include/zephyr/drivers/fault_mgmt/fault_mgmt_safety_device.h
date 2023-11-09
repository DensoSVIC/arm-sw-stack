/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_FAULT_MGMT_SAFETY_DEVICE_H_
#define ZEPHYR_INCLUDE_FAULT_MGMT_SAFETY_DEVICE_H_

#include <zephyr/device.h>

/**
 * @defgroup fault_mgmt_safety_device Safety state device driver
 * @ingroup fault_mgmt
 *
 * @brief Safety state device driver
 *
 * @{
 */

/**
 * @brief Safety device state enumeration
 */
enum fault_mgmt_safety_state {
	/** @brief TEST state: self-test on boot */
	FAULT_MGMT_SAFETY_STATUS_TEST = 0x0,
	/** @brief SAFE state: safe operation */
	FAULT_MGMT_SAFETY_STATUS_SAFE = 0x3,
	/** @brief ERRN state: Non-critical fault detected */
	FAULT_MGMT_SAFETY_STATUS_ERRN = 0x5,
	/** @brief ERRC state: Critical fault detected */
	FAULT_MGMT_SAFETY_STATUS_ERRC = 0x6,
};

/**
 * @brief Safety device control value enumeration
 *
 * Control signals trigger a safety device state transition
 */
enum fault_mgmt_safety_signal {
	/** @brief compl_ok: Diagnostics complete or non-critical fault cleared */
	FAULT_MGMT_SAFETY_CNTRL_COMPL_OK = 0x0,
	/** @brief nce_ok: Non-critical fault diagnosed */
	FAULT_MGMT_SAFETY_CNTRL_NCE_OK = 0x1,
	/** @brief ce_not_ok: Critical fault diagnosed */
	FAULT_MGMT_SAFETY_CNTRL_CE_NOT_OK = 0x2,
	/** @brief nce_not_ok: Non-correctable non-critical fault */
	FAULT_MGMT_SAFETY_CNTRL_NCE_NOT_OK = 0x3,
};

/**
 * @brief Convert a safety state value into a string
 *
 * @param val A safety state value
 * @return A pointer to a static const string corresponding to the input
 */
const char *fault_mgmt_safety_state_to_str(enum fault_mgmt_safety_state val);

/**
 * @brief Convert a string into a safety control signal value
 *
 * @param str A safety control signal string
 * @param val An out pointer to receive
 * @retval 0 If @p str represents a valid control signal
 * @retval -EINVAL If @p str is invalid
 */
int fault_mgmt_safety_signal_from_str(const char *str, enum fault_mgmt_safety_signal *val);

/**
 * @brief The API that a safety state device must implement
 */
__subsystem struct fault_mgmt_safety_device_api {
	/**
	 * @brief Read the current safety device state
	 *
	 * @param dev A pointer to the safety state device
	 * @return A @ref fault_mgmt_safety_state value
	 */
	enum fault_mgmt_safety_state (*status)(const struct device *dev);

	/**
	 * @brief Send a safety device control signal
	 *
	 * @param dev A pointer to the safety state device
	 * @param val A @ref fault_mgmt_safety_signal value
	 */
	void (*control)(const struct device *dev, enum fault_mgmt_safety_signal val);
};

/**
 * @brief A convenience macro to access a safety state device's API
 */
#define FAULT_MGMT_SAFETY_DEV_API(dev)                                                             \
	((const struct fault_mgmt_safety_device_api *const)(dev)->api)

/** @} */

#endif /* ZEPHYR_INCLUDE_FAULT_MGMT_SAFETY_DEVICE_H_ */
