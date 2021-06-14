#*****************************************************************************
#
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

#*****************************************************************************
HOSTCC ?= cc
CFLAGS ?= -Werror -Wall -Wextra
LDFLAGS ?=

amdsigtool_exe : amdsigtool.c
	$(HOSTCC) $(CFLAGS) amdsigtool.c -o amdsigtool $(LDFLAGS) -lz

amdsigtool : amdsigtool_exe

clean:
	@rm -f amdsigtool.o amdsigtool
