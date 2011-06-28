# Makefile for the process status utility.
#
CFLAGS += -O -D_MINIX -D_POSIX_SOURCE
CFLAGS += -I$(ROOT)/kernel/arch/$(ARCH)/include

CFLAGS += -I$(ROOT)/include/arch

PROGRAM := ps

OBJS := ps.o

include $(ROOT)/commands/Makefile.single.inc
