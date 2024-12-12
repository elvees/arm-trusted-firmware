/*
 * Copyright 2023 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <common/runtime_svc.h>

#include <plat_sip_svc.h>
#include <plat_private.h>
#include <risc0_ipc/client/api.h>

#define DDR_SUBS_URB_BASE	0xC000000
#define HSPERIPH_BAR		0xDC
#define GPU_BAR			0xD8
#define LSPERIPH0_BAR		0xE0
#define LSPERIPH1_BAR		0xE4

DEFINE_RISC0_IPC_SEND(ddr_subs);

static risc0_ipc_cmd_t cmd = { .hdr.service = RISC0_IPC_DDR_SUBS };

uint64_t mcom03_sip_ddr_subs_handler(uint64_t id, uint64_t param,
				     uint64_t arg0, uint64_t arg1)
{
	uintptr_t addr;
	risc0_ipc_ddr_subs_func mbox_func;

	switch (id) {
	case MCOM03_SIP_DDR_SUBS_SET_HSPERIPH_BAR:
		addr = DDR_SUBS_URB_BASE + HSPERIPH_BAR;
		mbox_func = RISC0_IPC_DDR_SUBS_FUNC_SET_HSPERIPH_BAR;
		break;
	case MCOM03_SIP_DDR_SUBS_SET_LSPERIPH0_BAR:
		addr = DDR_SUBS_URB_BASE + LSPERIPH0_BAR;
		mbox_func = RISC0_IPC_DDR_SUBS_FUNC_SET_LSPERIPH0_BAR;
		break;
	case MCOM03_SIP_DDR_SUBS_SET_LSPERIPH1_BAR:
		addr = DDR_SUBS_URB_BASE + LSPERIPH1_BAR;
		mbox_func = RISC0_IPC_DDR_SUBS_FUNC_SET_LSPERIPH1_BAR;
		break;
	case MCOM03_SIP_DDR_SUBS_SET_GPU_BAR:
		addr = DDR_SUBS_URB_BASE + GPU_BAR;
		mbox_func = RISC0_IPC_DDR_SUBS_FUNC_SET_GPU_BAR;
		break;
	default:
		ERROR("%s: unhandled SiP command (0x%lx)\n", __func__, id);
		return SMC_UNK;
	}

	if (risc0_ipc_get_capability() & BIT(RISC0_IPC_DDR_SUBS)) {
		cmd.hdr.func = mbox_func;
		cmd.param.ddr_subs.bar.value = param;
		ddr_subs_service_send(&cmd, NULL);
	} else {
		mmio_write_32(addr, param);
	}

	return SMC_OK;
}
