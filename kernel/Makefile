# Makefile for kernel

# Programs, flags, etc.
# so files in kernel/system and kernel/arch/blah can inclcude files form the
# main kernel directory as "kernel/bleh.h"
CFLAGS += -I$(ROOT)
CFLAGS += -I$(ROOT)/kernel

# to inlude files from kernel/arch/???/
CFLAGS += -I$(ROOT)/kernel/arch/$(ARCH)/
CFLAGS += -I$(ROOT)/kernel/arch/$(ARCH)/include

ifneq ($(SERIAL_CONSOLE),)
CFLAGS += -DKERNEL_SERIAL_DEBUG
endif

HOSTNAME := $(shell uname -n)
BUILDTIME := $(shell date +%d-%m-%Y-%H:%M)

CFLAGS += -D_BUILD_VERSION="\"$(HOSTNAME)-$(BUILDTIME)\""

# first-stage, arch-dependent startup code
HEAD =	head.o
FULLHEAD = arch/$(ARCH)/$(HEAD)

OBJS =	start.o	\
	table.o	\
	main.o	\
	proc.o	\
	system.o	\
	clock.o	\
	utility.o	\
	debug.o	\
	profile.o	\
	interrupt.o	\
	watchdog.o	\
	cpulocals.o

ifeq ($(CONFIG_SMP),y)
OBJS += smp.o
endif

SYSTEM = system.a
ARCHLIB = $(PWD)/kernel/arch/$(ARCH)/$(ARCH).a
LLSYSTEM = system.bca
LLARCHLIB = $(PWD)/kernel/arch/$(ARCH)/$(ARCH).bca

LIBS = -lgcc -ltimers -lsys -lc

# What to make.
all: build

build install: $(LDS) head system_lib arch_lib kernel

$(SYSTEM) $(LLSYSTEM): system_lib
$(FULLHEAD) $(ARCHLIB) $(LLARCHLIB): arch_lib

VPATH += $(ROOT)/lib/obj-$(COMPILER_TYPE)/
LIB_FILES = $(addsuffix .a, $(subst -l,lib,$(LIBS)))
LLLIB_FILES = $(addsuffix .bca, $(subst -l,lib,$(LIBS)))

ifneq ($(COMPILER_TYPE),ack)
LDS := kernel.lds
LDFLAGS += -T $(LDS)
endif

STACK_SIZE-kernel=0k

ifeq ($(PLAT), Minix)
kernel: $(LDS) $(FULLHEAD) $(OBJS) $(SYSTEM) $(ARCHLIB) $(LIB_FILES)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@ $(FULLHEAD) $(OBJS) \
		$(SYSTEM) $(ARCHLIB) $(LIBS)
	install -S 0 $@
else
kernel: kernel.elf
	$(AOUT)

endif

clean::
	$(MAKE) -C system clean
	$(MAKE) -C arch/$(ARCH) clean
	-rm -f *.a *.o *~ *.bak kernel kernel.elf kernel.raw kernel.ahdr

head: 
	$(MAKE) -C arch/$(ARCH) HEAD=$(HEAD) $(HEAD)

include $(ROOT)/kernel/Makefile.inc
include $(ROOT)/Makefile.deps

#
# See if a LLVM linking pass has to be performed
#
ifeq ($(DO_LLVM_LPASS),1)

system_lib:
	$(MAKE) -C system ../$(LLSYSTEM) ../$(SYSTEM)
arch_lib:
	$(MAKE) -C arch/$(ARCH) $(notdir $(LLARCHLIB) $(ARCHLIB))

kernel.elf: TARGET_NAME = $(@:.elf=)
kernel.elf: LIBSOPT = $(SYSTEM) $(ARCHLIB) $(LIBS)
kernel.elf: LLLIBSOPT = $(LLSYSTEM) $(LLARCHLIB) $(LIBS)
kernel.elf: BCCS =       $(foreach file, $(OBJS:.o=), $(if $(wildcard $(file).[sS]),,$(file).bcc))
kernel.elf: OBJS-NOBCC = $(foreach file, $(OBJS:.o=), $(if $(wildcard $(file).[sS]),$(file).o,)) $(FULLHEAD)
kernel.elf: $(LDS) $(FULLHEAD) $(OBJS) $(LLSYSTEM) $(LLARCHLIB) $(SYSTEM) $(ARCHLIB) $(LIB_FILES) $(BCCS)
	$(LLVM_LINK) 

else

system_lib:
	$(MAKE) -C system ../$(SYSTEM)
arch_lib:
	$(MAKE) -C arch/$(ARCH) $(notdir $(ARCHLIB))

kernel.elf: $(LDS) $(FULLHEAD) $(OBJS) $(SYSTEM) $(ARCHLIB) $(LIB_FILES)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@ $(FULLHEAD) $(OBJS) \
		$(SYSTEM) $(ARCHLIB) $(LIBS)

endif
