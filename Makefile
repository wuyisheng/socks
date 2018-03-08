all : main.o server.o test.o client.o
	g++ -o server main.o server.o -g -ggdb
	g++ -o test test.o client.o

main.o : main.cc server.h
	g++ -c main.cc

server.o : server.cc server.h
	g++ -c server.cc

test.o : test.cc client.h
	g++ -c test.cc

client.o: client.cc client.h
	g++ -c client.cc

clean:
	rm -f *.o *.so server test

