# because of the way gnu-like compilers link stuff, we need to force the _start
# symbol as undefined as it is referenced only in the linker script. Gnu-like
# compilation also depends on linker scripts

include $(ROOT)/Makefile.userlibs.inc

all build: $(TARGETS)
.SECONDEXPANSION:

# When building on Minix aout is the default aoutput format and we need not do
# an extra step from elf to aout. This difference will go in the future
ifeq ($(PLAT), Minix)
ALL_OBJS := $(foreach target, $(TARGETS), $(OBJS-$(target)))
clean::
	-rm -f $(ALL_OBJS)
$(TARGETS): $(LIB_FILES) $(ALL_OBJS) $(LDS)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@ $(OBJS-$@) $(LIBSOPT)
ifneq ($(COMPILER_TYPE),ack)
	install -S 128k $@
endif
else

$(TARGETS): %: %.elf
	$(AOUT)
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

%.elf: TARGET_NAME = $(@:.elf=)
%.elf: BCCS =       $(foreach file, $(OBJS-$(TARGET_NAME):.o=), $(if $(wildcard $(file).[sS]),,$(file).bcc))
%.elf: OBJS-NOBCC = $(foreach file, $(OBJS-$(TARGET_NAME):.o=), $(if $(wildcard $(file).[sS]),$(file).o,))
%.elf: $(LIB_FILES) $$(OBJS-$$(TARGET_NAME)) $(LDS) $(BCCS)
	$(LLVM_LINK)

clean:: TARGET_DIR = .
clean::
	$(LLVM_LINK_CLEAN)
else

%.elf: TARGET_NAME = $(@:.elf=)
%.elf: $(LIB_FILES) $$(OBJS-$$(TARGET_NAME)) $(LDS)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@ $(OBJS-$(TARGET_NAME)) $(LIBSOPT)

endif

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

clean::
	-rm -f *.o *.bak $(TARGETS) *.ahdr *.raw *.elf

#
# Support for subdirectories
#
ifneq ($(BUILD_SUBDIRS),)

BUILD_SUBTARGETS = $(addsuffix /%.o, $(BUILD_SUBDIRS))

$(BUILD_SUBTARGETS):
	$(MAKE) -C $(dir $@) $(notdir $@)

clean::
	$(foreach DIR, $(BUILD_SUBDIRS), $(MAKE) -C $(DIR) clean;)

endif

include $(ROOT)/Makefile.deps
