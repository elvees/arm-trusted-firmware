/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <plat_private.h>

#include <drivers/delay_timer.h>
#include <drivers/mailbox/mailbox.h>

#include "api.h"

CASSERT((sizeof(risc0_ipc_hdr_t) % sizeof(uint32_t)) == 0U,
	risc0_ipc_hdr_t_is_not_aligned);
CASSERT((sizeof(risc0_ipc_cmd_t) % sizeof(uint32_t)) == 0U,
	risc0_ipc_cmd_t_is_not_aligned);
CASSERT((sizeof(risc0_ipc_resp_t) % sizeof(uint32_t)) == 0U,
	risc0_ipc_resp_t_is_not_aligned);

static uint32_t capability;

static risc0_ipc_resp_t __section(".sharedmem") responses[PLATFORM_CORE_COUNT] = {0};

static int risc0_ipc_wait_for_resp(const risc0_ipc_resp_t *resp, uint32_t timeout)
{
	uint64_t expire;

	if (resp == NULL)
		return -EINVAL;

	if (timeout)
		expire = timeout_init_us(timeout * 1000U);

	/* Wait until the response message would be written */
	while (resp->state.value != RISC0_IPC_RESP_STATE_COMPLETE) {
		dmbsy();
		if (timeout)
			if (timeout_elapsed(expire))
				return -ETIMEDOUT;
	}

	return 0;
}

static int risc0_ipc_message_write(const uint8_t *data, uint32_t size)
{
	int count = 0;

	count = mailbox_message_write(PLAT_MAILBOX_FIFO_NUMBER, data, size, 100U);
	if (count < 0)
		return count;
	if (count != size)
		return -EIO;
	return 0;
}

int risc0_ipc_start(void)
{
	int ret;
	u_register_t mpidr = read_mpidr_el1();
	unsigned int core = plat_core_pos_by_mpidr(mpidr);

	mailbox_init((void *)PLAT_MAILBOX_BASE);

	risc0_ipc_cmd_t cmd = {
		.hdr.service = RISC0_IPC_INIT,
		.hdr.func = RISC0_IPC_INIT_FUNC_GET_CAPABILITY,
	};

	capability = 0U;
	ret = risc0_ipc_send(&cmd, &responses[core]);
	if (ret == 0U)
		capability = responses[core].param.init.capability.value;
	else
		INFO("Mailbox services are not found, ret = %d\n", ret);

	return ret;
}

int risc0_ipc_send(const risc0_ipc_cmd_t *cmd,
		   risc0_ipc_resp_t *resp)
{
	int ret = 0;

	risc0_ipc_hdr_t hdr = {
		.magic_num = RISC0_IPC_MAGIC,
		.cmd_len = sizeof(risc0_ipc_cmd_t),
		.shrmem_len = 0,
	};

	risc0_ipc_shrmem_t shrmem = {
		.data = (uint64_t)resp
	};

	if (cmd == NULL) {
		return -EINVAL;
	}

	ret = mailbox_message_start(PLAT_MAILBOX_FIFO_NUMBER, 100U);
	if (ret)
		goto exit;

	if (resp != NULL)
		hdr.shrmem_len = sizeof(shrmem);

	dmbsy();
	ret = risc0_ipc_message_write((const uint8_t *)&hdr, sizeof(hdr));
	if (ret)
		goto exit;

	dmbsy();
	ret = mailbox_notify(PLAT_MAILBOX_FIFO_NUMBER);
	if (ret)
		goto exit;

	ret = risc0_ipc_message_write((const uint8_t *)cmd, sizeof(*cmd));
	if (ret)
		goto exit;

	if (resp != NULL) {
		memset((void *)shrmem.data, 0U, sizeof(risc0_ipc_resp_t));

		dmbsy();
		ret = risc0_ipc_message_write((const uint8_t *)&shrmem, sizeof(shrmem));
		if (ret)
			goto exit;

		ret = risc0_ipc_wait_for_resp(resp, 100U);
		if (ret)
			goto exit;
	}

exit:
	mailbox_message_end();

	return ret;
}

risc0_ipc_resp_t *risc0_ipc_alloc_resp_buffer(unsigned int core)
{
	return (risc0_ipc_resp_t *)&responses[core];
}

int risc0_ipc_free_resp_buffer(risc0_ipc_resp_t *resp)
{
	return 0;
}

uint32_t risc0_ipc_get_capability(void)
{
	return capability;
}

int risc0_ipc_stop(void)
{
	return 0;
}
