/*
 * Copyright 2019-2023 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __TL_SERVICES_H__
#define __TL_SERVICES_H__

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define TL_MBOX_SERVICES_MAGIC  (0x4d424f58) // "MBOX" in ASCII

typedef enum {
	TL_MBOX_SERVICES_INIT = 0x00U,
	TL_MBOX_SERVICES_TL_PROTO = 0x01U,
	TL_MBOX_SERVICES_WDT = 0x02U,
	TL_MBOX_SERVICES_DDR_SUBS = 0x05U,
	TL_MBOX_SERVICES_COUNT,
} TL_MBOX_SERVICES_t;

typedef enum {
	TL_MBOX_SERVICES_INIT_FUNC_GET_CAPABILITY = 0x00U,
	TL_MBOX_SERVICES_INIT_FUNC_COUNT,
} TL_MBOX_SERVICES_INIT_FUNC_t;

typedef enum {
	TL_MBOX_SERVICES_TL_PROTO_FUNC_STOP = 0x00U,
	TL_MBOX_SERVICES_TL_PROTO_FUNC_START = 0x01U,
	TL_MBOX_SERVICES_TL_PROTO_FUNC_COUNT,
} TL_MBOX_SERVICES_TL_PROTO_FUNC_t;

typedef enum {
	TL_MBOX_SERVICES_WDT_FUNC_IS_ENABLE = 0x01U,
	TL_MBOX_SERVICES_WDT_FUNC_START = 0x02U,
	TL_MBOX_SERVICES_WDT_FUNC_PING = 0x03U,
	TL_MBOX_SERVICES_WDT_FUNC_SET_TIMEOUT_S = 0x04U,
	TL_MBOX_SERVICES_WDT_FUNC_GET_TIMEOUT_S = 0x05U,
	TL_MBOX_SERVICES_WDT_FUNC_GET_MAX_TIMEOUT_S = 0x06U,
	TL_MBOX_SERVICES_WDT_FUNC_GET_MIN_TIMEOUT_S = 0x07U,
	TL_MBOX_SERVICES_WDT_FUNC_COUNT,
} TL_MBOX_SERVICES_WDT_FUNC_t;

typedef enum {
	TL_MBOX_SERVICES_DDR_SUBS_FUNC_SET_HSPERIPH_BAR = 0x01U,
	TL_MBOX_SERVICES_DDR_SUBS_FUNC_SET_LSPERIPH0_BAR = 0x02U,
	TL_MBOX_SERVICES_DDR_SUBS_FUNC_SET_LSPERIPH1_BAR = 0x03U,
	TL_MBOX_SERVICES_DDR_SUBS_FUNC_SET_GPU_BAR = 0x04U,
	TL_MBOX_SERVICES_DDR_SUBS_FUNC_COUNT,
} TL_MBOX_SERVICES_DDR_SUBS_FUNC_t;

typedef enum {
	TL_MBOX_SERVICES_RESP_STATE_BUSY = 0x00U,
	TL_MBOX_SERVICES_RESP_STATE_COMPLETE = 0x01U,
	TL_MBOX_SERVICES_RESP_STATE_COUNT,
} TL_MBOX_SERVICES_RESP_STATE_t;

#pragma pack(push, 1)
/* Command params */
typedef struct {
	uint64_t shrmem;
} TL_MBOX_SERVICES_TL_PROTO_start_t;

typedef struct {
	uint32_t timeout;
} TL_MBOX_SERVICES_WDT_start_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_WDT_set_timeout_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_DDR_SUBS_set_bar_t;

typedef union {
	union {
		TL_MBOX_SERVICES_TL_PROTO_start_t start;
	} tlProto;
	union {
		TL_MBOX_SERVICES_WDT_start_t start;
		TL_MBOX_SERVICES_WDT_set_timeout_t setTimeout;
	} wdt;
	union {
		TL_MBOX_SERVICES_DDR_SUBS_set_bar_t bar;
	} ddr_subs;
} TL_MBOX_SERVICES_cmd_param_t;

/* Response params */
typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_INIT_get_capability_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_WDT_is_enable_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_WDT_get_timeout_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_WDT_get_max_timeout_t;

typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_WDT_get_min_timeout_t;

typedef union {
	union {
		TL_MBOX_SERVICES_INIT_get_capability_t capability;
	} init;
	union {
		TL_MBOX_SERVICES_WDT_is_enable_t isEnable;
		TL_MBOX_SERVICES_WDT_get_timeout_t timeout;
		TL_MBOX_SERVICES_WDT_get_max_timeout_t minTimeout;
		TL_MBOX_SERVICES_WDT_get_min_timeout_t maxTimeout;
	} wdt;
} TL_MBOX_SERVICES_resp_param_t;

/* Command message */
typedef struct {
	uint16_t service;
	uint16_t func;
} TL_MBOX_SERVICES_hdr_t;

typedef struct {
	TL_MBOX_SERVICES_hdr_t hdr;
	TL_MBOX_SERVICES_cmd_param_t param;
} TL_MBOX_SERVICES_cmd_t;

/* Response message */
typedef struct {
	uint32_t value;
} TL_MBOX_SERVICES_resp_state_t;

typedef struct {
	TL_MBOX_SERVICES_resp_state_t state;
	TL_MBOX_SERVICES_resp_param_t param;
} TL_MBOX_SERVICES_resp_t;

typedef struct {
	uint64_t data;
} TL_MBOX_SERVICES_shrmem_t;
#pragma pack(pop)

#endif /* __TL_SERVICES_H__ */

