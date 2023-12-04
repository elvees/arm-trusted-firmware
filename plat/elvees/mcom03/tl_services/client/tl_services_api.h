/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __TL_SERVICES_API_H__
#define __TL_SERVICES_API_H__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <tl_services/tl_services.h>

#define DEFINE_TL_SERVICES_SEND(service) \
	static inline void service##_service_send(const TL_MBOX_SERVICES_cmd_t *cmd, \
						  TL_MBOX_SERVICES_resp_t *resp) \
	{ \
		int err = tl_services_send(cmd, resp); \
		if (err) { \
			ERROR("%s failed to send msg, err = %d\r\n", __func__, err); \
			panic(); \
		} \
	}

#pragma pack(push, 1)
typedef struct {
	uint32_t magic_num;
	uint16_t cmd_len;
	uint16_t shrmem_len;
} tl_services_hdr_t;
#pragma pack(pop)

int tl_services_start(void);
int tl_services_send(const TL_MBOX_SERVICES_cmd_t *cmd, TL_MBOX_SERVICES_resp_t *resp);
TL_MBOX_SERVICES_resp_t *tl_services_alloc_resp_buffer(unsigned int core);
int tl_services_free_resp_buffer(TL_MBOX_SERVICES_resp_t *resp);
uint32_t tl_services_get_capability(void);
int tl_services_stop(void);

#endif /* __TL_SERVICES_API_H__ */
