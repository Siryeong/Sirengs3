#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>


#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv){
    int serv_sock;
    char same[BUFSIZE];
    char message[BUFSIZE];
    int str_len, num=0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    //Create socket, "SOCK_DGRAM" == TCP
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    if(serv_sock == -1) error_handling("UDP 소켓생성오류\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    //IPv4 Setting
    serv_addr.sin_family = AF_INET;
    //Server IP address setting
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Server port number setting
    serv_addr.sin_port=htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr))==-1) 
        error_handling("bind() error\n");
    int cnt = 0;
    while(1){
    printf("nth input : %d \n", cnt);
        clnt_addr_size=sizeof(clnt_addr);
        str_len = recvfrom(serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        message[str_len] = 0;

        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));

        int fp = open(message, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if(fp == -1)
            fprintf(stderr, " 파일을 open도중 오류 발생: %s\n", strerror(errno));

        while(1)
        { 
            clnt_addr_size=sizeof(clnt_addr);
            if((str_len = recvfrom(serv_sock, message, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size)) <= 1){
                sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
                write(fp, message, str_len);
                break;
            }
            message[str_len] = 0;

            sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
            write(fp, message, str_len);
        }

        printf("Done!\n");
        cnt += 1;
        close(fp);
    }
    
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr); 
    fputc('\n', stderr); 
    exit(1);
}