/*
 * Copyright 2019 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>

#include <platform_def.h>

#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/psci/psci.h>

#include <mcom03.h>

uintptr_t plat_sec_entrypoint;

static int pwr_domain_on(u_register_t mpidr)
{
	int cpu = plat_core_pos_by_mpidr(mpidr);

	if (cpu < 0)
		return PSCI_E_NOT_PRESENT;

	/* power UP selected core */
	CPU_PPOLICY_SET(cpu, PPOLICY_ON);
	mdelay(10);

	/* check status */
	if (!(CPU_PSTATUS_GET(cpu) & PPOLICY_ON))
		return PSCI_E_NOT_PRESENT;

	return PSCI_E_SUCCESS;
}

static void pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	/* Program the gic per-cpu distributor or re-distributor interface */
	mcom03_gic_pcpu_init();

	/* Enable the gic cpu interface */
	mcom03_gic_cpuif_enable();
}

static void cpu_standby(plat_local_state_t cpu_state)
{
	unsigned int scr;

	assert(cpu_state == PLAT_LOCAL_STATE_RET);

	scr = read_scr_el3();
	/* Enable PhysicalIRQ bit for NS world to wake the CPU */
	write_scr_el3(scr | SCR_IRQ_BIT);
	isb();
	dsb();
	wfi();

	write_scr_el3(scr);
}

/* optional ?? */
const plat_psci_ops_t plat_psci_ops = {
	.pwr_domain_on		= pwr_domain_on,
	.pwr_domain_on_finish	= pwr_domain_on_finish,
	.cpu_standby		= cpu_standby,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const struct plat_psci_ops **psci_ops)
{
	plat_sec_entrypoint = sec_entrypoint;
	flush_dcache_range((uint64_t)&plat_sec_entrypoint,
			   sizeof(plat_sec_entrypoint));

	*psci_ops = &plat_psci_ops;

	return 0;
}
