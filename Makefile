# Configuration options
#
# ARCH 			what is the target architecture
# CC			what compiler to use
# BOOT_IMAGE_TYPE 	what is the target format of the boot image elf or aout
# 			(Minix aout)
# SERIAL_CONSOLE	clone/only whether the output should be coloned or only
# 			sent to the serial console for debugging


PLAT = $(shell uname)
ROOT := $(PWD)

#compiling for i386 by default
ifeq ($(ARCH),)
ARCH=i386
endif

# global settings
include kernel/arch/$(ARCH)/Makefile.inc

#configuration
include Makefile.conf

# Directories
INCLUDES = $(ROOT)/include

#architecture dependent includes
ARCH_LINK := include/machine
ARCHINC := $(ROOT)/include/arch/$(ARCH)

# Programs, flags, etc.
CFLAGS += -D__minix
CFLAGS += -I$(INCLUDES)

GNU_FLAGS := \
		-fno-builtin \
		-nostdinc \
		-ffreestanding \
		-fno-stack-protector

# Platform specific settings
ifeq ($(PLAT), Minix)
# default compiler on Minix is ACK
BOOT_IMAGE_TYPE:=aout
ifeq ($(CC),cc)
# gnu make assumes separate drivers for linker and assembler
COMPILER_TYPE:=ack
LD:=cc -.o
CFLAGS += -w -wo $(CPROFILE) $(CPPFLAGS) $(EXTRA_OPTS)
LDFLAGS += -L$(ROOT)/lib/obj-ack
LDFLAGS += -i
COMPILER := ack
else
COMPILER_TYPE:=gnu
LD:=gld
AR:=gar
LDFLAGS += -L$(ROOT)/lib/obj-gnu
LDFLAGS += -m$(ARCH)minix
LDFLAGS += -nostdlib
CFLAGS += $(GNU_FLAGS) 
endif

MKFS := mkfs

else
#any other system than Minix

ifeq ($(BOOT_IMAGE_TYPE),)
BOOT_IMAGE_TYPE:=aout
endif
COMPILER_TYPE:=gnu
CFLAGS += $(GNU_FLAGS) 
LDFLAGS += -L$(ROOT)/lib/obj-gnu

ifeq ($(ARCH), i386)
CFLAGS += -m32
LLCFLAGS += -march=x86 -mattr=-sse2
ifeq ($(PLAT), FreeBSD)
LDFLAGS += -melf_$(ARCH)_fbsd
else
LDFLAGS += -melf_$(ARCH)
endif
endif

# Assume that the default compiler on other systems is gcc for now
ifneq ($(PLAT), Minix)
ifeq ($(CC),cc)
COMPILER := gcc
endif

# If a different version of gcc (e.g. gcc-4.3) is passed as CC, 
ifeq ($(CC),$(findstring gcc, $(CC)))
COMPILER := gcc
endif
endif


ifeq ($(CC),icc)
COMPILER := icc
LD := icc
LDFLAGS += -ffreestanding
endif

E2A := $(ROOT)/tools/ehdr2ahdr/ehdr2ahdr


AOUT := \
	$$(E2A) $$(E2AFLAGS) -c $$(ARCH) -s $$(if $$(STACK_SIZE-$$(@)),$$(STACK_SIZE-$$(@)),128k) -i $$< -o $$@.ahdr ;\
	objcopy -O binary -S $$< $$@.raw ;\
	cat $$@.ahdr $$@.raw > $$@

MKFS_NATIVE := mkfs.native
export MKFS_NATIVE
MKFS := $(ROOT)/drivers/memory/ramdisk/$(MKFS_NATIVE)

export E2A
endif

AS := $(CC)

AS := $(AS) -D__ASSEMBLY__

# LLVM tools
LLC:=llc
LLLD:=llvm-ld
LLAR:=llvm-ar
LLP:=opt

# LLVM flags.
# -disable-internalize is required for incremental linking
# -disable-opt is to postpone LTO after user-specified linking passes
LLLDFLAGS += -L$(ROOT)/lib/obj-$(COMPILER_TYPE)/bca -disable-internalize -disable-opt
LLPFLAGS += -disable-internalize

