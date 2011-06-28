# Makefile for Reincarnation Server (RS)
TARGETS := service

service:LIBS := -lc

OBJS-service := service.o

#
# Ignore LLVM passes for service
#
DO_LLVM_CPASS := 0
DO_LLVM_LPASS := 0

include $(ROOT)/servers/Makefile.inc
