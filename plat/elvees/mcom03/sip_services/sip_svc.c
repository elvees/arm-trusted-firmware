/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <assert.h>

#include <common/runtime_svc.h>
#include <tools_share/uuid.h>

#include <plat_sip_svc.h>

/* MCom-03 SiP Service UUID */
DEFINE_SVC_UUID2(mcom03_sip_svc_uid,
		 0x38d28ea4, 0x5154, 0x11ed, 0xbd, 0xc3,
		 0x02, 0x42, 0xac, 0x12, 0x00, 0x02);

uintptr_t mcom03_sip_handler(uint32_t smc_fid,
			     u_register_t x1,
			     u_register_t x2,
			     u_register_t x3,
			     u_register_t x4,
			     void *cookie,
			     void *handle,
			     u_register_t flags)
{

	switch (smc_fid) {
	case MCOM03_SIP_WDT:
		SMC_RET1(handle, mcom03_sip_wdt_handler(x1, x2, x3, x4));
	default:
		ERROR("%s: unhandled SMC (0x%x)\n", __func__, smc_fid);
		SMC_RET1(handle, SMC_UNK);
	}
}

/*
 * This function is responsible for handling all SiP calls from the NS world
 */
uintptr_t mcom03_smc_handler(uint32_t smc_fid,
			     u_register_t x1,
			     u_register_t x2,
			     u_register_t x3,
			     u_register_t x4,
			     void *cookie,
			     void *handle,
			     u_register_t flags)
{
	uint32_t ns;

	/* Determine which security state this SMC originated from */
	ns = is_caller_non_secure(flags);
	if (!ns)
		SMC_RET1(handle, SMC_UNK);

	switch (smc_fid) {
	case MCOM03_SIP_SVC_CALL_COUNT:
		/* Return the number of MCom-03 SiP Service Calls. */
		SMC_RET1(handle, MCOM03_COMMON_SIP_NUM_CALLS);
	case MCOM03_SIP_SVC_UID:
		/* Return UID to the caller */
		SMC_UUID_RET(handle, mcom03_sip_svc_uid);
	case MCOM03_SIP_SVC_VERSION:
		/* Return the version of current implementation */
		SMC_RET2(handle, MCOM03_SIP_SVC_VERSION_MAJOR,
			 MCOM03_SIP_SVC_VERSION_MINOR);
	default:
		return mcom03_sip_handler(smc_fid, x1, x2, x3, x4,
					  cookie, handle, flags);
	}
}

/* Define a runtime service descriptor for fast SMC calls */
DECLARE_RT_SVC(
	mcom03_sip_svc,
	OEN_SIP_START,
	OEN_SIP_END,
	SMC_TYPE_FAST,
	NULL,
	mcom03_smc_handler
);
