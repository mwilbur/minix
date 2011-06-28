# Makefile for lib/float.

FC_SRCS= \
	add_ext.fc \
	adder.fc \
	adf4.fc \
	adf8.fc \
	cff4.fc \
	cff8.fc \
	cfi.fc \
	cfu.fc \
	cif4.fc \
	cif8.fc \
	cmf4.fc \
	cmf8.fc \
	compact.fc \
	cuf4.fc \
	cuf8.fc \
	div_ext.fc \
	dvf4.fc \
	dvf8.fc \
	extend.fc \
	fef4.fc \
	fef8.fc \
	fif4.fc \
	fif8.fc \
	mlf4.fc \
	mlf8.fc \
	mul_ext.fc \
	ngf4.fc \
	ngf8.fc \
	nrm_ext.fc \
	sbf4.fc \
	sbf8.fc \
	sft_ext.fc \
	shifter.fc \
	sub_ext.fc \
	zrf4.fc \
	zrf8.fc \
	zrf_ext.fc

SRCS+= $(FC_SRCS)  fptrp.s

BUILD_DIR := $(LIB_BUILD_ROOT)/ack/float
OBJS := $(addsuffix .o,$(basename $(SRCS)))

all: $(addprefix $(BUILD_DIR)/, $(OBJS))

$(BUILD_DIR)/%.o: %.fc 
	$(SH) ./FP.compile $@ $<

$(BUILD_DIR)/%.o: %.s
	$(AS) $(CFLAGS) -c -o $@ $<

clean::
	rm -f $(SRCS:.fc=.s)
