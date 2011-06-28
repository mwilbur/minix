# Makefile for memory driver (MEMORY)
TARGETS := memory

# programs, flags, etc.
LIBS :=  -ldriver

# imgrd_s.s is the ACK assembler version of the ramdisk. For more portability,
# use the C version imgrd.c. However, the C compiler takes too much memory
# compiling imgrd.c.
IMGRD=imgrd.o

OBJS-memory := memory.o

RAMDISK_FILES = \
	../at_wini/at_wini			\
	../bios_wini/bios_wini			\
	../floppy/floppy			\
	../pci/pci				\
	../../commands/simple/cdprobe		\
	../../commands/simple/dev2name		\
	../../commands/simple/loadramdisk	\
	../../commands/simple/newroot		\
	../../commands/simple/sysenv		\
	../../commands/ash/sh			\
	../../commands/service/service		\
	../../servers/mfs/mfs

include $(ROOT)/drivers/Makefile.inc

$(RAMDISK_FILES):
	$(error "Broken dependecies?")

#
# See if a LLVM compilation pass has to be performed
#
ifeq ($(DO_LLVM_CPASS),1)

$(IMGRD): TARGET_NAME = $(@:.o=)
$(IMGRD): INPUT_NAME  = $(@:.o=)
$(IMGRD): $(RAMDISK_FILES) ramdisk/proto.gen
	$(MAKE) -C ramdisk
	$(LLVM_COMPILE)

else

$(IMGRD): $(RAMDISK_FILES) ramdisk/proto.gen
	$(MAKE) -C ramdisk
	$(XCC) $(XCFLAGS) $(CFLAGS) -c -o $@ $(@:.o=.c)

endif

OBJS-memory += $(IMGRD)

ifeq ($(PLAT), Minix)
$(TARGETS): $(IMGRD)
else
$(TARGETS).elf: $(IMGRD)
endif

clean::
	$(MAKE) -C ramdisk clean

