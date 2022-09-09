/*
 * Copyright 2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DW_WDT_H
#define DW_WDT_H

#define DW_WDT_CONTROL_REG		0x0
#define DW_WDT_TIMEOUT_RANGE_REG	0x4
#define DW_WDT_CURRENT_COUNT_REG	0x8
#define DW_WDT_COUNTER_RESTART_REG	0xC
#define DW_WDT_CRR_KICK_VALUE		0x76
#define DW_WDT_STATUS_REG		0x10
#define DW_WDT_EOI_REG			0x14

#define DW_WDT_EN			BIT_32(0)
#define DW_WDT_RMOD			BIT_32(1)
#define DW_WDT_RPL			GENMASK_32(4, 2)

#define DW_WDT_TOP			GENMASK_32(3, 0)
#define DW_WDT_TOP_INT			GENMASK_32(7, 4)

#define DW_WDT_CCVR_MASK		GENMASK_32(31, 0)
#define DW_WDT_CRR_MASK			GENMASK_32(7, 0)
#define DW_WDT_ISR			BIT_32(0)
#define DW_WDT_ICR			BIT_32(0)

bool dw_wdt_is_enabled(uintptr_t base);
void dw_wdt_start(uintptr_t base, uint32_t ticks);
void dw_wdt_refresh(uintptr_t base, uint32_t ticks);

#endif /* DW_WDT_H */
