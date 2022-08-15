/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <platform_def.h>

#include <common/interrupt_props.h>
#include <drivers/arm/gicv3.h>
#include <plat/common/platform.h>

#include <plat_private.h>

static uintptr_t rdistif_base_addrs[PLATFORM_CORE_COUNT];

static const interrupt_prop_t interrupt_props[] = {
	/* G0 interrupts */
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_0, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP0, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_6, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP0, GIC_INTR_CFG_EDGE),

	/* G1S interrupts */
	INTR_PROP_DESC(PLAT_IRQ_SEC_PHY_TIMER, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_LEVEL),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_1, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_2, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_3, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_4, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_5, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE),
	INTR_PROP_DESC(PLAT_IRQ_SEC_SGI_7, GIC_HIGHEST_SEC_PRIORITY,
		       INTR_GROUP1S, GIC_INTR_CFG_EDGE)
};

static unsigned int mpidr_to_core_pos(u_register_t mpidr)
{
	return plat_core_pos_by_mpidr(mpidr);
}

static const struct gicv3_driver_data gic_driver_data = {
	.gicd_base = PLAT_GICD_BASE,
	.gicr_base = PLAT_GICR_BASE,
	.interrupt_props = interrupt_props,
	.interrupt_props_num = ARRAY_SIZE(interrupt_props),
	.rdistif_num = PLATFORM_CORE_COUNT,
	.rdistif_base_addrs = rdistif_base_addrs,
	.mpidr_to_core_pos = mpidr_to_core_pos,
};

void mcom03_gic_driver_init(void)
{
	gicv3_driver_init(&gic_driver_data);
}

void mcom03_gic_init(void)
{
	gicv3_distif_init();
	gicv3_rdistif_init(plat_my_core_pos());
	gicv3_cpuif_enable(plat_my_core_pos());
}

void mcom03_gic_cpuif_enable(void)
{
	gicv3_cpuif_enable(plat_my_core_pos());
}

void mcom03_gic_cpuif_disable(void)
{
	gicv3_cpuif_disable(plat_my_core_pos());
}

void mcom03_gic_pcpu_init(void)
{
	gicv3_rdistif_init(plat_my_core_pos());
}
