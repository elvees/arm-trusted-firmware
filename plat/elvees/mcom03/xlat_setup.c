/*
 *Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

#include <common/debug.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

#define PLAT_REG_REGION_BASE	0x00000000ULL
#define PLAT_REG_REGION_SIZE	0x60000000ULL

#define PLAT_GICD_REGION_BASE	PLAT_GICD_BASE
#define PLAT_GICD_REGION_SIZE	0x10000

#define PLAT_GICR_REGION_BASE	PLAT_GICR_BASE
#define PLAT_GICR_REGION_SIZE	0x80000

#define URB_REGION_BASE		PLAT_URB_BASE
#define URB_REGION_SIZE		0x10000

void mcom03_mmap_setup(uintptr_t total_base, size_t total_size,
		       const struct mmap_region *mmap)
{
	VERBOSE("Trusted RAM seen by this BL image: %p - %p\n",
		(void *)total_base, (void *)(total_base + total_size - 1UL));
	mmap_add_region(total_base, total_base, total_size,
			MT_RW_DATA | MT_SECURE);

	/* remap the code section */
	VERBOSE("Code region: %p - %p\n",
		(void *)BL_CODE_BASE, (void *)BL_CODE_END);
	mmap_add_region(BL_CODE_BASE, BL_CODE_BASE,
			round_up(BL_CODE_END, PAGE_SIZE) - BL_CODE_BASE,
			MT_CODE | MT_SECURE);

	/* Re-map the read-only data section */
	VERBOSE("Read-only data region: %p - %p\n",
		(void *)BL_RO_DATA_BASE, (void *)BL_RO_DATA_END);
	mmap_add_region(BL_RO_DATA_BASE, BL_RO_DATA_BASE,
			round_up(BL_RO_DATA_END, PAGE_SIZE) - BL_RO_DATA_BASE,
			MT_RO_DATA | MT_SECURE);

	/* register regions */
	mmap_add_region(PLAT_REG_REGION_BASE, PLAT_REG_REGION_BASE,
			PLAT_REG_REGION_SIZE, MT_DEVICE | MT_RW | MT_NS);

	mmap_add_region(PLAT_GICD_REGION_BASE, PLAT_GICD_REGION_BASE,
			PLAT_GICD_REGION_SIZE, MT_DEVICE | MT_RW | MT_SECURE);

	mmap_add_region(PLAT_GICR_REGION_BASE, PLAT_GICR_REGION_BASE,
			PLAT_GICR_REGION_SIZE, MT_DEVICE | MT_RW | MT_SECURE);

	mmap_add_region(URB_REGION_BASE, URB_REGION_BASE,
			URB_REGION_SIZE,
			MT_DEVICE | MT_RW | MT_SECURE);

	/* Map secure DDR for shared mem */
	mmap_add_region(PLAT_BL31_USER_MEMORY_BASE, PLAT_BL31_USER_MEMORY_BASE,
			PLAT_BL31_USER_MEMORY_SIZE,
			MT_NON_CACHEABLE | MT_RW | MT_SECURE);


	/* additional regions if needed */
	if (mmap)
		mmap_add(mmap);

	init_xlat_tables();
}
