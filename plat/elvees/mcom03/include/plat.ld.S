/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCOM03_PLAT_LD_S
#define MCOM03_PLAT_LD_S

#include <platform_def.h>

MEMORY {
    USER (rwx): ORIGIN = PLAT_BL31_USER_MEMORY_BASE, LENGTH = PLAT_BL31_USER_MEMORY_SIZE
}

SECTIONS
{
    . = PLAT_BL31_USER_MEMORY_BASE;
    ASSERT(. == ALIGN(PAGE_SIZE),
           "PLAT_BL31_USER_MEMORY_BASE address is not aligned on a page boundary.")
    .user.mem (NOLOAD) :
    {
        __USER_MEMORY_START__ = .;
        *(.sharedmem)
        __USER_MEMORY_END__ = .;
    } > USER

    ASSERT(__USER_MEMORY_END__ < (PLAT_BL31_USER_MEMORY_BASE + PLAT_BL31_USER_MEMORY_SIZE),
           "User memory overlaps boundary")
}

#endif /* MCOM03_PLAT_LD_S */
