/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>
#include <lib/utils_def.h>
#include <plat/common/common_def.h>

/* CPU topology */
#define PLAT_MAX_CORES_PER_CLUSTER	4
#define PLAT_CLUSTER_COUNT		1
#define PLATFORM_CORE_COUNT		(PLAT_CLUSTER_COUNT *	\
					 PLAT_MAX_CORES_PER_CLUSTER)

#define PLAT_MAX_PWR_LVL		MPIDR_AFFLVL1
#define PLAT_MAX_RET_STATE		MPIDR_AFFLVL1
#define PLAT_MAX_OFF_STATE		MPIDR_AFFLVL2

#define PLAT_LOCAL_STATE_RUN		0
#define PLAT_LOCAL_STATE_RET		1
#define PLAT_LOCAL_STATE_OFF		2

#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 40)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 39)
#define MAX_XLAT_TABLES			8
#define MAX_MMAP_REGIONS		8

#define PLATFORM_STACK_SIZE		0x400

#define BL31_BASE			0x80000000
#define BL31_SIZE			0x00080000
#define BL31_LIMIT			(BL31_BASE + BL31_SIZE)

#define BL33_BASE			BL31_LIMIT

/* TIMER related constants */
#ifdef PLAT_MCOM03_HAPS
#define PLAT_TIMER_CLK			5000000
#define PLAT_BOOT_UART_BASE		0x1640000
#define PLAT_BOOT_UART_CLK_IN_HZ	14750000
#else
#define PLAT_TIMER_CLK			27000000
#define PLAT_BOOT_UART_BASE		0x1750000
#define PLAT_BOOT_UART_CLK_IN_HZ	27000000
#endif

#define PLAT_CONSOLE_BAUDRATE		115200

#define PLAT_GICD_BASE			0x1100000
#define PLAT_GICR_BASE			0x1180000

#define PLAT_IRQ_SEC_PHY_TIMER		29

#define PLAT_IRQ_SEC_SGI_0		8
#define PLAT_IRQ_SEC_SGI_1		9
#define PLAT_IRQ_SEC_SGI_2		10
#define PLAT_IRQ_SEC_SGI_3		11
#define PLAT_IRQ_SEC_SGI_4		12
#define PLAT_IRQ_SEC_SGI_5		13
#define PLAT_IRQ_SEC_SGI_6		14
#define PLAT_IRQ_SEC_SGI_7		15

#define MAX_INTR_EL3			159

#define PLAT_URB_BASE			0x1000000

#define PPOLICY_OFF			0x1
#define PPOLICY_WARM_RST		0x8
#define PPOLICY_ON			0x10

#endif /* PLATFORM_DEF_H */
