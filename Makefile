all: Main.o
	g++ -Wall Main.o -o Main.out

Main.o: Main.cpp
	g++ -Wall -c Main.cpp

run:
	./Main.out $(SIZE)

clean:
	rm -rf *.o
	rm -f *.out
	rm -f *~*.h.gch *#
	rm -f *.h.gch