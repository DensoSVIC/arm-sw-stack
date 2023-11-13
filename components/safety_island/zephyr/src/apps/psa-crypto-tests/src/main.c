/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

/* Run the PSA test suite */
void psa_test(void);

int main(void)
{
#ifdef CONFIG_PSA_ARCH_TEST_NONE
	#error "No PSA test suite set. Use Kconfig to enable a test suite.\n"
#else
	psa_test();
#endif

	return 0;
}
