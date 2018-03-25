EXECS =  ProduitTermeATerme_scal
CFLAGS= -O3 -ffast-math -mavx 
CFLAGS2= -O3 -ffast-math -ftree-vectorize #-ftree-vectorizer-verbose=2
CFLAGS3= -O3  -ffast-math -march=native
CFLAGS4= -O3  -ffast-math -mavx -ftree-vectorize -march=native #
OUT=output.txt 

all: $(EXECS)

ProduitTermeATerme_scal: ProduitTermeATerme.c
	gcc -S $(CFLAGS) -o $@.s $< ; grep -i mul $@.s
	gcc $(CFLAGS) -o $@ $<
	gcc $(CFLAGS2) -o $@_vect $<
	gcc $(CFLAGS3) -o $@_march $<
	gcc $(CFLAGS4) -o $@_All $<
	gcc -o $@_norm $<
	gcc $(CFLAGS) -o $@_mavx ProduitTermeATerme_vectorize.c

exec: all 
	rm -rf $(OUT)
	(for e in $(EXECS) ; do echo "Running $$e ..." ; echo "" >> $(OUT) ; echo "Running $$e ..." >> $(OUT) ; ./$$e $$N >> $(OUT) ; done)

clean: 
	rm -rf $(EXECS) *~ *.s 

.phony: all
