all: source sink

source: source.o meminfo.o
	gcc source.o meminfo.o -o source

sink: sink.o meminfo.o
	gcc sink.o meminfo.o -o sink

source.o: source.c meminfo.h
	gcc -c source.c

sink.o: sink.c meminfo.h
	gcc -c sink.c

meminfo.o: meminfo.c meminfo.h
	gcc -c meminfo.c

clean:
	rm -f *.o source sink meminfo