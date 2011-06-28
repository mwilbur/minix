DRIVERS=\
		tty	\
		amddev	\
		at_wini	\
		floppy	\
		printer	\
		rtl8139	\
		rtl8169	\
		orinoco	\
		fxp	\
		e1000	\
		dpeth	\
		dec21140A \
		atl2	\
		log	\
		bios_wini	\
		filter	\
		hello	\
		random	\
		readclock	\
		dp8390	\
		lance	\
		pci	\
		ti1225	\
		acpi

.PHONY:   clean memory $(DRIVERS)

all: memory

memory: $(DRIVERS) service
	$(MAKE) -C $@

service: ../commands/service/service
	$(MAKE) -C ../commands/ service

$(DRIVERS):
	$(MAKE) -C $@

clean:
	$(foreach DRV, $(DRIVERS), $(MAKE) -C $(DRV) clean;)
	$(MAKE) -C memory/ clean
