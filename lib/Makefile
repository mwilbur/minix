LIBS= libsys libtimers libutil libcurses libedit libdriver libnetdriver \
	libexec
BUILDIR_PREFIX = obj-
BUILD_DIR=$(BUILDIR_PREFIX)$(COMPILER_TYPE)
BCA_BUILD_DIR=$(BUILD_DIR)/bca

LIB_BUILD_ROOT = $(ROOT)/lib/$(BUILD_DIR)
export LIB_BUILD_ROOT

# stuff for a specific compiler type. GNU libraries are not binary compatible
# with ACK
ifeq ($(COMPILER_TYPE), gnu)
COMPILER_TYPE_DIRS = \
			gnu/ieee_float  \
			gnu/rts		\
			gnu/softfloat

endif
ifeq ($(COMPILER_TYPE), ack)
COMPILER_TYPE_DIRS = \
			ack/i386/em \
			ack/i386/head \
			ack/math \
			ack/rts
#			ack/float \
#			ack/fphook \
#			ack/libm2 \
#			ack/libp \
#			ack/liby \

endif

# we need to build architecture specific stuff too
ifeq ($(ARCH), i386)
ARCH_DIRS =\
		i386/rts	\
		i386/math	\
		i386/misc	\
		i386/string
endif


# all directories that need to be compiled
BUILD_LIBC_SUBDIRS:= \
			ansi \
			asyn \
			ip \
			math \
			other \
			posix \
			regex \
			stdio \
			stdtime \
			syscall \
			sysvipc \
			wchar \
			$(addprefix arch/, $(ARCH_DIRS)) \
			$(COMPILER_TYPE_DIRS)

LIBC_SUBDIRS := $(addprefix libc/, $(BUILD_LIBC_SUBDIRS))

BUILD_SUBDIRS+= $(LIBC_SUBDIRS) $(LIBS)

# ACK needs to define symbols like edata or etext etc. in assembler, GNU can
# do it in linker scripts
ifeq ($(COMPILER_TYPE), ack)
BUILD_SUBDIRS += end
endif
			

.PHONY : clean $(BUILD_SUBDIRS)

# create directories for the object files and build them
all: build-dirs lib-objs libgcc

libgcc: obj-gnu/libgcc.a

obj-gnu/libgcc.a:
	objcopy --change-leading-char `gcc -m32 -print-libgcc-file-name` $@

#
# See if a LLVM linking pass has to be performed
#
ifneq ($(LLVM_LPASS),)

build-dirs:
	@mkdir -p $(addprefix $(BUILD_DIR)/, $(BUILD_SUBDIRS)) $(BCA_BUILD_DIR)

# the second pass collects all created object files into library archives
archives: $(addsuffix .a, $(addprefix $(BUILD_DIR)/, libc $(LIBS))) $(addsuffix .bca, $(addprefix $(BCA_BUILD_DIR)/, libc $(LIBS)))

else

build-dirs:
	@mkdir -p $(addprefix $(BUILD_DIR)/, $(BUILD_SUBDIRS))

# the second pass collects all created object files into library archives
archives: $(addsuffix .a, $(addprefix $(BUILD_DIR)/, libc $(LIBS)))

endif

$(BUILD_DIR)/libsys.a: $(wildcard $(BUILD_DIR)/libsys/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libsys.bca: $(wildcard $(BUILD_DIR)/libsys/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libtimers.a: $(wildcard $(BUILD_DIR)/libtimers/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libtimers.bca: $(wildcard $(BUILD_DIR)/libtimers/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libutil.a: $(wildcard $(BUILD_DIR)/libutil/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libutil.bca: $(wildcard $(BUILD_DIR)/libutil/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libcurses.a: $(wildcard $(BUILD_DIR)/libcurses/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libcurses.bca: $(wildcard $(BUILD_DIR)/libcurses/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libedit.a: $(wildcard $(BUILD_DIR)/libedit/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libedit.bca: $(wildcard $(BUILD_DIR)/libedit/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libdriver.a: $(wildcard $(BUILD_DIR)/libdriver/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libdriver.bca: $(wildcard $(BUILD_DIR)/libdriver/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libnetdriver.a: $(wildcard $(BUILD_DIR)/libnetdriver/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libnetdriver.bca: $(wildcard $(BUILD_DIR)/libnetdriver/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libexec.a: $(wildcard $(BUILD_DIR)/libexec/*.o)
	$(XAR) cr $@ $?
$(BCA_BUILD_DIR)/libexec.bca: $(wildcard $(BUILD_DIR)/libexec/*.bcc)
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_DIR)/libc.a: $(foreach DIR, $(LIBC_SUBDIRS), $(wildcard $(BUILD_DIR)/$(DIR)/*.o))
	$(XAR) cr $@ $?
	$(XAR) d $@ crtso.o
$(BCA_BUILD_DIR)/libc.bca: $(foreach DIR, $(LIBC_SUBDIRS), $(wildcard $(BUILD_DIR)/$(DIR)/*.bcc))
	$(XLLAR) cr$(LLARFLAGS) $@ $?

$(BUILD_SUBDIRS):
	$(MAKE) -C $@ all

lib-objs: $(BUILD_SUBDIRS)

# cleans all directories and removes all possible directories with object files
clean:
	$(foreach DIR, $(BUILD_SUBDIRS), $(MAKE) -C $(DIR) clean;)
	-rm -fr $(BUILDIR_PREFIX)*
