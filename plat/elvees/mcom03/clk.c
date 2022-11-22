/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <platform_def.h>

#include <common/debug.h>

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

int mcom03_get_apb_clk(void)
{
	int service_subs_pll;

	service_subs_pll = get_pll_freq(PLAT_SERVICE_SUBS_PLLCFG);

	return get_ucg_freq(PLAT_SERVICE_SUBS_UCG1, 0, service_subs_pll);
}
