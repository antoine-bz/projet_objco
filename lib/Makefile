all : session.o data.o reqRep.o audioStream.o

objCoLib.o: objCoLib.c objCoLib.h
	gcc -c objCoLib.c -lvlc

reqRep.o: reqRep.c reqRep.h
	gcc -c reqRep.c

session.o: session.c session.h
	gcc -c session.c

data.o: data.c data.h
	gcc -c data.c

clean:
	rm -f *.o
	rm -f *.a