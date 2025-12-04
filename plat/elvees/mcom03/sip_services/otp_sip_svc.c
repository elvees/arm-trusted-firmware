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

DEFINE_RISC0_IPC_SEND(otp);

static risc0_ipc_cmd_t cmd = { .hdr.service = RISC0_IPC_OTP };

static uint64_t mcom03r_sip_otp_get_dump(unsigned int core, uint64_t buf, uint64_t size)
{
	int ret = -ENOENT;
	risc0_ipc_resp_t *resp;

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_OTP)) {
		resp = risc0_ipc_alloc_resp_buffer(core);
		cmd.hdr.func = RISC0_IPC_OTP_FUNC_GET_DUMP;
		cmd.param.otp.get_dump.buf = buf;
		cmd.param.otp.get_dump.size = (uint32_t)size;
		otp_service_send(&cmd, resp);
		ret = resp->param.otp.get_dump.error;
		risc0_ipc_free_resp_buffer(resp);
	}

	return (uint64_t)ret;
}

uint64_t mcom03r_sip_otp_handler(uint64_t id, uint64_t param0, uint64_t param1)
{
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	switch (id) {
	case MCOM03R_SIP_OTP_GET_DUMP:
		return mcom03r_sip_otp_get_dump(core, param0, param1);
	default:
		ERROR("%s: unhandled SiP command (0x%lx)\n", __func__, id);
		return SMC_UNK;
	}
}
