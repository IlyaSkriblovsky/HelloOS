OBJS = string.o stdio.o

all: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

depend:
	for i in *.[cS]; do $(CC) -M $(CFLAGS) $$i; done >depend

clean:
	rm -f *.i *.s *.o *~ *.elf depend

include depend
