#
# Copyright 2022 RnD Center "ELVEES", JSC
#
# SPDX-License-Identifier: BSD-3-Clause

# Include GICv3 driver files
include drivers/arm/gic/v3/gicv3.mk

# TSP source files specific to MCOM03 platform
BL32_SOURCES	+=	$(GICV3_SOURCES)			\
			plat/common/aarch64/platform_mp_stack.S	\
			plat/elvees/mcom03/tsp/tsp_plat_setup.c	\
			plat/common/plat_gicv3.c		\
			plat/elvees/mcom03/gicv3.c		\
			plat/elvees/mcom03/psci.c		\
			plat/elvees/mcom03/topology.c		\
			plat/elvees/mcom03/xlat_setup.c

ifneq (${ENABLE_STACK_PROTECTOR}, 0)
	BL32_SOURCES	+=	plat/elvees/mcom03/mcom03_stack_protector.c
endif

