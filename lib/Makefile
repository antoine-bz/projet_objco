# Détermination du compilateur
ifdef CCC
    CC=$(CCC)
else
    CC=gcc
endif

all: libServer.a libClient.a

objCoLib.o: objCoLib.c objCoLib.h
	$(CC) -c objCoLib.c -lvlc

reqRep.o: reqRep.c reqRep.h
	$(CC) -c reqRep.c

session.o: session.c session.h
	$(CC) -c session.c

data.o: data.c data.h
	$(CC) -c data.c

protoClient.o: protoClient.c protoClient.h
	$(CC) -c protoClient.c

protoServer.o: protoServer.c protoServer.h
	$(CC) -c protoServer.c


libServer.a : session.o data.o reqRep.o objCoLib.o protoServer.o
	ar -crs $@ $^
	rm -f *.o

libClient.a : session.o data.o reqRep.o objCoLib.o protoClient.o
	ar -crs $@ $^
	rm -f *.o

clean:
	rm -f *.o
	rm -f *.a


