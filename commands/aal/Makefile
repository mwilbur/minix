# Makefile for aal

CFLAGS +=-I. -DAAL -DSTB -DNDEBUG -DDISTRIBUTION -D_POSIX_SOURCE -D_MINIX

TARGETS := aal

OBJS-aal := \
	archiver.o \
	print.o \
	rd.o \
	rd_arhdr.o \
	rd_unsig2.o \
	sprint.o \
	wr_arhdr.o \
	wr_bytes.o \
	wr_int2.o \
	wr_long.o \
	wr_ranlib.o \
	format.o \
	rd_bytes.o \
	system.o \
	write.o \
	long2str.o

include $(ROOT)/commands/Makefile.single.inc
