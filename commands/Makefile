# Makefile for commands.

BZIP2=bzip2-1.0.3
FLEX=flex-2.5.4

#SMALLPROGRAMS= $(ARCH) advent autil byacc cawf cron de dhcpd dis88 \
#		elle elvis ftp101 ftpd200 ibm indent m4 make mined patch pax \
#		profile ps reboot rlogind scripts sh simple syslogd talk talkd \
#		telnet telnetd urlget yap zoneinfo

SMALLPROGRAMS := simple ash ps aal autil service profile

usage:
	@echo "Usage: make all          # Compile all commands" >&2
	@echo "       make install      # Install the result (run as bin!)" >&2
	@echo "       make clean        # Delete .o files and other junk" >&2

all: small

install: small smallinstall

ifeq ($(PLAT), Minix)
BINSIZES_NORMAL = binsizes normal
endif

clean::
	cd $(BZIP2) && $(MAKE) clean
	cd zmodem && $(MAKE) clean
	if [ -f $(FLEX)/Makefile ]; then cd $(FLEX) && $(MAKE) distclean ; fi
	for p in $(SMALLPROGRAMS); do ( cd $$p && $(MAKE) clean ); done

.PHONY : clean $(SMALLPROGRAMS)

$(SMALLPROGRAMS):
	$(MAKE) -C $@

small: $(SMALLPROGRAMS)

smallinstall::
	set -e; for p in $(SMALLPROGRAMS); do ( cd $$p && $(MAKE) install ); done

