OPT=-O0 -march=core2 -mpc64
G95=-mpc64

all: sum_gfortran.x sum_gcc.x
clean:
	rm -f *.s *.o *.x

sum_intel.x: sum_numbers.f90
	ifort -o $@ $(OPT) $(INTEL) $^

sum_pgi.x: sum_numbers.f90
	pgf90 -o $@ $(OPT) $(PGI) $^

sum_g95.x: sum_numbers.f90
	g95 -o $@ $(OPT) $(G95) $^

sum_gfortran.x: sum_numbers.f90
	gfortran -o $@ $(OPT) $(G95) $^

sum_gcc.x: sum_numbers.c
	gcc -o $@ $(OPT) $(G95) $^
