# Makefile for the init program (INIT)
TARGETS := init

# programs, flags, etc.
CFLAGS += -O -D_MINIX -D_POSIX_SOURCE
OBJS-init := init.o

#
# Should we perform LLVM passes?
#
ifneq ($(LLVM_CPASS),)
DO_LLVM_CPASS := $(LLVM_PASS_INIT)
endif

ifneq ($(LLVM_LPASS),)
DO_LLVM_LPASS := $(LLVM_PASS_INIT)
endif

include ../Makefile.inc
