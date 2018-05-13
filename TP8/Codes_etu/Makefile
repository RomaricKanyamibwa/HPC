EXECS = matmul
CFLAGS4= -O3  -ffast-math -mavx -ftree-vectorize -march=native -fopenmp -funroll-loops


all: $(EXECS)

matmul: matmul.c
	gcc $(CFLAGS4) -o $@ $<

clean: 
	rm -rf $(EXECS)

.phony: all
