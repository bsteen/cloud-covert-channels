all: source sink

source: source.o meminfo.o
	g++ source.o meminfo.o -o source

sink: sink.o meminfo.o
	g++ sink.o meminfo.o -o sink

# For testing
# meminfo: meminfo.o
# 	g++ meminfo.o -o meminfo

source.o: source.cpp meminfo.hpp
	g++ -c source.cpp

sink.o: sink.cpp meminfo.hpp
	g++ -c sink.cpp

meminfo.o: meminfo.cpp meminfo.hpp
	g++ -c meminfo.cpp meminfo.hpp

test: test.c
	gcc test.c -o test
	
clean:
	rm -f *.o *.gch source sink meminfo test *.txt *.png