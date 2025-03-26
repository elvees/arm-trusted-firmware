/*
 * Copyright 2025 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <common/runtime_svc.h>
#include <errno.h>

#include <plat_sip_svc.h>
#include <plat_private.h>
#include <risc0_ipc/client/api.h>

DEFINE_RISC0_IPC_SEND(bootstage);

static risc0_ipc_cmd_t cmd = { .hdr.service = RISC0_IPC_BOOTSTAGE };

static uint64_t mcom03_sip_bootstage_set_stage(uint32_t id)
{
	uint64_t ret = -ENOENT;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_BOOTSTAGE)) {
		cmd.hdr.func = RISC0_IPC_BOOTSTAGE_FUNC_SET_STAGE;
		cmd.param.bootstage.set_stage.id = id;
		bootstage_service_send(&cmd, NULL);
		ret = SMC_OK;
	}

	return ret;
}

static uint64_t mcom03_sip_bootstage_get_timestamp(unsigned int core, uint32_t id)
{
	uint64_t ret = -ENOENT;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_BOOTSTAGE)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_BOOTSTAGE_FUNC_GET_TIMESTAMP;
		cmd.param.bootstage.get_timestamp.id = id;
		bootstage_service_send(&cmd, resp);
		ret = resp->param.bootstage.get_timestamp.value;
		risc0_ipc_free_resp_buffer(resp);
	}

	return ret;
}

uint64_t mcom03_sip_bootstage_handler(uint64_t id, uint64_t arg0)
{
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	switch (id) {
	case MCOM03_SIP_BOOTSTAGE_SET_STAGE:
		return mcom03_sip_bootstage_set_stage(arg0);
	case MCOM03_SIP_BOOTSTAGE_GET_TIMESTAMP:
		return mcom03_sip_bootstage_get_timestamp(core, arg0);
	default:
		ERROR("%s: unhandled SiP command (0x%lx)\n", __func__, id);
		return SMC_UNK;
	}
}
