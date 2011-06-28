# Makefile for zmodem

ALL = rz sz

all: $(ALL)


CFLAGS+= -DPOSIX -D_MINIX -D_POSIX_SOURCE -O

ifeq ($(COMPILER_TYPE), ack)
CFLAGS += -i -wo
endif

SRCS := rbsb.c zm.c
OBJS := $(SRCS:.c=.o)

ifeq ($(PLAT), Minix)
$(ALL): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $@.c
else
LDS := $(ROOT)/commands/command.lds
$(ALL:=.elf):	$(OBJS) $(LDS)
	$(LD) $(LDFLAGS) -o $@ $@.c -lc

$(ALL): $(ALL:=.elf)
	$(AOUT)
endif


install:
	$(error "$@ not implemented")

#install:	/usr/bin/rz /usr/bin/sz
#
#/usr/bin/rz:	rz
#	install -cs -o bin rz $@
#
#/usr/bin/sz:	sz
#	install -cs -o bin sz $@
#
#clean:	
#	rm -f *.bak *.o core rz sz

clean::
	rm -f $(ALL)

include $(ROOT)/Makefile.deps
