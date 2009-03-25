# Component settings
COMPONENT := wapcaplet
# Default to a static library
COMPONENT_TYPE ?= lib-static

# Setup the tooling
include build/makefiles/Makefile.tools

# Reevaluate when used, as BUILDDIR won't be defined yet
TESTRUNNER = $(BUILDDIR)/test_testrunner$(EXEEXT)

# Toolchain flags
WARNFLAGS := -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes \
	-Wmissing-declarations -Wnested-externs -Werror -pedantic
CFLAGS := $(CFLAGS) -std=c99 -D_BSD_SOURCE -I$(CURDIR)/include/ \
	-I$(CURDIR)/src $(WARNFLAGS) 

include build/makefiles/Makefile.top

ifeq ($(WANT_TEST),yes)
  ifneq ($(PKGCONFIG),)
    TESTCFLAGS := $(TESTCFLAGS) $(shell $(PKGCONFIG) --cflags check)
    TESTLDFLAGS := $(TESTLDFLAGS) $(shell $(PKGCONFIG) --libs check)
  else
    TESTLDFLAGS := $(TESTLDFLAGS) -lcheck
  endif
endif

# Extra installation rules
INSTALL_ITEMS := $(INSTALL_ITEMS) /include/libwapcaplet:include/libwapcaplet/libwapcaplet.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib/pkgconfig:lib$(COMPONENT).pc.in
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib:$(BUILDDIR)/lib$(COMPONENT)$(LIBEXT)
