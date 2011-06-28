# Makefile for lib/stdtime.

Z=../../../commands/zoneinfo

CFLAGS+=-D_MINIX -D_POSIX_SOURCE -D__USG -I$Z

SRCS= \
 	ftime.c		\
	asctime.c	\
	localtime.c	\
	strftime.c	\
	strptime.c
	
		
BUILD_DIR = $(LIB_BUILD_ROOT)/libc/stdtime
include $(ROOT)/lib/Makefile.inc
