#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100

int sock, inputSteam;
char message[BUFSIZE];
int str_len, addr_size, i;
struct sockaddr_in serv_addr;
struct sockaddr_in from_addr;

void error_handling(char *message);

void handler(int sig)
{
    sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	alarm(3);
}

int main(int argc, char **argv)
{

    signal(SIGALRM, handler) ;

    if(argc!=4){
        printf("Usage : %s <IP> <port> <file name>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("UDP 소켓 생성 오류");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");

    inputSteam = open(argv[3], O_RDONLY);
    if(inputSteam == -1)
        error_handling("non existing file");

    alarm(1);
    strcpy(message, argv[3]);
    sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    addr_size = sizeof(from_addr);
    str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size);
    alarm(0);
    message[str_len]=0;

    while(str_len = read(inputSteam, message, BUFSIZE))
    {
        sendto(sock, message, str_len, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        //str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size);
        
    } printf("sended...\n");

    sendto(sock, "", 0, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &addr_size);
    //printf("rev2 : %s\n", message);

    close(inputSteam);
    close(sock);

    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}