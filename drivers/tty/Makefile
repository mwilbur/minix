# Makefile for terminal driver (TTY)
TARGETS := tty

ifeq ($(SERIAL_CONSOLE), clone)
CFLAGS += -DDUP_CONS_TO_SER
endif

ifeq ($(SERIAL_CONSOLE), only)
CFLAGS += -DDUP_CONS_TO_SER -DCONS_SER_ONLY
endif

LIBS += -ldriver -ltimers

OBJS-tty := tty.o console.o keyboard.o pty.o rs232.o 

include ../Makefile.inc
