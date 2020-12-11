#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

char buf[BUFSIZE];
char header[8];

void error_handling(char *message);

int main (int argc, char ** argv)
{
    int sock, inputSteam;
    int str_len;
    struct sockaddr_in serv_addr;
    if(argc!=4){
        printf("Usage : %s <IP> <port> <file name>\n", argv[0]);
        exit(1);
    }
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");

    inputSteam = open(argv[3], O_RDONLY);
    if(inputSteam == -1)
        error_handling("non existing file");

    sprintf(header, "%08d", strlen(argv[3]));
    //printf("header : %s\n", header);
    write(sock, header, 8);
    write(sock, argv[3], strlen(argv[3]));
    
    while(str_len = read(inputSteam, buf, BUFSIZE)){
        write(sock, buf, str_len);
    } printf("sended\n");
    
    close(sock);
    close(inputSteam);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}