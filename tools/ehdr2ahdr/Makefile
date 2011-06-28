all: ehdr2ahdr

OBJS := ehdr2ahdr.o elfparse.o

CFLAGS := -m32 -idirafter $(ROOT)/include

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ehdr2ahdr: $(OBJS)
	$(CC) -m32 -o $@ $(OBJS)

clean::
	rm -f ehdr2ahdr *.o

include $(ROOT)/Makefile.deps
