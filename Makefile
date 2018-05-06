all:
	mpiCC -g -Wall -o mpi_catan.out  Main.cpp
run:
	mpiexec -n $(p) ./mpi_catan.out $(size) $(playernum)

clean:
	rm -rf *.o
	rm -f *.out
	rm -f *~*.h.gch *#
	rm -f *.h.gch
	rm *~