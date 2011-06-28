ifneq ($(COMPILER_TYPE),ack)
LDFLAGS += -u _start
LDFLAGS += -T $(LDS)
endif


VPATH += $(ROOT)/lib/obj-$(COMPILER_TYPE)/

LIBS += -lgcc -lc -lutil -ledit -lcurses

# gnu like compiler do just only pass through libraries to find undefined
# symbols by default. That is not enough always if libraries depend on each
# other. We need to force such compiler resolve all undefined symbols

ifeq ($(COMPILER_TYPE),gnu)

LDFLAGS += -static
LIBSOPT = --start-group $(LIB_DEP) $(LIBS) --end-group

else

LIBSOPT = $(LIBS) $(LIB_DEP) $(LIBS) $(LIB_DEP)
LIBSOPT += $(ROOT)/lib/obj-$(COMPILER_TYPE)/end/em_end.o

endif

LLLIBSOPT = $(LIBS) $(LIB_DEP) $(LIBS) $(LIB_DEP)

LIB_FILES = $(addsuffix .a, $(subst -l,lib,$(LIBS))) $(LIB_DEP)

LDFLAGS += $(ROOT)/lib/obj-$(COMPILER_TYPE)/libc/arch/$(ARCH)/rts/crtso.o
