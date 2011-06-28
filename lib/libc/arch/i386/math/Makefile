# Makefile for lib/arch/i386/misc.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE


SRCS= \
	arch_compare.c \
	arch_round.c   \
	fpu_cw.S       \
	fpu_sw.S       \
	fpu_round.S    \
	fegetround.c   \
	feholdexcept.c \
	fesetround.c

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/arch/i386/math
include $(ROOT)/lib/Makefile.inc
