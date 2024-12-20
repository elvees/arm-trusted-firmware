/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>

#include <platform_def.h>

#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <risc0_ipc/client/api.h>
#include <lib/psci/psci.h>

#include <plat_sip_svc.h>
#include <plat_private.h>

#define RESET_TIMEOUT 2U

uintptr_t plat_sec_entrypoint;

static int pwr_domain_on(u_register_t mpidr)
{
	int core = plat_core_pos_by_mpidr(mpidr);

	if (core < 0)
		return PSCI_E_NOT_PRESENT;

	/* Insert reset vectors for current core */
	CPU_RVBADDR_SET(core, plat_sec_entrypoint);
	isb();
	dsb();

	/* Power UP selected core */
	CPU_PPOLICY_SET(core, PPOLICY_ON);
	mdelay(10);

	/* Check status */
	if (!(CPU_PSTATUS_GET(core) & PPOLICY_ON))
		return PSCI_E_INTERN_FAIL;

	return PSCI_E_SUCCESS;
}

static void pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	/* Program the gic per-cpu distributor or re-distributor interface */
	mcom03_gic_pcpu_init();

	/* Enable the gic cpu interface */
	mcom03_gic_cpuif_enable();
}

void __dead2 system_reset(void)
{
	mcom03_sip_wdt_handler(MCOM03_SIP_WDT_START, RESET_TIMEOUT, 0, 0);

	/* Prevent waking up this cpu from wfi */
	mcom03_gic_cpuif_disable();

	/* wait for reset to assert... */
	mdelay(500);
	wfi();
	ERROR("Failed to reset system\n");
	panic();
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

static void pwr_domain_off(const psci_power_state_t *target_state)
{
	/* Disable the gic cpu interface */
	mcom03_gic_cpuif_disable();
}

static void __dead2 pwr_domain_pwr_down_wfi(const psci_power_state_t
					     *target_state)
{
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	dsbsy();

	isb();
	dsb();

	/* Power DOWN selected core */
	/* Switching the cores to WARM_RESET mode because switching to POWER_OFF
	 * mode requires a resident module in the trusted RISC0 subsystem to
	 * control the powering of the cores and to set the entry point into
	 * the software. In POWER_OFF mode there is no possibility to do
	 * this from the ARM subsystem. */
	CPU_PPOLICY_SET(core, PPOLICY_WARM_RST);

	for (;;)
		wfi();
}

static void __dead2 system_off(void)
{
	uint64_t wdt_max_timeout;

	INFO("MCom-03 System Off\n");

	wdt_max_timeout = mcom03_sip_wdt_handler(MCOM03_SIP_WDT_GET_MAX_TIMEOUT_S, 0, 0, 0);
	mcom03_sip_wdt_handler(MCOM03_SIP_WDT_SET_TIMEOUT_S, wdt_max_timeout, 0, 0);
	mcom03_gic_cpuif_disable();

	while (1) {
		// Reset watchdog every 0.8 * max_timeout seconds to avoid occasional reboots
		mcom03_sip_wdt_handler(MCOM03_SIP_WDT_PING, 0, 0, 0);
		mdelay(wdt_max_timeout * 800);
	}
}

/*
 * Export the platform handlers via plat_psci_ops. The ARM Standard
 * platform will take care of registering the handlers with PSCI.
 */
const plat_psci_ops_t plat_psci_ops = {
	.pwr_domain_on			= pwr_domain_on,
	.pwr_domain_on_finish		= pwr_domain_on_finish,
	.cpu_standby			= cpu_standby,
	.pwr_domain_off			= pwr_domain_off,
	.pwr_domain_pwr_down_wfi	= pwr_domain_pwr_down_wfi,
	.system_reset			= system_reset,
	.system_off			= system_off,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const struct plat_psci_ops **psci_ops)
{
	plat_sec_entrypoint = sec_entrypoint;
	flush_dcache_range((uint64_t)&plat_sec_entrypoint,
			   sizeof(plat_sec_entrypoint));

	VERBOSE("%s: sec_entrypoint=0x%lx\n", __func__,
		(unsigned long)plat_sec_entrypoint);

	/*
	 * Initialize PSCI ops struct
	 */
	*psci_ops = &plat_psci_ops;

	return 0;
}