# LLVM passes
ifneq ($(LLVM_PASS),)
LLVM_CPASS := $(LLVM_PASS)
LLVM_LPASS := $(LLVM_PASS)
endif

# If only a LLVM linking pass is used, we force a dummy compile pass to
# generate bitcode files from .c files correctly
ifneq ($(LLVM_LPASS),)
ifeq ($(LLVM_CPASS),)
LLVM_CPASS := -disable-opt
endif
endif
#... and viceversa
ifneq ($(LLVM_CPASS),)
ifeq ($(LLVM_LPASS),)
LLVM_LPASS := -disable-opt
endif
endif

# LLVM pass defaults
ifeq ($(LLVM_LTO),)
LLVM_LTO := 1 #standard LTO is enabled by default
endif
ifndef ($(LLVM_PASS_DRIVERS),)
LLVM_PASS_DRIVERS := 1
endif
ifndef ($(LLVM_PASS_SERVERS),)
LLVM_PASS_SERVERS := 1
endif
ifndef ($(LLVM_PASS_INIT),)
LLVM_PASS_INIT := 0
endif
ifndef ($(LLVM_PASS_KERNEL),)
LLVM_PASS_KERNEL := 0 #FIXME: only compile pass works with kernel now.
endif

# LLVM debug
LLVM_DEBUG_OUT_FLAGS := 1>/dev/null

ifeq ($(LLVM_DEBUG),1) #print verbose output
LLLDFLAGS += -v
LLARFLAGS += v
LLPFLAGS += -debug
LLVM_DEBUG_OUT_FLAGS :=
endif

ifeq ($(LLVM_DEBUG),2) #generate unoptimized and easy-to-read code
ifneq ($(CC),clang) #clang doesn't like -O
CFLAGS += -O0
endif
LDFLAGS += -O0
LLLDFLAGS += -v
LLVM_LTO := 0
LLARFLAGS += v
LLPFLAGS += -debug
LLCFLAGS += -O0
LLVM_DEBUG_OUT_FLAGS :=
endif

ifeq ($(LLVM_DEBUG),3) #generate highly optimized code
ifneq ($(CC),clang) #clang doesn't like -O
CFLAGS += -O3
endif
LDFLAGS += -O3
LLLDFLAGS += -v
LLVM_LTO := 2
LLARFLAGS += v
LLCFLAGS += -O3
LLVM_DEBUG_OUT_FLAGS :=
endif

#LLVM link-time optimizations (LTO)
ifeq ($(LLVM_LTO),1)
LLVM_LPASS += -std-link-opts #standard LTO
endif
ifeq ($(LLVM_LTO),2)
LLVM_LPASS += -O3 #advanced LTO
endif

#LLVM macros
LLVM_COMPILE := \
        $$(subst @echo,echo $(LLVM_DEBUG_OUT_FLAGS), \
	$$(XCC) $$(C_STD) $$(XCFLAGS) $$(CFLAGS) -c -emit-llvm -o $$(TARGET_NAME).BCC $$(INPUT_NAME).c ;\
	$$(XLLP) $$(LLPFLAGS) $$(LLVM_CPASS) -o $$(TARGET_NAME).bcc $$(TARGET_NAME).BCC 2>&1 | grep -v "defined more than once" ;\
	$$(XLLC) $$(LLCFLAGS) -filetype=asm -o $$(TARGET_NAME).bccs.s $$(TARGET_NAME).BCC ;\
	$$(XAS) $$(XCFLAGS) $$(CFLAGS) -c -o $$(TARGET_NAME).o $$(TARGET_NAME).bccs.s )

