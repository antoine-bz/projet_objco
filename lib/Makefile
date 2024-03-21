all : session.o data.o reqRep.o


reqRep.o: reqRep.c reqRep.h
	gcc -c reqRep.c

session.o: session.c session.h
	gcc -c session.c

data.o: data.c data.h
	gcc -c data.c

clean:
	rm -f *.o
	rm -f *.a