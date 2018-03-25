EXECS = matmul_scal
CFLAGS= -O3 -ffast-math -mavx -std=c11
CFLAGS2= -O3 -ffast-math -ftree-vectorize #-ftree-vectorizer-verbose=2
CFLAGS3= -O3  -ffast-math -march=native
CFLAGS4= -O3  -ffast-math -mavx -ftree-vectorize -march=native
OUT=output.txt 
GREP_PATTERN= -i -e mul -e fmadd
N=512

all: $(EXECS)

%_scal: %.c
	gcc -S $(CFLAGS) -o $@.s $< ; grep $(GREP_PATTERN) $@.s
	gcc $(CFLAGS) -o $@ $<
	gcc $(CFLAGS2) -o $@_vect $<
	gcc $(CFLAGS3) -o $@_march $<
	gcc $(CFLAGS4) -o $@_All $<
	gcc -o $@_norm $<
	gcc $(CFLAGS) -o $@_mavx matmul_vectorize.c

exec: all 
	rm -rf $(OUT)
	(for e in $(EXECS) ; do echo "Running $$e $N..." ; echo "" >> $(OUT) ; echo "Running $$e $N..." >> $(OUT) ; ./$$e $N >> $(OUT) ; done)

clean: 
	rm -rf $(EXECS) *~ *.s 

.phony: all
