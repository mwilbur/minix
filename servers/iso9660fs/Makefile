# Makefile for ISO9660 fs
TARGETS := isofs
NR_BUFS = 100

CFLAGS += $(EXTRA_OPTS) $(CPROFILE) -DNR_BUFS=$(NR_BUFS)
LIBS = -lsys -ltimers

OBJS-isofs := main.o table.o mount.o super.o inode.o device.o \
	utility.o misc.o path.o read.o stadir.o cache.o

include ../Makefile.inc
