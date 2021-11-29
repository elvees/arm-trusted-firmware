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
#include <drivers/synopsys/dw_wdt.h>
#include <lib/psci/psci.h>

#include <plat_private.h>

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

static int get_pll_freq(uintptr_t pll_cfg)
{
	int nr, nf, od;
	uint32_t pll;

	pll = mmio_read_32(pll_cfg);
	if (!(pll & PLL_CFG_LOCK))
		WARN("PLL is not locked\n");

	if (pll & PLL_CFG_MAN) {
		nr = (pll & PLL_CFG_NR) >> 27;
		nf = (pll & PLL_CFG_NF) >> 14;
		od = (pll & PLL_CFG_OD) >> 10;
		return XTI_CLOCK * (nf + 1) / (nr + 1) / (od + 1);
	} else
		return XTI_CLOCK * ((pll & PLL_CFG_SEL) + 1);
}

static int get_ucg_freq(uintptr_t ucg_base, int ucg_channel, int parent_rate)
{
	int div;
	uint32_t ctr;

	ctr = mmio_read_32(UCG_CTR_REG(ucg_base, ucg_channel));
	if (!(ctr & UCG_CTR_DIV_LOCK))
		WARN("Clock divider is not locked\n");

	div = (ctr & UCG_CTR_DIV_COEFF) >> 10;
	if (div == 0)
		div = 1;

	if (mmio_read_32(UCG_BP_CTR_REG(ucg_base)) & BIT(ucg_channel))
		return XTI_CLOCK;
	else
		return parent_rate / div;
}

void __dead2 system_reset(void)
{
	int clk_apb, service_subs_pll;

	service_subs_pll = get_pll_freq(PLAT_SERVICE_SUBS_PLLCFG);
	clk_apb = get_ucg_freq(PLAT_UCG1_BASE, 0, service_subs_pll);

	/* Set timeout to ~2s and enable WDT if it is disabled */
	if (dw_wdt_is_enabled(PLAT_WDT0_BASE))
		dw_wdt_refresh(PLAT_WDT0_BASE, clk_apb * 2);
	else
		dw_wdt_start(PLAT_WDT0_BASE, clk_apb * 2);

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
