LIBS += -lgcc -lsys

CFLAGS+=-I$(ROOT)/kernel/arch/$(ARCH)/include
CFLAGS += $(addprefix -D, $(KERNEL_DEFINES))
LDS := $(ROOT)/userapp.lds

CFLAGS += -I$(ROOT)/ -I$(ROOT)/drivers

#
# Should we perform LLVM passes?
#
ifneq ($(LLVM_CPASS),)
DO_LLVM_CPASS := $(LLVM_PASS_DRIVERS)
endif

ifneq ($(LLVM_LPASS),)
DO_LLVM_LPASS := $(LLVM_PASS_DRIVERS)
endif

include $(ROOT)/Makefile.user.inc
