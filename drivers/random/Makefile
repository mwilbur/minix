# Makefile for random driver (RANDOM)
TARGETS := random

LIBS += -ldriver -lsys -lutil

BUILD_SUBDIRS = aes

OBJS-random := main.o random.o aes/rijndael_api.o aes/rijndael_alg.o

include ../Makefile.inc

