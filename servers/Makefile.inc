LIBS += -lsys

CFLAGS+=-I../../kernel/arch/$(ARCH)/include -I$(ROOT)
CFLAGS += $(addprefix -D, $(KERNEL_DEFINES))
LDS := $(ROOT)/userapp.lds

CFLAGS += -D_POSIX_SOURCE

#
# Should we perform LLVM passes?
#
ifneq ($(LLVM_CPASS),)
ifneq ($(DO_LLVM_CPASS),0)
DO_LLVM_CPASS := $(LLVM_PASS_SERVERS)
endif
endif

ifneq ($(LLVM_LPASS),)
ifneq ($(DO_LLVM_LPASS),0)
DO_LLVM_LPASS := $(LLVM_PASS_SERVERS)
endif
endif

include $(ROOT)/Makefile.user.inc
