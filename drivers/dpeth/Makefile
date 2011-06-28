##
##  Makefile for ISA ethernet drivers	May  02, 2000
##

##  Programs, flags, etc.
TARGETS := dpeth

LIBS += -lnetdriver

OBJS-dpeth := 3c501.o 3c509.o 3c503.o ne.o wd.o 8390.o devio.o netbuff.o dp.o

CFLAGS += -I..

include ../Makefile.inc
