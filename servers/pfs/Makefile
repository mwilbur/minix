# Makefile for Pipe File System (PFS)
TARGETS = pfs

CFLAGS += $(EXTRA_OPTS) $(CPROFILE)

LIBS += -ldriver

OBJS-pfs = open.o table.o inode.o main.o super.o link.o \
	 buffer.o read.o misc.o utility.o stadir.o \
	 uds.o dev_uds.o

include ../Makefile.inc
