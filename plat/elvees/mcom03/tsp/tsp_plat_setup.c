/*
 * Copyright 2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/ti/uart/uart_16550.h>
#include <plat/arm/common/plat_arm.h>

#include <plat_private.h>
#include <platform_tsp.h>

/*
 * Initialize the UART
 */
void tsp_early_platform_setup(void)
{
	static console_t console;

	console_16550_register(PLAT_TSP_UART_BASE,
			       PLAT_TSP_UART_CLK_IN_HZ,
			       PLAT_TSP_CONSOLE_BAUDRATE, &console);

	NOTICE("TSP: Boot CPU: [0x%lx] Processor [0x%lx]\n",
		read_midr(), read_mpidr());
}

/*
 * Perform platform specific setup placeholder
 */
void tsp_platform_setup(void)
{
}

/*
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the MMU
 */
void tsp_plat_arch_setup(void)
{
	mcom03_mmap_setup(BL32_BASE, BL32_SIZE, NULL);
	enable_mmu_el1(0);
}
