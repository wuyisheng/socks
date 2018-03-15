all : socks5server.o socks5.o epollwapper.o proxy.o client.o 
	g++ -o socks5server socks5server.o socks5.o epollwapper.o -g -ggdb
	g++ -o proxy proxy.o client.o

socks5server.o: socks5server.cc socks5server.h epollwapper.h socks5.h common.h
	g++ -c socks5server.cc

epollwapper.o: epollwapper.cc epollwapper.h common.h
	g++ -c epollwapper.cc

socks5.o:socks5.cc socks5.h common.h
	g++ -c socks5.cc

common.o: common.cc common.h
	g++ -c common.cc

proxy.o : proxy.cc client.h
	g++ -c proxy.cc

client.o: client.cc client.h
	g++ -c client.cc

clean:
	rm -f *.o *.so proxy socks5server

