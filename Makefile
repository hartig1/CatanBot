all:
	g++ -o catan -fopenmp Main.cpp
run:
	export OMP_NUM_THREADS=$(p)
	./catan $(size) $(playernum)

clean:
	rm -rf *.o
	rm -f *.out
	rm -f *~*.h.gch *#
	rm -f *.h.gch
	rm *~