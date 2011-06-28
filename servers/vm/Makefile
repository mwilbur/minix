# Makefile for VM server
TARGETS = vm

CFLAGS := -Iarch/$(ARCH)/ -I$(ROOT)/servers/vm $(CFLAGS)

BUILD_SUBDIRS = arch/$(ARCH)

OBJS-vm := main.o alloc.o utility.o exec.o exit.o fork.o break.o \
	signal.o mmap.o slaballoc.o region.o pagefaults.o addravl.o \
	physravl.o rs.o queryexit.o yieldedavl.o regionavl.o
ARCHOBJS :=  arch/$(ARCH)/vm.o arch/$(ARCH)/pagetable.o

OBJS-vm += $(ARCHOBJS)

include ../Makefile.inc

