# Makefile for inet.

TARGETS := inet

LIBS += -ldriver -lutil

# Directories
BUILD_SUBDIRS = generic
g = generic

# Programs, flags, and libraries
CFLAGS += -I. -D_MINIX

OBJS-inet := 	buf.o clock.o inet.o inet_config.o \
	mnx_eth.o mq.o qp.o sr.o \
	$g/udp.o $g/arp.o $g/eth.o $g/event.o \
	$g/icmp.o $g/io.o $g/ip.o $g/ip_ioctl.o \
	$g/ip_lib.o $g/ip_read.o $g/ip_write.o \
	$g/ipr.o $g/rand256.o $g/tcp.o $g/tcp_lib.o \
	$g/tcp_recv.o $g/tcp_send.o $g/ip_eth.o \
	$g/ip_ps.o $g/psip.o \
	queryparam.o version.o

include ../Makefile.inc
