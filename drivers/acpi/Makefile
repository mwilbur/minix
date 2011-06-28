TARGETS := acpi

DIRS := dispatcher  events  executer  hardware  namespace \
	parser  resources  tables  utilities

SRCS := $(foreach DIR, $(DIRS), $(wildcard $(DIR)/*.c)) osminixxf.c pci.o acpi.o
OBJS-acpi := $(addsuffix .o,$(basename $(SRCS)))

CFLAGS += -I$(ROOT)/drivers/acpi/include/
CFLAGS += -std=c99
CFLAGS += -DACPI_LIBRARY

LIBS += -ldriver

include ../Makefile.inc
