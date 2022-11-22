/*
 * Copyright 2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/synopsys/dw_wdt.h>
#include <lib/mmio.h>
#include <platform_def.h>

#define DW_WDT_MAX_TOP	15

bool dw_wdt_is_enabled(uintptr_t base)
{
	return mmio_read_32(base + DW_WDT_CONTROL_REG) & DW_WDT_EN;
}

void dw_wdt_set_timeout(uintptr_t base, uint32_t timeout_sec, uint32_t ticks_per_sec)
{
	uint32_t ticks;
	int i = 0;

	ticks = timeout_sec * ticks_per_sec;
	for (i = 0; i < DW_WDT_MAX_TOP; i++) {
		if (ticks <= ((1 << 16) << i)) {
			break;
		}
	}
	mmio_write_32(base + DW_WDT_TIMEOUT_RANGE_REG, i | (i << 4));
}

uint32_t dw_wdt_get_timeout(uintptr_t base, uint32_t ticks_per_sec)
{
	uint32_t top;

	top = mmio_read_32(base + DW_WDT_TIMEOUT_RANGE_REG) & 0xF;
	return (1U << (16 + top)) / ticks_per_sec;
}

uint32_t dw_wdt_get_max_timeout(uintptr_t base, uint32_t ticks_per_sec)
{
	return (1U << (16 + DW_WDT_MAX_TOP)) / ticks_per_sec;
}

uint32_t dw_wdt_get_min_timeout(uintptr_t base, uint32_t ticks_per_sec)
{
	return (1U << 16) / ticks_per_sec;
}

void dw_wdt_reset(uintptr_t base)
{
	mmio_write_32(base + DW_WDT_COUNTER_RESTART_REG, DW_WDT_CRR_KICK_VALUE);
}

void dw_wdt_start(uintptr_t base, uint32_t timeout_sec, uint32_t ticks_per_sec)
{
	dw_wdt_set_timeout(base, timeout_sec, ticks_per_sec);

	/* Disable interrupt mode; always perform system reset. */
	mmio_clrbits_32(base + DW_WDT_CONTROL_REG, DW_WDT_RMOD);
	/* Enable watchdog. */
	mmio_setbits_32(base + DW_WDT_CONTROL_REG, DW_WDT_EN);

	dw_wdt_reset(base);
}
