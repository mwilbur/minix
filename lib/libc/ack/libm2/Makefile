# Makefile for lib/ack/libm2.

CFLAGS+=-O -I../h -wo
M2FLAGS=-O -ws -n


SRCS= \
	Arguments.c \
	ArraySort.mod \
	CSP.mod \
	Conversion.mod \
	EM.e \
	InOut.mod \
	LtoUset.e \
	MathLib0.mod \
	Mathlib.mod \
	PascalIO.mod \
	Processes.mod \
	RealConver.mod \
	RealInOut.mod \
	SYSTEM.c \
	Semaphores.mod \
	Storage.mod \
	StrAss.c \
	Streams.mod \
	Strings.mod \
	Termcap.mod \
	Terminal.mod \
	Traps.mod \
	XXTermcap.c \
	absd.c \
	absf.e \
	absi.c \
	absl.c \
	blockmove.c \
	cap.c \
	catch.c \
	confarray.c \
	dvi.c \
	halt.c \
	head_m2.e \
	init.c \
	load.c \
	par_misc.e \
	random.mod \
	rcka.c \
	rcki.c \
	rckil.c \
	rcku.c \
	rckul.c \
	sigtrp.c \
	stackprio.c \
	store.c \
	ucheck.c 
	
BUILD_DIR = $(LIB_BUILD_ROOT)/ack/libm2
include $(ROOT)/lib/Makefile.inc

%.o: %.mod
	$(M2) $(M2FLAGS) -o $@ $<
