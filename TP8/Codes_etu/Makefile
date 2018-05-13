EXECS = matmul
CFLAGS4= -Wall -Werror -O3  -ffast-math -mavx -ftree-vectorize -march=native -fopenmp -funroll-loops -I/opt/OpenBLAS/include/ -L/opt/OpenBLAS/lib -lopenblas -lpthread -lgfortran


all: $(EXECS)

matmul: matmul.c
	gcc $(CFLAGS4) -o $@ $<

clean: 
	rm -rf $(EXECS)

.phony: all
