# Makefile for lib/regex.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE


SRCS= \
	regcomp.c \
	regerror.c \
	regexec.c \
	regfree.c

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/regex
include $(ROOT)/lib/Makefile.inc
