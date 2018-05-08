all:
	g++ -o catan.out -fopenmp Main.cpp
run:
	export OMP_NUM_THREADS=$(p)
	./catan.out $(size) $(playernum)

clean:
	rm -rf *.o
	rm -f *.out
	rm -f *~*.h.gch *#
	rm -f *.h.gch
	rm *~