all: source sink

source: source.o meminfo.o
	g++ source.o meminfo.o -o source

sink: sink.o meminfo.o
	g++ sink.o meminfo.o -o sink

meminfo.o: meminfo.cpp meminfo.hpp
	g++ -c meminfo.cpp

source.o: source.cpp meminfo.hpp
	g++ -c source.cpp 

sink.o: sink.cpp meminfo.hpp
	g++ -c sink.cpp

test: test.c
	gcc test.c -o test
	
clean:
	rm -f *.o *.gch source sink meminfo test output/*.txt output/*.png