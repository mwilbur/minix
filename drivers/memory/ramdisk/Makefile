# Makefile for ramdisk image

PROGRAMS=at_wini bios_wini cdprobe dev2name floppy loadramdisk newroot \
	pci sh service sysenv mfs acpi
EXTRA=system.conf passwd rs.single

MAKEDEV=/usr/bin/MAKEDEV

ifeq ($(PLAT),Minix)
INSTALL_CMD:=install -s
else
INSTALL_CMD:=cp
endif

all:	image.c 

clean::
	-rm -rf $(PROGRAMS) $(EXTRA) bintoc image image.c 
#	-rm -rf image.s t proto.gen

image.c:	bintoc image proto.gen mtab rc $(PROGRAMS) $(EXTRA)
	./bintoc -o $@ image

image.s:	image.c
	sed < image.c > $@ 's/^/.data1 /;s/,$$//' || { rm -f $@; false; }

# Note for cross compilation: this executable has to be compiled for the
# host system
bintoc:	bintoc.c
	$(CC) -o $@ bintoc.c

ifneq ($(PLAT), Minix)
MKFS_SRC := $(ROOT)/commands/simple/mkfs.c
$(MKFS_NATIVE): $(MKFS_SRC)
	$(CC) -m32 -D_POSIX_SOURCE -idirafter $(ROOT)/include -I$(ROOT)/servers -o $@ $<
image: $(MKFS_NATIVE)
clean::
	rm -f $(MKFS_NATIVE)

endif

image :	proto.gen mtab rc $(PROGRAMS) $(EXTRA)
#	dd if=/dev/zero of=image bs=1k count=1512
	$(MKFS) image proto.gen || { rm -f image; false; }

acpi: ../../acpi/acpi
	$(INSTALL_CMD) ../../$@/$@ $@

at_wini: ../../at_wini/at_wini
	$(INSTALL_CMD) ../../$@/$@ $@

bios_wini: ../../bios_wini/bios_wini
	$(INSTALL_CMD) ../../$@/$@ $@

floppy: ../../floppy/floppy
	$(INSTALL_CMD) ../../$@/$@ $@

pci: ../../pci/pci
	$(INSTALL_CMD) ../../$@/$@ $@

cdprobe:  ../../../commands/simple/cdprobe
	$(INSTALL_CMD) ../../../commands/simple/$@ $@

dev2name:  ../../../commands/simple/dev2name
	$(INSTALL_CMD) ../../../commands/simple/$@ $@

loadramdisk:  ../../../commands/simple/loadramdisk
	$(INSTALL_CMD) ../../../commands/simple/$@ $@

newroot:  ../../../commands/simple/newroot
	$(INSTALL_CMD) ../../../commands/simple/$@ $@

sysenv:  ../../../commands/simple/sysenv
	$(INSTALL_CMD) ../../../commands/simple/$@ $@

sh:	../../../commands/ash/sh
	$(INSTALL_CMD) ../../../commands/ash/$@ $@

service: ../../../commands/service/service
	$(INSTALL_CMD) ../../../commands/service/service $@

mfs: ../../../servers/mfs/mfs
	$(INSTALL_CMD) ../../../servers/mfs/$@ $@

system.conf: ../../../etc/system.conf
	$(INSTALL_CMD) ../../../etc/$@ $@

rs.single: ../../../etc/rs.single
	$(INSTALL_CMD) ../../../etc/$@ $@

passwd: ../../../etc/passwd
	$(INSTALL_CMD) ../../../etc/$@ $@


#proto.gen: $(MAKEDEV) proto.sh proto
#	sh -e proto.sh >proto.gen
