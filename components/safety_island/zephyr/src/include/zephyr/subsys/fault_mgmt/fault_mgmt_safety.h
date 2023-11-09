/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAULT_MGMT_SAFETY_H_
#define FAULT_MGMT_SAFETY_H_

#include <zephyr/device.h>

#include "zephyr/drivers/fault_mgmt/fault_mgmt_safety_device.h"

/**
 * @defgroup fault_mgmt_safety Fault management safety component
 * @ingroup fault_mgmt
 *
 * @brief Fault management subsystem safety component
 *
 * @{
 */

/**
 * @brief Read the current safety device state
 *
 * @param dev A pointer to the safety state device
 * @return A @ref fault_mgmt_safety_state value
 */
enum fault_mgmt_safety_state fault_mgmt_safety_status(const struct device *dev);

/**
 * @brief Send a safety device control signal
 *
 * @param dev A pointer to the safety state device
 * @param val A @ref fault_mgmt_safety_signal signal
 */
void fault_mgmt_safety_control(const struct device *dev, enum fault_mgmt_safety_signal val);

/** @} */

#endif /* FAULT_MGMT_SAFETY_H_ */
