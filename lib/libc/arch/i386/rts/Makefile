# Makefile for lib/arch/i386/rts.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE


SRCS= \
	__sigreturn.S \
	_ipc.S \
	_senda.S \
	brksize.S\
	crtso.S
	
START_FILES=\
	crtso.S \
	m2rtso.S \
	prtso.S

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/arch/i386/rts
include $(ROOT)/lib/Makefile.inc
