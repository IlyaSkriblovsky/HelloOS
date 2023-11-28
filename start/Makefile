OBJS = start.o ../lib/string.o ../lib/stdio.o

all: start

start: $(OBJS)
	$(LD) $(LD_FLAGS) -M -o $@ $(OBJS) -e main >map.txt

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.i *.s *.o *~ start depend map.txt


depend:
	for i in *.[cS]; do $(CC) -M $(CFLAGS) $$i; done >depend

include depend