LLVM_COMPILE_CLEAN := \
	-rm -f $$(TARGET_DIR)/*.BCC $$(TARGET_DIR)/*.bcc $$(TARGET_DIR)/*.bcc*.*

LLVM_LINK := \
        $$(subst @echo,echo $(LLVM_DEBUG_OUT_FLAGS), \
	$$(XLLLD) $$(LLLDFLAGS) -b $$(TARGET_NAME).BCL -o $$(TARGET_NAME).BCL.sh $$(BCCS) $$(LLLIBSOPT) ;\
	$$(XLLP) $$(LLPFLAGS) $$(LLVM_LPASS) -o $$(TARGET_NAME).bcl $$(TARGET_NAME).BCL 2>&1 | grep -v "defined more than once" ;\
	$$(XLLC) $$(LLCFLAGS) -filetype=asm -o $$(TARGET_NAME).bcls.s $$(TARGET_NAME).bcl ;\
	$$(XAS) $$(XCFLAGS) $$(CFLAGS) -c -o $$(TARGET_NAME).bclo.o $$(TARGET_NAME).bcls.s ;\
	$$(XLD) $$(XLDFLAGS) $$(LDFLAGS) -o $$@ $$(TARGET_NAME).bclo.o $$(OBJS-NOBCC) $$(LIBSOPT) )

LLVM_LINK_CLEAN := \
	-rm -f $$(TARGET_DIR)/*.BCL $$(TARGET_DIR)/*.BCL.* $$(TARGET_DIR)/*.bcl $$(TARGET_DIR)/*.bcl*.*

# Clang requires some additional flags to work correctly
ifeq ($(findstring clang, $(CC)),clang)
CFLAGS += -Wreturn-type -Qunused-arguments
endif

ifneq ($(CROSS_COMPILE),)
# in case of cross-compilation. prefix the utilities
XCC := $(CROSS_COMPILE)$(CC)
XLD := $(CROSS_COMPILE)$(LD)
XAS := $(XCC)
XAR := $(CROSS_COMPILE)$(AR)
XLLC := $(CROSS_COMPILE)$(LLC)
XLLLD := $(CROSS_COMPILE)$(LLLD)
XLLAR := $(CROSS_COMPILE)$(LLAR)
XLLP := $(CROSS_COMPILE)$(LLP)
else
# in case of native compilation use the same utilities for compilation as well
# as for cross-compilation which makes cross-compilation a native compilation
XCC := $(CC)
XLD := $(LD)
XAS := $(AS)
XAR := $(AR)
XLLC:= $(LLC)
XLLLD:= $(LLLD)
XLLAR:= $(LLAR)
XLLP:= $(LLP)
endif

#dependencies for gcc and similar compilers
ifeq ($(COMPILER_TYPE),gnu)
DEPCMD:=$(XCC) -M $$(XCFLAGS) $$(CFLAGS)
endif

#dependencies for ack
ifeq ($(COMPILER_TYPE),ack)
# this is never a cross-compilation
DEPCMD:=mkdep '$(CC) -E $$(XCFLAGS) $$(CFLAGS)'
endif

SH := sh

# this tools are never used during cross-compilation, they are only used when
# compiling with ack and that is always native
ASMCPP := $(CC) -E -D__ASSEMBLY__
ASMCONV := gas2ack

MKDIR = mkdir -p

SYMLINKCMD := ln -sf
MKDIRCMD := mkdir -p

ifeq ($V,1)
SYMLINK = $(SYMLINKCMD) $(1) $(2)
MKDIR = $(MKDIRCMD) $(1)

else
MAKE := @$(MAKE) --no-print-directory --silent

ECHO_FILE_NAME := $$(subst $(ROOT)/,,$$(CURDIR)/$$(notdir $$@))

echo_cmd = @echo   "  $(1)$(ECHO_FILE_NAME)" ; $(2)

CC := 		$(call echo_cmd,CC      ,$(CC))
LD := 		$(call echo_cmd,LD      ,$(LD))
AS := 		$(call echo_cmd,AS      ,$(AS))
AOUT := 	$(call echo_cmd,AOUT    ,$(AOUT))
AR := 		$(call echo_cmd,AR      ,$(AR))
LLC := 		$(call echo_cmd,LLC     ,$(LLC))
LLLD :=		$(call echo_cmd,LLLD    ,$(LLLD))
LLAR :=		$(call echo_cmd,LLAR    ,$(LLAR))
LLP := 		$(call echo_cmd,LLP     ,$(LLP))
SH := 		$(call echo_cmd,SH      ,$(SH))
ASMCONV := 	$(call echo_cmd,ASMCONV ,$(ASMCONV))
ASMCPP :=	$(call echo_cmd,ASMCPP  ,$(ASMCPP))
MKFS :=		$(call echo_cmd,MKFS    ,$(MKFS))
LLVM_COMPILE := $(call echo_cmd,LLVM_CC ,$(LLVM_COMPILE))
LLVM_LINK    := $(call echo_cmd,LLVM_LD ,$(LLVM_LINK))

ifneq ($(CROSS_COMPILE),)
XCC := 		$(call echo_cmd,XCC     ,$(XCC))
XLD := 		$(call echo_cmd,XLD     ,$(XLD))
XAS := 		$(call echo_cmd,XAS     ,$(XAS))
XAR := 		$(call echo_cmd,XAR     ,$(XAR))
XLLC := 	$(call echo_cmd,XLLC    ,$(XLLC))
XLLLD := 	$(call echo_cmd,XLLLD   ,$(XLLLD))
XLLAR := 	$(call echo_cmd,XLLAR   ,$(XLLAR))
XLLP := 	$(call echo_cmd,XLLP    ,$(XLLP))
else
XCC := 		$(CC)
XLD := 		$(LD)
XAS := 		$(AS)
XLLC :=		$(LLC)
XLLLD :=	$(LLLD)
XLLAR :=	$(LLAR)
XLLP :=		$(LLP)
endif

SYMLINK =	@echo   "  SYMLINK $(subst $(ROOT)/,,$(2)) -> $(subst $(ROOT)/,,$(1))" ; $(SYMLINKCMD) $(1) $(2)
MKDIR = 	@echo   "  MKDIR   $(subst $(ROOT)/,,$(1))" ; $(MKDIRCMD) $(1)
endif

#When cross-installing default to host minix.host if not specified
ifeq ($(CI_HOST),)
CI_HOST := "minix.host"
endif

# Export variables set in the main Makefile used in recursion
export CFLAGS
export XCFLAGS
export C_STD
export LDFLAGS
export XLDFLAGS
export ARCH
export INCLUDES
export PLAT
export COMPILER_TYPE
export COMPILER
export CC
export LD
export AS
export XCC
export XLD
export XAS
export AR
export XAR
export SH
export ASMCONV
export ASMCPP
export AOUT
export DEPCMD
export MKFS
export BOOT_IMAGE_TYPE
export SERIAL_CONSOLE

export LLVM_CPASS
export LLVM_LPASS
export LLVM_PASS_DRIVERS
export LLVM_PASS_SERVERS
export LLVM_PASS_INIT
export LLVM_PASS_KERNEL
export LLCFLAGS
export LLLDFLAGS
export LLARFLAGS
export LLPFLAGS
export LLC
export LLLD
export LLAR
export LLP
export XLLC
export XLLLD
export XLLAR
export XLLP
export LLVM_COMPILE
export LLVM_COMPILE_CLEAN
export LLVM_LINK
export LLVM_LINK_CLEAN

export ROOT

usage:
	@echo "" 
	@echo "Master Makefile for MINIX commands and utilities." 
	@echo "Root privileges are required for some actions." 
	@echo "" 
	@echo "Usage:"
	@echo "	make all 	# Compile everything (libraries & commands)" 
	@echo "	make clean      # Remove all compiler results"
	@echo "	make cross-install # Cross-install everything"
	@echo "" 
	@echo "Selective make :"
	@echo "	make build_<kernel | libs | drivers | servers | commands | image>"
	@echo "	make clean_<kernel | libs | drivers | servers | commands | image>"
	@echo "	make cross-install_<drivers | servers | commands | image>"
	@echo "" 
	@echo "Run 'make' in tools/ to create a new MINIX configuration." 
	@echo "" 

all: build_image

# world has to be able to make a new system, even if there
# is no complete old system. it has to install commands, for which
# it has to install libraries, for which it has to install includes,
# for which it has to install /etc (for users and ownerships).
# etcfiles also creates a directory hierarchy in its
# 'make install' target.
# 
# etcfiles has to be done first.
world: includes depend libraries cmds install postinstall

build_kernel: kernel_libs
	$(MAKE) -C kernel/

build_servers: build_libs
	$(MAKE) -C servers/

build_drivers: build_libs build_servers build_commands
	$(MAKE) -C drivers/ all	# drivers last because of the imgrd

build_hellodrv: build_libs
	$(MAKE) -C drivers/hello all

build_commands: build_libs
	$(MAKE) -C commands/ all

build_test:
	$(MAKE) -C test all

clean_test:
	$(MAKE) -C test clean

kernel_libs: build_libs

KERNEL_ARCH_INCLUDES = $(ROOT)/kernel/.arch_includes
KERNEL_ARCH_INCLUDES_LINK = $(KERNEL_ARCH_INCLUDES)/arch
export KERNEL_ARCH_INCLUDES

$(ARCH_LINK):
	$(call SYMLINK, $(ARCHINC), $@)

$(KERNEL_ARCH_INCLUDES_LINK):
	$(call MKDIR, $(KERNEL_ARCH_INCLUDES))
	$(call SYMLINK, $(ROOT)/kernel/arch/$(ARCH), $(KERNEL_ARCH_INCLUDES_LINK))

build_libs: $(ARCH_LINK)
	$(MAKE) -C lib/
	$(MAKE) -C lib/ archives

build_tools:
	$(MAKE) -C tools/ all

cmds:
	if [ -f commands/Makefile ] ; then cd commands && $(MAKE) all; fi

install::
	if [ -f commands/Makefile ] ; then cd commands && $(MAKE) install; fi

build_kernel : $(KERNEL_ARCH_INCLUDES_LINK)

ifneq ($(PLAT), Minix)
build_kernel : ehdr2ahdr_tool
build_servers : ehdr2ahdr_tool
build_drivers : ehdr2ahdr_tool
build_hellodrv : ehdr2ahdr_tool
build_commands : ehdr2ahdr_tool

ehdr2ahdr_tool :
	$(MAKE) -C tools/ ehdr2ahdr_tool
bootloader install_bootloader:
	$(error "Boot loader is compilable only on Minix using ack compiler")
else
bootloader:
	$(MAKE) -C boot build_boot

install_bootloader:
	cd boot/ && sh updateboot.sh
endif

build_image: build_libs build_commands build_servers build_drivers build_kernel
	$(MAKE) -C tools/ image

cross-install: all
	sh tools/cross-install.sh all $(CI_HOST)

cross-install_drivers: build_drivers
	sh tools/cross-install.sh drivers $(CI_HOST)

cross-install_servers: build_servers
	sh tools/cross-install.sh servers $(CI_HOST)

cross-install_commands: build_commands
	sh tools/cross-install.sh commands $(CI_HOST)

cross-install_image: build_image
	sh tools/cross-install.sh image $(CI_HOST)

#clean::
#	cd lib && $(MAKE) $@
#	test ! -f commands/Makefile || { cd commands && $(MAKE) $@; }
#
#etcfiles::
#	cd etc && $(MAKE) install
#
#clean::
#	cd test && $(MAKE) $@
#
#install clean::
#	cd boot && $(MAKE) $@
#	cd man && $(MAKE) $@	# First manpages, then commands
#	test ! -f commands/Makefile || { cd commands && $(MAKE) $@; }

clean: clean_libs clean_servers clean_drivers clean_commands clean_kernel \
		clean_tools clean_boot clean_test
	@-rm -f $(ARCH_LINK)
	@-rm -f image.elf image.aout

clean_libs:
	$(MAKE) -C lib/ clean
clean_servers:	
	$(MAKE) -C servers/ clean
clean_drivers:	
	$(MAKE) -C drivers/ clean
clean_commands:	
	$(MAKE) -C commands/ clean
clean_kernel: 	
	$(MAKE) -C kernel/ clean
	@-rm -f $(KERNEL_ARCH_INCLUDES_LINK)
	@-rm -rf $(KERNEL_ARCH_INCLUDES)
clean_tools:
	$(MAKE) -C tools/ clean
clean_boot:
	$(MAKE) -C boot/ clean

postinstall:
	cd etc && $(MAKE) $@
