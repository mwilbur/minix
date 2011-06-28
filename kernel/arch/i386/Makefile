# Makefile for kernel

ARCHAR=$(ARCH).a
LLARCHAR=$(ARCH).bca

# What to make.
all build install: $(HEAD) $(ARCHAR)

OBJS=	arch_do_vmctl.o \
	breakpoints.o \
	arch_clock.o \
	debugreg.o \
	do_int86.o \
	do_iopenable.o \
	do_readbios.o \
	do_sdevio.o \
	exception.o \
	i8259.o \
	io_inb.o \
	io_inl.o \
	io_intr.o \
	io_inw.o \
	io_outb.o \
	io_outl.o \
	io_outw.o \
	klib.o \
	klib16.o \
	multiboot.o \
	memory.o \
	mpx.o \
	protect.o \
	arch_system.o \
	apic.o \
	apic_asm.o \
	arch_watchdog.o \
	debugreg.o \
	pre_init.o \
	acpi.o

ifeq ($(CONFIG_SMP),y)
OBJS += arch_smp.o trampoline.o
endif

CFLAGS += -I$(shell pwd)/include

$(ARCHAR): $(filter-out mpx386.o, $(OBJS))
	$(XAR) cr $@ $?

$(LLARCHAR): BCCS =       $(foreach file, $(OBJS:.o=), $(if $(wildcard $(file).[sS]),,$(file).bcc))
$(LLARCHAR): $(filter-out mpx386.o, $(OBJS))
	$(XLLAR) cr$(LLARFLAGS) $@ $(BCCS)

clean::
	-rm -f *.a *.bca *.o *~
 
$(HEAD): mpx.o
	cp $< $@

include $(ROOT)/kernel/Makefile.inc
include $(ROOT)/Makefile.deps

