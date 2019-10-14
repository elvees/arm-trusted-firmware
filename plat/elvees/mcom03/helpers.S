/*
 * Copyright 2019 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <platform_def.h>

.global plat_my_core_pos
.global platform_mem_init
.global plat_is_my_cpu_primary
.global plat_secondary_cold_boot_setup
.global plat_crash_console_init
.global plat_crash_console_putc
.global plat_crash_console_flush

func plat_my_core_pos
	mrs	x0, mpidr_el1
	b	plat_core_pos_by_mpidr
endfunc plat_my_core_pos

func platform_mem_init
	ret
endfunc platform_mem_init

/*
 * Secondary CPUs are placed in a holding pen, waiting for their mailbox
 * to be populated. Note that all CPUs share the same mailbox ; therefore,
 * populating it will release all CPUs from their holding pen. If
 * finer-grained control is needed then this should be handled in the
 * code that secondary CPUs jump to.
 */
func plat_secondary_cold_boot_setup
	ldr	x0, plat_sec_entrypoint

	/* Wait until the mailbox gets populated */
poll_mailbox:
	cbz	x0, 1f
	br	x0
1:
	wfe
	b	poll_mailbox
endfunc plat_secondary_cold_boot_setup

/*
 * Find out whether the current cpu is the primary
 * cpu (applicable only after a cold boot)
 */
func plat_is_my_cpu_primary
	mov	x9, x30
	bl	plat_my_core_pos
	cmp	x0, 0
	cset	w0, eq
	ret	x9
endfunc plat_is_my_cpu_primary

/*
 * int plat_crash_console_init(void)
 * Function to initialize the crash console
 * without a C Runtime to print crash report.
 * Clobber list : x0, x1, x2
 */
func plat_crash_console_init
	mov_imm x0, PLAT_BOOT_UART_BASE
	mov_imm x1, PLAT_BOOT_UART_CLK_IN_HZ
	mov_imm x2, PLAT_CONSOLE_BAUDRATE
	b	console_16550_core_init
endfunc plat_crash_console_init

/*
 * int plat_crash_console_putc(int c)
 * Function to print a character on the crash
 * console without a C Runtime.
 * Clobber list : x1, x2
 */
func plat_crash_console_putc
	mov_imm	x1, PLAT_BOOT_UART_BASE
	b	console_16550_core_putc
endfunc plat_crash_console_putc

/*
 * int plat_crash_console_flush(int c)
 * Function to force a write of all buffered
 * data that hasn't been output.
 * Out : return -1 on error else return 0.
 * Clobber list : x0, x1
 */
func plat_crash_console_flush
	mov_imm	x0, PLAT_BOOT_UART_BASE
	b	console_16550_core_flush
endfunc plat_crash_console_flush
