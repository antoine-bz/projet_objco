serveur: libServer.a 
	gcc  main.c -DSERVEUR -o serveur.exe -lServer -L./ -lvlc
	rm -f *.a

client: libClient.a
	gcc main.c -I. -L. -L/usr/lib/x86_64-linux-gnu/ -o client.exe -DCLIENT -lClient -lvlc -lao -lm -L./
	rm -f *.a

libServer.a : ./lib/session.o ./lib/data.o ./lib/reqRep.o ./lib/audioStream.o protoServer.o
	ar -crs libServer.a ./lib/session.o ./lib/data.o ./lib/reqRep.o ./lib/audioStream.o protoServer.o 
	rm -f *.o

libClient.a : ./lib/session.o ./lib/data.o ./lib/reqRep.o ./lib/audioStream.o ./protoClient.o
	ar -crs libClient.a ./lib/session.o ./lib/data.o ./lib/reqRep.o protoClient.o ./lib/audioStream.o
	rm -f *.o

./lib/*.o: 
	cd lib && make

protoServer.o: protoServer.c protoServer.h
	gcc -c protoServer.c

protoClient.o: protoClient.c protoClient.h 
	gcc -c protoClient.c

testClient: libClient.a
	gcc testClient.c -o testClient.exe -lClient -lvlc  -L./ 
	rm -f *.a

testServer: libClient.a
	gcc testServer.c  -o testServer.exe -lClient -lvlc -L./ 
	rm -f *.a

clean:
	rm -f *.o 
	rm -f *.exe
	rm -f *.a
	cd lib && make clean