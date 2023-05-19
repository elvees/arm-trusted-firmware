/*
 * Copyright 2025 RnD Center "ELVEES", JSC
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

DEFINE_RISC0_IPC_SEND(pm);

static risc0_ipc_cmd_t cmd = { .hdr.service = RISC0_IPC_PM };

static uint64_t mcom03_sip_pm_send(unsigned int core, uint32_t func, uint64_t domain_id)
{
	uint64_t ret;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_PM)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = func;
		cmd.param.pm.toggle.id = domain_id;
		pm_service_send(&cmd, resp);
		ret = resp->param.pm.response.value;
		risc0_ipc_free_resp_buffer(resp);
	} else {
		/* ARM core unable to access TOP registers needed to
		 * enable power domain, so return error here */
		ret = SMC_UNK;
	}

	return ret;
}

uint64_t mcom03_sip_pm_handler(uint64_t id, uint64_t arg0)
{
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	switch (id) {
	case MCOM03_SIP_POWER_DOMAIN_CHECK_SUPPORT:
		return mcom03_sip_pm_send(core, RISC0_IPC_PM_FUNC_CHECK_SUPPORT, arg0);
	case MCOM03_SIP_POWER_DOMAIN_ENABLE:
		return mcom03_sip_pm_send(core, RISC0_IPC_PM_FUNC_ENABLE, arg0);
	case MCOM03_SIP_POWER_DOMAIN_DISABLE:
		return mcom03_sip_pm_send(core, RISC0_IPC_PM_FUNC_DISABLE, arg0);
	default:
		ERROR("%s: unhandled SiP command (0x%lx)\n", __func__, id);
		return SMC_UNK;
	}
}
