/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAT_PRIVATE_H
#define PLAT_PRIVATE_H

#include <stdint.h>
#include <cdefs.h>
#include <platform_def.h>

#include <plat/common/platform.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <lib/mmio.h>

#define CPU_PPOLICY_REG(core)	(PLAT_URB_BASE + (core) * 16U)
#define CPU_PSTATUS_REG(core)	(PLAT_URB_BASE + (core) * 16U + 4U)

#define CPU_RVBADDR_OFFSET		(0x0118U)
#define CPU_RVBADDR_HI(core) \
	(PLAT_URB_BASE + CPU_RVBADDR_OFFSET + (core) * 8U)

#define CPU_RVBADDR_LO(core) \
	(PLAT_URB_BASE + CPU_RVBADDR_OFFSET + 4U + (core) * 8U)

#define CPU_RVBADDR_SET(core, entrypoint) \
	do { \
		mmio_write_32(CPU_RVBADDR_HI(core), \
				((uint32_t)((entrypoint) >> 32U))); \
		mmio_write_32(CPU_RVBADDR_LO(core), \
				(uint32_t)(entrypoint)); \
	} while (0)

#define SYS_PPOLICY_REG	CPU_PPOLICY_REG(PLATFORM_CORE_COUNT)
#define SYS_PSTATUS_REG	CPU_PSTATUS_REG(PLATFORM_CORE_COUNT)

#define CPU_PPOLICY_SET(core, val) \
	mmio_write_32(CPU_PPOLICY_REG(core), val)

#define CPU_PPOLICY_GET(core) \
	mmio_read_32(CPU_PPOLICY_REG(core))

#define CPU_PSTATUS_GET(core) \
	mmio_read_32(CPU_PSTATUS_REG(core))

#define SYS_PPOLICY_SET(val) \
	mmio_write_32(SYS_PPOLICY_REG, val)

#define SYS_PPOLICY_GET() \
	mmio_read_32(SYS_PPOLICY_REG)

#define SYS_PSTATUS_GET() \
	mmio_read_32(SYS_PSTATUS_REG)

void mcom03_gic_driver_init(void);
void mcom03_gic_init(void);
void mcom03_gic_cpuif_enable(void);
void mcom03_gic_cpuif_disable(void);
void mcom03_gic_pcpu_init(void);

void mcom03_mmap_setup(uintptr_t total_base, size_t total_size,
		       const struct mmap_region *mmap);

int mcom03_get_apb_clk(void);

#endif /* PLAT_PRIVATE_H */
