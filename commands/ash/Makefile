# Makefile for ash.

#SRCS=	alias.c builtins.c cd.c error.c eval.c exec.c expand.c histedit.c \
#	input.c \
#	jobs.c mail.c main.c memalloc.c miscbltin.c mystring.c nodes.c \
#	options.c parser.c redir.c setmode.c show.c signames.c syntax.c \
#	trap.c \
#	output.c var.c
#
#OBJS=	alias.o builtins.o cd.o error.o eval.o exec.o expand.o histedit.o \
#	input.o \
#	jobs.o mail.o main.o memalloc.o miscbltin.o mystring.o nodes.o \
#	options.o parser.o redir.o setmode.o show.o signames.o syntax.o \
#	trap.o \
#	output.o var.o init.o \
#	bltin/echo.o bltin/expr.o bltin/operators.o bltin/regexp.o \
#	arith.o arith_lex.o

CFLAGS	+= -DSHELL -I. -D_MINIX -c
CFLAGS += -DNO_HISTORY # doesn't need libedit
CFLAGS += -DEDITLINE
CFLAGS += -I$(ROOT)/includes/arch

LDS := $(ROOT)/userapp.lds
include $(ROOT)/Makefile.userlibs.inc

LEX := flex

SRCS=	alias.c builtins.c cd.c error.c eval.c exec.c expand.c histedit.c \
	input.c \
	jobs.c mail.c main.c memalloc.c miscbltin.c mystring.c nodes.c \
	options.c parser.c redir.c setmode.c show.c signames.c syntax.c \
	trap.c \
	output.c var.c \

OBJS=	alias.o builtins.o cd.o error.o eval.o exec.o expand.o histedit.o \
	input.o \
	jobs.o mail.o main.o memalloc.o miscbltin.o mystring.o nodes.o \
	options.o parser.o redir.o setmode.o show.o signames.o syntax.o \
	trap.o \
	output.o var.o init.o \
	bltin/echo.o bltin/expr.o bltin/operators.o bltin/regexp.o \
	arith.o arith_lex.o


all:	sh



OBJS += libyywrap.o

libyywrap.o : $(ROOT)/commands/flex-2.5.4/libyywrap.c
	$(XCC) $(XCFLAGS) $(CFLAGS) -o $@ $<

%.o: %.c
	$(XCC) $(XCFLAGS) $(CFLAGS) -o $@ $<

DEPS = mknodes nodes.c nodes.h syntax.c syntax.h bltin/operators.h

# Enable this if you don't want job control
NO_JOBS=-DJOBS=0
MKB_NO_JOBS=-j

CFLAGS +=  $(NO_JOBS)

ifeq ($(PLAT), Minix)
sh:	$(DEPS) $(OBJS) $(LDS)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@  $(OBJS) $(LIBSOPT)
else
sh.elf:	$(DEPS) $(OBJS) $(LDS)
	$(XLD) $(XLDFLAGS) $(LDFLAGS) -o $@  $(OBJS) $(LIBSOPT)
sh: sh.elf
	$(AOUT)
endif

arith.c: builtins.h arith.y
	$(YACC) -d -l arith.y
	mv y.tab.c $@

arith_y.h: arith.c
	mv y.tab.h $@

arith_lex.c: arith_y.h arith_lex.l

builtins.c builtins.h: builtins.def shell.h
	sh mkbuiltins $(MKB_NO_JOBS) . shell.h builtins.def

nodes.c nodes.h: mknodes nodetypes nodes.c.pat
	./mknodes nodetypes nodes.c.pat

mknodes: mknodes.c      
	$(CC) mknodes.c -o $@


syntax.c syntax.h: mksyntax
	./mksyntax

mksyntax: mksyntax.c parser.h
	$(CC) -D__minix mksyntax.c -o $@

parser.c: token.h

token.h: mktokens
	sh mktokens

signames.c signames.h: mksignames
	./mksignames

mksignames: mksignames.c  
	$(CC) mksignames.c -o $@

init.c: mkinit $(SRCS)
	./mkinit $(SRCS)

mkinit: mkinit.c          
	$(CC) -D__minix mkinit.c -o $@

bltin/operators.h bltin/operators.c : bltin/mkexpr bltin/unary_op bltin/binary_op
	cd bltin && sh mkexpr unary_op binary_op

# Dependencies you say?  This will have to do.
$(OBJS): error.h eval.h exec.h expand.h init.h input.h \
	jobs.h machdep.h mail.h main.h memalloc.h mystring.h options.h \
	output.h parser.h redir.h shell.h trap.h var.h \
	builtins.h nodes.h signames.h syntax.h


CLEANFILES= $(OBJS) \
	arith.c arith_y.h arith_lex.c builtins.c builtins.h init.c \
	mkinit mknodes mksignames mksyntax \
	nodes.c nodes.h signames.c signames.h syntax.c syntax.h \
	token.h \
	bltin/operators.h bltin/operators.c


clean::
	-rm -f $(CLEANFILES)
	-rm -f sh sh.*

# the dependencies are too hairy and of not much use here so we don't generate
# them for now
NODEPS:=1
