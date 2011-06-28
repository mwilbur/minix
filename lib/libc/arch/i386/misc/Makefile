# Makefile for lib/arch/i386/misc.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE


SRCS= \
	_cpuid.S \
	_cpufeature.c \
	alloca.S \
	get_bp.S \
	getprocessor.S \
	oneC_sum.S

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/arch/i386/misc
include $(ROOT)/lib/Makefile.inc
