OBJS := $(addsuffix .o,$(basename $(SRCS)))

all: $(addprefix $(BUILD_DIR)/, $(OBJS))

DEPS_PREFIX := $(BUILD_DIR)/

# for pretty printing!
CURDIR:=$(BUILD_DIR)

#
# See if a LLVM compilation pass has to be performed
#
ifneq ($(LLVM_CPASS),)

$(BUILD_DIR)/%.o: TARGET_NAME = $(@:.o=)
$(BUILD_DIR)/%.o: INPUT_NAME  = $(<:.c=)
$(BUILD_DIR)/%.o: %.c
	$(LLVM_COMPILE)

clean: TARGET_DIR = $(BUILD_DIR)
clean::
	$(LLVM_COMPILE_CLEAN)
else

$(BUILD_DIR)/%.o: %.c
	$(XCC) $(C_STD) $(XCFLAGS) $(CFLAGS) -c -o $@ $<

endif

#compiling by ack needs assembler translation from gnu
ifeq ($(COMPILER_TYPE),ack)
$(BUILD_DIR)/%.o: %.s
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.e
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S
	$(ASMCPP) $(XCFLAGS) $(CFLAGS) -o $(<:.S=.tmp) $< 
	$(ASMCONV) -m$(ARCH) $(<:.S=.tmp) $(<:.S=.ack.s)
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $(<:.S=.ack.s)
else
$(BUILD_DIR)/%.o: %.S
	$(XAS) $(XCFLAGS) $(CFLAGS) -c -o $@ $<
endif


clean::
	-rm -f $(BUILD_DIR)/*.[od]
	-rm -f *.ack.s *.tmp

include $(ROOT)/Makefile.deps
