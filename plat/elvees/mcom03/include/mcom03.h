/*
 * Copyright 2019 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCOM03_H
#define MCOM03_H

#include <stdint.h>

#include <lib/xlat_tables/xlat_tables_v2.h>

void mcom03_gic_driver_init(void);
void mcom03_gic_init(void);
void mcom03_gic_cpuif_enable(void);
void mcom03_gic_cpuif_disable(void);
void mcom03_gic_pcpu_init(void);

void mcom03_mmap_setup(uintptr_t total_base, size_t total_size,
		       const struct mmap_region *mmap);

#endif /* MCOM03_H */
