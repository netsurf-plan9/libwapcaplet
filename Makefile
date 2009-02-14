# Simple Makefile for libwapcaplet

LIB := libwapcaplet.a

SRCS := libwapcaplet.c
HDRS := libwapcaplet/libwapcaplet.h

PREFIX ?= /usr/local

TESTSRCS := testmain.c basictests.c memorytests.c

TARGET ?= debug

BUILDDIR := build-$(TARGET)

MKDIR ?= mkdir -p
SED ?= sed
INSTALL ?= install

all: $(BUILDDIR)/$(LIB)

test: $(BUILDDIR)/testrunner
	$(BUILDDIR)/testrunner

install: all
	$(MKDIR) $(PREFIX)/lib/pkgconfig $(PREFIX)/include/libwapcaplet
	$(SED) -e 's#PREFIX#$(PREFIX)#' libwapcaplet.pc.in >libwapcaplet.pc
	$(INSTALL) -m 644 libwapcaplet.pc $(PREFIX)/lib/pkgconfig
	$(INSTALL) -m 644 $(BUILDDIR)/$(LIB) $(PREFIX)/lib/$(LIB)
	for F in $(HDRS); do \
		$(INSTALL) -m 644 include/$$F $(PREFIX)/include/libwapcaplet; \
	done

CFLAGS := -Iinclude -Wall -Werror

ifeq ($(TARGET),debug)
CFLAGS += -O0 -g
else
CFLAGS += -O2 -DNDEBUG
endif


clean:
	rm -fr build-*

$(BUILDDIR)/stamp:
	mkdir -p $(BUILDDIR)
	touch $(BUILDDIR)/stamp

define srcfile
src/$1
endef

define objfile
$(BUILDDIR)/$(1:.c=.o)
endef

define depfile
$(BUILDDIR)/$(1:.c=.d)
endef

DEPS :=
OBJS :=

define _depandbuild

$2: $1 $(BUILDDIR)/stamp
	$(CC) -MMD -MP $($5) -o $2 -c $1

$4 += $2
DEPS += $3

endef

define depandbuild
$(call _depandbuild,$(call srcfile,$1),$(call objfile,$1),$(call depfile,$1),OBJS,CFLAGS)
endef

$(eval $(foreach SOURCE,$(SRCS),$(call depandbuild,$(SOURCE))))

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(BUILDDIR)/$(LIB): $(BUILDDIR)/stamp $(OBJS)
	$(AR) cru $@ $^

define testsrc
test/$1
endef

define depandbuildtest
$(call _depandbuild,$(call testsrc,$1),$(call objfile,test-$1),$(call depfile,test-$1),TOBJS,TESTCFLAGS)
endef

TOBJS :=

TESTCFLAGS := $(CFLAGS) $(shell pkg-config --cflags check)
TESTLDFLAGS := $(LDFLAGS) $(shell pkg-config --libs check)

$(eval $(foreach TESTSRC,$(TESTSRCS),$(call depandbuildtest,$(TESTSRC))))

$(BUILDDIR)/testrunner: $(BUILDDIR)/stamp $(TOBJS) $(BUILDDIR)/$(LIB)
	$(CC) -o $@ $(TOBJS) $(BUILDDIR)/$(LIB) $(TESTLDFLAGS)
