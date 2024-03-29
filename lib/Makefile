# Détermination du compilateur
ifdef CCC
    CC=$(CCC)
else
    CC=gcc
endif

ifdef CFLAGS
	CFLAGS=$(CFLAGS)
else
	CFLAGS=
endif



objCoLib.o: objCoLib.c objCoLib.h
	$(CC) -c objCoLib.c -lvlc $(CFLAGS)

reqRep.o: reqRep.c reqRep.h
	$(CC) -c reqRep.c $(CFLAGS)

session.o: session.c session.h
	$(CC) -c session.c $(CFLAGS)

data.o: data.c data.h
	$(CC) -c data.c $(CFLAGS)

protoClient.o: protoClient.c protoClient.h
	$(CC) -c protoClient.c $(CFLAGS)

protoServer.o: protoServer.c protoServer.h
	$(CC) -c protoServer.c $(CFLAGS)


libServer.a : session.o data.o reqRep.o objCoLib.o protoServer.o
	ar -crs $@ $^
	rm -f *.o

libClient.a : session.o data.o reqRep.o objCoLib.o protoClient.o
	ar -crs $@ $^
	rm -f *.o

clean:
	rm -f *.o
	rm -f *.a


