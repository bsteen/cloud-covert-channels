all: sendWithLocks receiveWithLocks

receiveWithLocks: receiveWithLocks.o manageProcLocks.o
	gcc -o receiveWithLocks receiveWithLocks.o manageProcLocks.o

sendWithLocks: manageProcLocks.o sendWithLocks.o
	gcc manageProcLocks.o sendWithLocks.o -o sendWithLocks

receiveWithLocks.o: receiveWithLocks.c
	gcc -c receiveWithLocks.c

sendWithLocks.o: sendWithLocks.c
	gcc -c sendWithLocks.c

manageProcLocks.o: manageProcLocks.c manageProcLocks.h
	gcc -c manageProcLocks.c

clean:
	rm -f *.o receiveWithLocks sendWithLocks

#download: scp -r hnw@128.4.27.28:/home/hnw/Documents/bsteen/covert-channels/programs /home/benjamin/Documents/
#upload:   scp -r /home/benjamin/Documents/programs hnw@128.4.27.28:/home/hnw/Documents/bsteen/covert-channels/
