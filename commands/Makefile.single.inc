TARGET := $(PROGRAM)
LDS := $(ROOT)/userapp.lds

#
# Ignore LLVM passes for commands
#
DO_LLVM_CPASS := 0
DO_LLVM_LPASS := 0

include $(ROOT)/Makefile.user.inc

