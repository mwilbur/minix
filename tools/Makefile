# Makefile for the kernel image.

# tool must be built to run on the target we are compiling on!

ifeq ($(PLAT), Minix)
CFLAGS +=	-O -D_MINIX -D_POSIX_SOURCE
else
endif

# Specify the programs that are part of the system image.
PROGRAMS=	../kernel/kernel \
	../servers/ds/ds \
	../servers/rs/rs \
	../servers/pm/pm \
	../servers/sched/sched \
	../servers/vfs/vfs \
	../drivers/memory/memory \
	../drivers/log/log \
	../drivers/tty/tty \
	../servers/mfs/mfs \
	../servers/vm/vm \
	../servers/pfs/pfs \
	../servers/init/init

usage:	
	@echo " " >&2
	@echo "Master Makefile to create new MINIX configuration." >& 2
	@echo "Root privileges are required." >&2
	@echo " " >&2
	@echo "Usage:" >&2
	@echo "	make includes   # Install include files" >&2
	@echo "	make depend     # Generate dependency files" >&2
	@echo "	make libraries  # Make system libraries" >&2
	@echo "	make services   # Compile and install all services" >&2
	@echo "	make fresh      # Make clean, libraries, and services" >&2
	@echo "	make image      # Make needed services and create boot image" >&2
	@echo "	make install    # Make image, and install to hard disk" >&2
	@echo "	make hdboot     # Make image, and install to hard disk" >&2
	@echo "	make fdboot     # Make image, and install to floppy disk" >&2
	@echo "	make bootable   # Make hard disk bootable" >&2
	@echo "	make clean      # Remove all compiler results, except libs" >&2
	@echo " " >&2
	@echo "To create a fresh MINIX configuration, try:" >&2
	@echo "	make clean install      # new boot image" >&2
	@echo "	make fresh install      # new everything" >&2
	@echo " " >&2

# create a fresh configuration or system image
fresh: 
	cd ../lib && $(MAKE) clean
	$(MAKE) clean 
	$(MAKE) libraries services

#all: services image

ehdr2ahdr_tool:
	$(MAKE) -C ehdr2ahdr

IMAGE := $(ROOT)/image.$(BOOT_IMAGE_TYPE)

image: $(IMAGE)

ifeq ($(BOOT_IMAGE_TYPE), elf)
all: elf_image_tool
PROGRAMS := $(addsuffix .elf, $(PROGRAMS))
MKIMAGE := elf_image/elf_image

$(IMAGE): elf_image_tool $(PROGRAMS)
	$(MKIMAGE) $@ $(PROGRAMS)
	@echo "Image $(subst $(ROOT)/,,$(@)"

else

ifneq ($(PLAT), Minix)
INSTALLBOOT := ../boot/installboot
$(INSTALLBOOT):
	$(MAKE) -C ../boot installboot

$(IMAGE):  $(INSTALLBOOT) $(PROGRAMS)
else
INSTALLBOOT := installboot
$(IMAGE): $(PROGRAMS)
endif
	$(INSTALLBOOT) -image $@ $(PROGRAMS)
	@echo "Image $(subst $(ROOT)/,,$@)"

image_small:  includes
	cd ../kernel && $(MAKE) EXTRA_OPTS=$(EXTRA_OPTS)
	cd ../servers && $(MAKE) EXTRA_OPTS=-D_MINIX_SMALL=1 image
	cd ../drivers && $(MAKE) EXTRA_OPTS=$(EXTRA_OPTS) image
	$(INSTALLBOOT) -image $@ $(PROGRAMS)
	@echo "Image $(subst $(ROOT)/,,$(IMAGE))"

# rebuild the program or system libraries
includes:
	cd ../include && $(MAKE) install

depend: includes
	cd ../ && $(MAKE) depend

services: includes 
	cd ../kernel && $(MAKE) 
	cd ../servers && $(MAKE) all install
	cd ../drivers && $(MAKE) all install

libraries: includes
	cd ../lib && $(MAKE) clean
	cd ../lib && $(MAKE) all
	cd ../lib && $(MAKE) install

endif

# make bootable and place system images
bootable:
	exec su root mkboot bootable

hdboot:	image
	exec sh mkboot $@
	@sync

fdboot:	image
	exec su root mkboot $@
	@sync

elf_image_tool:
	$(MAKE) -C elf_image

install: includes services hdboot

# clean up compile results
clean::
	rm -rf *.bak $(IMAGE) image_small *.iso *.iso.gz cdfdimage rootimage src
	$(MAKE) -C elf_image clean
	$(MAKE) -C ehdr2ahdr clean

