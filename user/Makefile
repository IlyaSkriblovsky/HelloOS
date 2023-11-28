BINS = test cat ls xo waitvar setvar

all: $(BINS)

%: %.o
	$(LD) $(LD_FLAGS) -M -e main -o $@ $< ../lib/stdio.o ../lib/string.o >map.$<.txt

.c.o:
	$(CC) $(CFLAGS) -O1 -c $< -o $@


clean:
	rm -f *.i *.s *.o *~ $(BINS) *.o depend map.*.txt

depend:
	for i in *.[cS]; do $(CC) -M $(CFLAGS) $$i; done >depend


include depend
