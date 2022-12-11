

pcv-par.o:
	mpicc pcv-par.c -o pcv-par.o

pcv-seq.o:
	gcc pcv-seq.c -o pcv-seq.o

run_par : pcv-par.o
	mpirun -np 4 pcv-par.o 5

run_seq : pcv-seq.o
	./pcv-seq.o 5

clear:
	rm *.o
