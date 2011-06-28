# Makefile for File System (FS)
TARGETS := vfs

LIBS += -ltimers -lexec

OBJS-vfs := main.o open.o read.o write.o pipe.o dmap.o \
	path.o device.o mount.o link.o exec.o \
	filedes.o stadir.o protect.o time.o \
	lock.o misc.o utility.o select.o table.o \
	vnode.o vmnt.o request.o fscall.o gcov.o

include ../Makefile.inc
