EXECS = matmul
CFLAGS4= -Wall -Werror -O3  -ffast-math -mavx -ftree-vectorize -march=native -fopenmp -funroll-loops -lpthread -lgfortran


all: $(EXECS)

matmul: matmul.c
	gcc $(CFLAGS4) -o $@ $< libopenblas.a

clean: 
	rm -rf $(EXECS)

.phony: all
