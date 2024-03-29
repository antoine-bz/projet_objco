serveur: 
	cd lib && make libServer.a
	gcc  main.c -DSERVEUR -o serveur.exe -lServer -L./lib -lvlc
	rm -f *.a

client:
	cd lib && make libClient.a
	gcc main.c -o client.exe -DCLIENT -lClient -L./lib  -lvlc 
	rm -f *.a

testClient:
	cd lib && make libClient.a
	gcc testClient.c -o testClient.exe -lClient -lvlc  -L./lib 
	rm -f *.a

testServer:
	cd lib && make libServer.a
	gcc testServer.c  -o testServer.exe -lClient -lvlc -L./lib
	rm -f *.a



#MAKEFILE FOR LIB RPI meme version qu'au dessus mais CCC

PATH_PWD=$(CURDIR)
PATH_CC=$(PATH_PWD)/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
CCC=$(PATH_CC)/arm-linux-gnueabihf-gcc


serveurCCC: 
	cd lib && make libServer.a CCC=$(CCC)
	$(CCC)  main.c -DSERVEUR -o serveurCCC.exe -lServer -L./lib -lvlc -lrt 
	rm -f *.a

clientCCC: 
	cd lib && make libClient.a CCC=$(CCC)
	$(CCC) main.c -o clientCCC.exe -DCLIENT -lClient -L./lib -lvlc 
	rm -f *.a


serveurJOYPI:
	cd lib && make libServer.a CCC=$(CCC) CFLAGS="-DJOYPI"
	$(CCC) main.c -DSERVEUR -o serveurJOYPI.exe -lServer -L./lib -lvlc
	rm -f *.a


clean:
	rm -f *.o 
	rm -f *.exe
	rm -f *.a
	cd lib && make clean
