/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

#include <arch.h>

#include <plat_private.h>

unsigned char plat_tree_desc[PLAT_CLUSTER_COUNT + 1];

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	unsigned int cluster_id, cpu_id;

	cluster_id = MPIDR_AFFLVL1_VAL(mpidr);
	if (cluster_id >= PLAT_CLUSTER_COUNT)
		return -1;

	cpu_id = MPIDR_AFFLVL0_VAL(mpidr);
	if (cpu_id >= PLAT_MAX_CORES_PER_CLUSTER)
		return -1;

	return cpu_id;
}

const unsigned char *plat_get_power_domain_tree_desc(void)
{
	int i;

	plat_tree_desc[0] = PLAT_CLUSTER_COUNT;

	for (i = 0; i < PLAT_CLUSTER_COUNT; i++)
		plat_tree_desc[i + 1] = PLAT_MAX_CORES_PER_CLUSTER;

	return plat_tree_desc;
}
