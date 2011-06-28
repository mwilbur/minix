# Makefile for lib/ip.
#
# Note: The oneC_sum.c file is not used if there is an assembly equivalent.

CFLAGS+=-O -D_MINIX -D_POSIX_SOURCE -I. -DNDEBUG


SRCS= \
	accept.c \
	addrinfo.c \
	bind.c \
	connect.c \
	dhcp_gettag.c \
	dhcp_settag.c \
	ether_line.c \
	ethera2n.c \
	ethere2a.c \
	etherh2n.c \
	ethern2h.c \
	gai_strerror.c \
	getdomain.c \
	gethnmadr.c \
	gethostent.c \
	gethostname.c \
	getifaddrs.c \
	getnetbyaddr.c \
	getnetbyname.c \
	getnetent.c \
	getpeereid.c \
	getpeername.c \
	getproto.c \
	getprotoent.c \
	getprotoname.c \
	getservent.c \
	getsockname.c \
	getsockopt.c \
	getsrvbyname.c \
	getsrvbyport.c \
	hton.c \
	inet_addr.c \
	inet_network.c \
	inet_ntoa.c \
	inet_ntop.c \
	inet_pton.c \
	listen.c \
	memcspn.c \
	nameinfo.c \
	oneC_sum.c \
	rcmd.c \
	recv.c \
	recvfrom.c \
	recvmsg.c \
	res_comp.c \
	res_init.c \
	res_mkquery.c \
	res_query.c \
	res_send.c \
	ruserok.c \
	send.c \
	sendmsg.c \
	sendto.c \
	servxcheck.c \
	sethostent.c \
	setsockopt.c \
	shutdown.c \
	socket.c \
	socketpair.c \
	strcasecmp.c

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/ip
include $(ROOT)/lib/Makefile.inc
