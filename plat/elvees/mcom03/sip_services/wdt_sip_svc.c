/*
 * Copyright 2019-2023 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <assert.h>

#include <common/runtime_svc.h>

#include <plat_sip_svc.h>
#include <plat_private.h>
#include <arch_helpers.h>

#include <risc0_ipc/client/api.h>
#include <drivers/synopsys/dw_wdt.h>

DEFINE_RISC0_IPC_SEND(wdt);

static risc0_ipc_cmd_t cmd = { .hdr.service = RISC0_IPC_WDT };

static uint64_t mcom03_sip_wdt_is_enable(unsigned int core, uint32_t param)
{
	uint64_t ret;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_IS_ENABLE;
		wdt_service_send(&cmd, resp);
		ret = resp->param.wdt.isEnable.value;
		risc0_ipc_free_resp_buffer(resp);
	} else {
		ret = dw_wdt_is_enabled(PLAT_WDT0_BASE);
	}

	return ret;
}

static void mcom03_sip_wdt_start(unsigned int core, uint32_t param)
{
	int clk_apb;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_START;
		cmd.param.wdt.start.timeout = param;
		wdt_service_send(&cmd, NULL);
	} else {
		clk_apb = mcom03_get_apb_clk();
		dw_wdt_start(PLAT_WDT0_BASE, param, clk_apb);
	}
}

static void mcom03_sip_wdt_ping(unsigned int core, uint32_t param)
{
	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_PING;
		wdt_service_send(&cmd, NULL);
	} else {
		dw_wdt_reset(PLAT_WDT0_BASE);
	}
}

static void mcom03_sip_wdt_set_timeout(unsigned int core, uint32_t param)
{
	int clk_apb;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_SET_TIMEOUT_S;
		cmd.param.wdt.setTimeout.value = param;
		wdt_service_send(&cmd, NULL);
	} else {
		clk_apb = mcom03_get_apb_clk();
		dw_wdt_set_timeout(PLAT_WDT0_BASE, param, clk_apb);
	}
}

static uint64_t mcom03_sip_wdt_get_timeout(unsigned int core, uint32_t param)
{
	uint64_t ret;
	int clk_apb;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_GET_TIMEOUT_S;
		wdt_service_send(&cmd, resp);
		ret = resp->param.wdt.timeout.value;
		risc0_ipc_free_resp_buffer(resp);
	} else {
		clk_apb = mcom03_get_apb_clk();
		ret = dw_wdt_get_timeout(PLAT_WDT0_BASE, clk_apb);
	}

	return ret;
}

static uint64_t mcom03_sip_wdt_get_max_timeout(unsigned int core, uint32_t param)
{
	uint64_t ret;
	int clk_apb;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_GET_MAX_TIMEOUT_S;
		wdt_service_send(&cmd, resp);
		ret = resp->param.wdt.maxTimeout.value;
		risc0_ipc_free_resp_buffer(resp);
	} else {
		clk_apb = mcom03_get_apb_clk();
		ret = dw_wdt_get_max_timeout(PLAT_WDT0_BASE, clk_apb);
	}

	return ret;
}

static uint64_t mcom03_sip_wdt_get_min_timeout(unsigned int core, uint32_t param)
{
	uint64_t ret;
	int clk_apb;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_WDT)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_WDT_FUNC_GET_MIN_TIMEOUT_S;
		wdt_service_send(&cmd, resp);
		ret = resp->param.wdt.minTimeout.value;
		risc0_ipc_free_resp_buffer(resp);
	} else {
		clk_apb = mcom03_get_apb_clk();
		ret = dw_wdt_get_min_timeout(PLAT_WDT0_BASE, clk_apb);
	}

	return ret;
}

uint64_t mcom03_sip_wdt_handler(uint64_t id, uint64_t param,
				uint64_t arg0, uint64_t arg1)
{
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	switch (id) {
	case MCOM03_SIP_WDT_IS_ENABLE:
		return mcom03_sip_wdt_is_enable(core, param);
	case MCOM03_SIP_WDT_START:
		mcom03_sip_wdt_start(core, param);
		return SMC_OK;
	case MCOM03_SIP_WDT_PING:
		mcom03_sip_wdt_ping(core, param);
		return SMC_OK;
	case MCOM03_SIP_WDT_SET_TIMEOUT_S:
		mcom03_sip_wdt_set_timeout(core, param);
		return SMC_OK;
	case MCOM03_SIP_WDT_GET_TIMEOUT_S:
		return mcom03_sip_wdt_get_timeout(core, param);
	case MCOM03_SIP_WDT_GET_MAX_TIMEOUT_S:
		return mcom03_sip_wdt_get_max_timeout(core, param);
	case MCOM03_SIP_WDT_GET_MIN_TIMEOUT_S:
		return mcom03_sip_wdt_get_min_timeout(core, param);
	default:
		ERROR("%s: unhandled SiP command (0x%lx)\n", __func__, id);
		return SMC_UNK;
	}
}
