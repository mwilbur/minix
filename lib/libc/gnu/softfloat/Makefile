SRCS = softfloat.c gcc_softfloat.c

# this is compiled only by a C99 capable compilers anyway (gcc)
C_STD=

BUILD_DIR = $(LIB_BUILD_ROOT)/libc/gnu/softfloat
include $(ROOT)/lib/Makefile.inc

test_long_double_size: test_long_double_size.c
	$(XCC) -o $@ $<

#
# See if a LLVM compilation pass has to be performed
#
ifneq ($(LLVM_CPASS),)

$(BUILD_DIR)/gcc_softfloat.o: TARGET_NAME = $(@:.o=)
$(BUILD_DIR)/gcc_softfloat.o: INPUT_NAME  = gcc_softfloat
$(BUILD_DIR)/gcc_softfloat.o: CFLAGS = "-D__SIZEOF_LONG_DOUBLE__=`./test_long_double_size`"
$(BUILD_DIR)/gcc_softfloat.o: test_long_double_size gcc_softfloat.c
	$(LLVM_COMPILE)

else
 
$(BUILD_DIR)/gcc_softfloat.o: gcc_softfloat.c test_long_double_size
	$(XCC) $(C_STD) $(XCFLAGS) $(CFLAGS) "-D__SIZEOF_LONG_DOUBLE__=`./test_long_double_size`" -c -o $@ gcc_softfloat.c

endif

clean::
	-rm -f test_long_double_size
