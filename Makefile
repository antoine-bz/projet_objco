serveur: libServer.a 
	gcc  main.c -DSERVEUR -o serveur -lServer -L./
	rm -f *.a

client: libClient.a
	gcc main.c -I. -L. -L/usr/lib/x86_64-linux-gnu/ -o client -DCLIENT -lClient -lmpg123 -lao -lm -L./
	rm -f *.a

libServer.a : ./lib/session.o ./lib/data.o ./lib/reqRep.o protoServer.o 
	ar -crs libServer.a ./lib/session.o ./lib/data.o ./lib/reqRep.o protoServer.o 
	rm -f *.o

libClient.a : ./lib/session.o ./lib/data.o ./lib/reqRep.o ./lib/protoClient.o
	ar -crs libClient.a ./lib/session.o ./lib/data.o ./lib/reqRep.o protoClient.o
	rm -f *.o

protoServer.o: protoServer.c protoServer.h
	gcc -c protoServer.c

protoClient.o: protoClient.c protoClient.h
	gcc -c protoClient.c

clean:
	rm -f *.o
	rm -f serveur
	rm -f client
	rm -f *.a