# Makefile for the tests.

CFLAGS += -O -D_MINIX -D_POSIX_SOURCE

TESTS := test1  test2  test3  test4  test5  test6  test7  test8  test9  \
	test10        test12 test13 test14 test15 test16 test17 test18 test19 \
	       test21 test22 test23        test25 test26 test27 test28 test29 \
	test30 test31 test32        test34 test35 test36 test37 test38 \
	test39 t10a t11a t11b test40 t40a t40b t40c t40d t40e t40f test41 \
	test42 test44 test45 test47 test48 test49 test 50 test51 test52 \
	\
	test20 test24	\
	\
	test11 test33	\

OBJS := $(TESTS:=.o)

LDS := $(ROOT)/userapp.lds
include $(ROOT)/Makefile.userlibs.inc

all:	$(TESTS)
	chmod 755 *.sh run

%: %.c
	$(CC) $(C_STD) $(CFLAGS) -c -o $@ $<

ifeq ($(PLAT), Minix)
$(TESTS): %: %.o $(LDS)
	$(LD) $(LDFLAGS) -o $@ $< $(LIBSOPT)
else

%.elf: %.o $(LDS)
	$(LD) $(LDFLAGS) -o $@ $< $(LIBSOPT)

$(TESTS): %: %.elf
	$(AOUT)
endif

clean::
	-rm -f $(TESTS)

include $(ROOT)/Makefile.deps
