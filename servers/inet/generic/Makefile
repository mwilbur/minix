# Makefile for inet/generic
TARGETS := inet

# Programs, flags, and libraries
CFLAGS += -I. -D_MINIX

OBJS-inet := 	udp.o arp.o eth.o event.o \
	icmp.o io.o ip.o ip_ioctl.o \
	ip_lib.o ip_read.o ip_write.o \
	ipr.o rand256.o tcp.o tcp_lib.o \
	tcp_recv.o tcp_send.o ip_eth.o \
	ip_ps.o psip.o

include ../../Makefile.inc
