all : main.o server.o proxy.o client.o
	g++ -o server main.o server.o -g -ggdb
	g++ -o proxy proxy.o client.o

main.o : main.cc server.h
	g++ -c main.cc

server.o : server.cc server.h
	g++ -c server.cc

proxy.o : proxy.cc client.h
	g++ -c proxy.cc

client.o: client.cc client.h
	g++ -c client.cc

clean:
	rm -f *.o *.so server proxy

