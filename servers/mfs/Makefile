# Makefile for Minix File System (MFS)
TARGETS := mfs
DEFAULT_NR_BUFS= 1024
BS=4096

# programs, flags, etc.
CFLAGS += $(EXTRA_OPTS) $(CPROFILE) -DDEFAULT_NR_BUFS=$(DEFAULT_NR_BUFS)

OBJS-mfs := cache.o device.o link.o \
	mount.o misc.o open.o protect.o read.o \
	stadir.o stats.o table.o time.o utility.o \
	write.o inode.o main.o path.o super.o

# build local binary 
#STACK_HEAP=`expr $(NR_BUFS) \* $(BS) \* 2`

include ../Makefile.inc
