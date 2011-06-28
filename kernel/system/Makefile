# Makefile for system library implementation

SYSTEM	= ../system.a
LLSYSTEM = ../system.bca

# What to make.
all build install: $(SYSTEM)

OBJS	= \
	do_fork.o \
	do_exec.o \
	do_newmap.o \
	do_clear.o \
	do_exit.o \
	do_trace.o \
	do_runctl.o \
	do_update.o \
	do_times.o \
	do_setalarm.o \
	do_stime.o \
	do_vtimer.o \
	do_irqctl.o \
	do_devio.o \
	do_vdevio.o \
	do_copy.o \
	do_umap.o \
	do_memset.o \
	do_setgrant.o \
	do_privctl.o \
	do_segctl.o \
	do_safecopy.o \
	do_safemap.o \
	do_sysctl.o \
	do_getksig.o \
	do_endksig.o \
	do_kill.o \
	do_sigsend.o \
	do_sigreturn.o \
	do_abort.o \
	do_getinfo.o \
	do_sprofile.o \
	do_cprofile.o \
	do_profbuf.o \
	do_vmctl.o \
	do_mcontext.o \
	do_schedule.o \
	do_schedctl.o \
   do_statectl.o

$(SYSTEM): $(OBJS)
	$(XAR) cr $@ $(OBJS)

$(LLSYSTEM): BCCS =       $(foreach file, $(OBJS:.o=), $(if $(wildcard $(file).[sS]),,$(file).bcc))
$(LLSYSTEM): $(OBJS)
	$(XLLAR) cr$(LLARFLAGS) $@ $(BCCS)

clean::
	rm -f $(SYSTEM) $(LLSYSTEM) *.o *~ *.bak 

include $(ROOT)/kernel/Makefile.inc
include $(ROOT)/Makefile.deps

