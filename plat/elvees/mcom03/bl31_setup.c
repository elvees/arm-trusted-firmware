/*
 * Copyright 2019 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <platform_def.h>

#include <arch.h>
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <drivers/ti/uart/uart_16550.h>
#include <drivers/generic_delay_timer.h>

#include <mcom03.h>

static console_t console;
static entry_point_info_t bl33_image_ep_info;

IMPORT_SYM(uintptr_t, __SPM_SHIM_EXCEPTIONS_START__, SPM_SHIM_EXCEPTIONS_START);
IMPORT_SYM(uintptr_t, __SPM_SHIM_EXCEPTIONS_END__,   SPM_SHIM_EXCEPTIONS_END);
IMPORT_SYM(uintptr_t, __SPM_SHIM_EXCEPTIONS_LMA__,   SPM_SHIM_EXCEPTIONS_LMA);

entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	assert(sec_state_is_valid(type));
	if (type == NON_SECURE)
		return &bl33_image_ep_info;

	return NULL;
}

/*******************************************************************************
 * Gets SPSR for BL33 entry
 ******************************************************************************/
static uint32_t get_spsr_for_bl33_entry(void)
{
	unsigned long el_status;
	unsigned int mode;
	uint32_t spsr;

	/* Figure out what mode we enter the non-secure world in */
	el_status = read_id_aa64pfr0_el1() >> ID_AA64PFR0_EL2_SHIFT;
	el_status &= ID_AA64PFR0_ELX_MASK;

	mode = (el_status) ? MODE_EL2 : MODE_EL1;

	spsr = SPSR_64(mode, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	return spsr;
}

void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
				u_register_t arg2, u_register_t arg3)
{
	/* Initialize the console to provide early debug support */
	console_16550_register(PLAT_BOOT_UART_BASE,
			       PLAT_BOOT_UART_CLK_IN_HZ,
			       PLAT_CONSOLE_BAUDRATE, &console);

	console_set_scope(&console, CONSOLE_FLAG_BOOT |
			  CONSOLE_FLAG_RUNTIME);

	/* There are no parameters from BL2 if BL31 is a reset vector */
	assert(arg0 == 0U);
	assert(arg1 == 0U);

	/* Populate entry point information for BL33 */
	SET_PARAM_HEAD(&bl33_image_ep_info, PARAM_EP, VERSION_1, 0);
	/*
	 * Tell BL31 where the non-trusted software image
	 * is located and the entry state information
	 */
	bl33_image_ep_info.pc = BL33_BASE;
	bl33_image_ep_info.spsr = get_spsr_for_bl33_entry();
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);
}

static void configure_sys_timer(void)
{
	generic_delay_timer_init();
}

void bl31_platform_setup(void)
{
	/* SMP coherence is already enabled */

	/* Initialize the GIC driver, cpu and distributor interfaces */
	mcom03_gic_driver_init();
	mcom03_gic_init();

	/* Enable and initialize the System level generic timer */
	configure_sys_timer();
}

void bl31_plat_runtime_setup(void)
{
}

void bl31_plat_arch_setup(void)
{
	mcom03_mmap_setup(BL31_BASE, BL31_SIZE, NULL);
	enable_mmu_el3(XLAT_TABLE_NC);
}

unsigned int plat_get_syscnt_freq2(void)
{
	return PLAT_TIMER_CLK;
}
