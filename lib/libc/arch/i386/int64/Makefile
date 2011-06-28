# Makefile for lib/arch/i386/int64.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE


SRCS= \
	add64.S \
	add64u.S \
	bsr64.S \
	cmp64.S \
	cv64u.S \
	cvu64.S \
	diff64.S \
	div64.c \
	div64u.S \
	ex64.S \
	make64.S \
	mul64.c \
	mul64u.S \
	sub64.S \
	sub64u.S

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/arch/i386/int64
include $(ROOT)/lib/Makefile.inc
