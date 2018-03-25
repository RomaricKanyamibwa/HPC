EXECS = matmul_scal 
CFLAGS= -O3 #-ffast-math
OUT=output.txt 
GREP_PATTERN= -i -e mul -e fmadd
N=512

all: $(EXECS)

%_scal: %.c
	gcc -S $(CFLAGS) -o $@.s $< ; grep $(GREP_PATTERN) $@.s
	gcc $(CFLAGS) -o $@ $<

exec: all 
	rm -rf $(OUT)
	(for e in $(EXECS) ; do echo "Running $$e $N..." ; echo "" >> $(OUT) ; echo "Running $$e $N..." >> $(OUT) ; ./$$e $N >> $(OUT) ; done)

clean: 
	rm -rf $(EXECS) *~ *.s 

.phony: all
