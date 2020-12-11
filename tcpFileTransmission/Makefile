all: client server

client : tcpClient.c
	gcc -o TCPclient tcpClient.c

server : tcpServer.c
	gcc -o TCPserver tcpServer.c

clean:
	rm TCPclient
	rm TCPserver
