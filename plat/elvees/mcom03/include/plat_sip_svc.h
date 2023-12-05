/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAT_SIP_SVC_H
#define PLAT_SIP_SVC_H

/* SMC function IDs for SiP Service queries */
#define MCOM03_SIP_SVC_CALL_COUNT		0x8200FF00
#define MCOM03_SIP_SVC_UID			0x8200FF01
#define MCOM03_SIP_SVC_VERSION			0x8200FF03

/* MCom-03 SiP Service Calls version numbers */
#define MCOM03_SIP_SVC_VERSION_MAJOR		0x0
#define MCOM03_SIP_SVC_VERSION_MINOR		0x2

/* Number of MCom-03 SiP Calls implemented */
#define MCOM03_COMMON_SIP_NUM_CALLS		2

/* MCom-03 WDT SiP Service queries */
#define MCOM03_SIP_WDT				0xC2000001
#define MCOM03_SIP_WDT_IS_ENABLE		0x01
#define MCOM03_SIP_WDT_START			0x02
#define MCOM03_SIP_WDT_PING			0x03
#define MCOM03_SIP_WDT_SET_TIMEOUT_S		0x04
#define MCOM03_SIP_WDT_GET_TIMEOUT_S		0x05
#define MCOM03_SIP_WDT_GET_MAX_TIMEOUT_S	0x06
#define MCOM03_SIP_WDT_GET_MIN_TIMEOUT_S	0x07

#define MCOM03_SIP_DDR_SUBS			0xC2000004
#define MCOM03_SIP_DDR_SUBS_SET_HSPERIPH_BAR	0x01
#define MCOM03_SIP_DDR_SUBS_SET_LSPERIPH0_BAR	0x02
#define MCOM03_SIP_DDR_SUBS_SET_LSPERIPH1_BAR	0x03
#define MCOM03_SIP_DDR_SUBS_SET_GPU_BAR		0x04

uint64_t mcom03_sip_wdt_handler(uint64_t id, uint64_t param,
				uint64_t arg0, uint64_t arg1);

uint64_t mcom03_sip_ddr_subs_handler(uint64_t id, uint64_t param,
				     uint64_t arg0, uint64_t arg1);

#endif /* PLAT_SIP_SVC_H */
