SERVERS = vm pm ds init mfs pfs iso9660fs vfs rs is inet ipc sched

.PHONY : clean $(SERVERS)

all: $(SERVERS)

$(SERVERS):
	$(MAKE) -C $@

clean:
	$(foreach SRV, $(SERVERS), $(MAKE) -C $(SRV) clean;)
