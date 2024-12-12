/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __API_H__
#define __API_H__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include "protocol.h"

#define DEFINE_RISC0_IPC_SEND(service) \
	static inline void service##_service_send(const risc0_ipc_cmd_t *cmd, \
						  risc0_ipc_resp_t *resp) \
	{ \
		int err = risc0_ipc_send(cmd, resp); \
		if (err) { \
			ERROR("%s failed to send msg, err = %d\r\n", __func__, err); \
			panic(); \
		} \
	}

int risc0_ipc_start(void);
int risc0_ipc_send(const risc0_ipc_cmd_t *cmd, risc0_ipc_resp_t *resp);
risc0_ipc_resp_t *risc0_ipc_alloc_resp_buffer(unsigned int core);
int risc0_ipc_free_resp_buffer(risc0_ipc_resp_t *resp);
uint32_t risc0_ipc_get_capability(void);
int risc0_ipc_stop(void);

#endif /* __API_H__ */
