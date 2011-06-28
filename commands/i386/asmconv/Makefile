# Makefile for asmconv.

PLAT=$(shell uname)

CFLAGS+= $(OPT)
LDFLAGS= -i

ifneq ($(PLAT), Minix)
	CFLAGS += -g
else
	INSTALL := install -S 8kw $@
endif

all:	asmconv

OBJ=	asm86.o asmconv.o globals.o parse_ack.o parse_gnu.o parse_bas.o \
	tokenize.o emit_ack.o emit_gnu.o

asmconv:	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

install:	/usr/bin/asmconv

/usr/bin/asmconv:	asmconv
	install -cs -o bin asmconv $@

clean:
	rm -f $(OBJ) asmconv core

# Dependencies.
asm86.o:	asm86.h asmconv.h token.h
asmconv.o:	asmconv.h languages.h asm86.h
parse_ack.o:	asmconv.h languages.h token.h asm86.h
parse_gnu.o:	asmconv.h languages.h token.h asm86.h globals.h
parse_bas.o:	asmconv.h languages.h token.h asm86.h
tokenize.o:	asmconv.h token.h
emit_ack.o:	asmconv.h languages.h token.h asm86.h globals.h
emit_gnu.o:	asmconv.h languages.h token.h asm86.h
