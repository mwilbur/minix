# How to build it

CFLAGS += $(addprefix -D, $(KERNEL_DEFINES))
CFLAGS += -I$(KERNEL_ARCH_INCLUDES)

ifeq ($(CC), gcc)
CFLAGS += -O
CFLAGS += -Wuninitialized
CFLAGS += -Wunused
CFLAGS += -Warray-bounds
CFLAGS += -Wtype-limits
CFLAGS += -Wreturn-type
CFLAGS += -Wimplicit-int
CFLAGS += -Wsequence-point
endif

CFLAGS += -D__kernel__

ifeq ($(COMPILER_TYPE), ack)
%.o: %.S
	$(ASMCPP) $(XCFLAGS) $(CFLAGS) -o $(<:.S=.tmp) $< 
	$(ASMCONV) -m$(ARCH) $(<:.S=.tmp) $(<:.S=.ack.s)
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $(<:.S=.ack.s)

clean::
	-rm -f *.tmp *.ack.s
else
%.o: %.S
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $<
endif	

#
# Should we perform LLVM passes?
#
ifneq ($(LLVM_CPASS),)
DO_LLVM_CPASS := $(LLVM_PASS_KERNEL)
endif

ifneq ($(LLVM_LPASS),)
DO_LLVM_LPASS := $(LLVM_PASS_KERNEL)
endif

#
# See if a LLVM compilation pass has to be performed
#
ifeq ($(DO_LLVM_CPASS),1)

%.o: TARGET_NAME = $(@:.o=)
%.o: INPUT_NAME  = $(<:.c=)
%.o: %.c
	$(LLVM_COMPILE)

clean: TARGET_DIR = .
clean::
	$(LLVM_COMPILE_CLEAN)
else

%.o: %.c
	$(XCC) $(C_STD) $(XCFLAGS) $(CFLAGS) -c -o $@ $<

endif

#
# See if a LLVM linking pass has to be performed
#
ifeq ($(DO_LLVM_LPASS),1)

clean:: TARGET_DIR = .
clean::
	$(LLVM_LINK_CLEAN)

endif

