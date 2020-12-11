all : client server

client : udpClient.c
	gcc -o UDPclient udpClient.c

server : udpServer.c
	gcc -o UDPserver udpServer.c

clean :
	rm UDPclient
	rm UDPserver