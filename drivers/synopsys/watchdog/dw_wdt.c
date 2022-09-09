/*
 * Copyright 2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <drivers/synopsys/dw_wdt.h>
#include <lib/mmio.h>
#include <platform_def.h>

bool dw_wdt_is_enabled(uintptr_t base)
{
	return mmio_read_32(base + DW_WDT_CONTROL_REG) & DW_WDT_EN;
}

static void dw_wdt_set_timeout(uintptr_t base, uint32_t ticks)
{
	uint32_t i;

	/* Calculate the timeout range value, use __builtin_clz() to calculate
	 * the logarithm since __builtin_fls() isn't provided by GCC.
	 */
	i = 32 - __builtin_clz(ticks) - 16;

	mmio_write_32(base + DW_WDT_TIMEOUT_RANGE_REG, i | (i << 4));
}

void dw_wdt_refresh(uintptr_t base, uint32_t ticks)
{
	dw_wdt_set_timeout(base, ticks);
	mmio_write_32(base + DW_WDT_COUNTER_RESTART_REG, DW_WDT_CRR_KICK_VALUE);
}

void dw_wdt_start(uintptr_t base, uint32_t ticks)
{
	dw_wdt_set_timeout(base, ticks);

	/* Disable interrupt mode; always perform system reset. */
	mmio_clrbits_32(base + DW_WDT_CONTROL_REG, DW_WDT_RMOD);
	/* Enable watchdog. */
	mmio_setbits_32(base + DW_WDT_CONTROL_REG, DW_WDT_EN);
}
