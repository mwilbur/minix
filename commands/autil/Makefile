# Makefile for commands/autil

CFLAGS += -I. -D_MINIX -D_POSIX_SOURCE

TARGETS := anm asize

OBJS-anm := anm.o rd.o rd_arhdr.o rd_bytes.o rd_unsig2.o

OBJS-asize := asize.o

include $(ROOT)/commands/Makefile.single.inc
