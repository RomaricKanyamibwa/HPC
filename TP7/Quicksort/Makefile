EXECS = QuickSort_seq QuickSort_paral
CFLAGS= -O3 -fopenmp

all: $(EXECS)

QuickSort_seq: QuickSort_seq.c
	gcc $(CFLAGS) -o $@ $<

QuickSort_paral: QuickSort_paral.c
	gcc $(CFLAGS) -o $@ $<

clean: 
	rm -rf $(EXECS) *~

.phony: all
