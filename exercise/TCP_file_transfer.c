#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv){
    int serv_sock;
    int clnt_sock;
    char message[BUFSIZE];
    int str_len, num=0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    //Create socket, "SOCK_STREAM" == TCP
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    if(serv_sock == -1) error_handling("TCP 소켓생성오류");

    memset(&serv_addr, 0, sizeof(serv_addr));
    //IPv4 Setting
    serv_addr.sin_family = AF_INET;
    //Server IP address setting
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Server port number setting
    serv_addr.sin_port=htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr))==-1) 
        error_handling("bind() error");
    //Waiting for connection

    if(listen(serv_sock, 5)==-1) 
        error_handling("listen() error");
    
    clnt_addr_size=sizeof(clnt_addr);

    //Accepting connection
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size); 
    if(clnt_sock==-1)
        error_handling("accept() error");

    //File reading
    str_len = read(clnt_sock, message, BUFSIZE);
    char name[BUFSIZE] = str_len;
    fflush(message);
    
    FILE *fp = fopen(name, "wb");


    if(!fp)
        printf("fopen error!");

    while(1) 
    {
        str_len = read(clnt_sock, message, BUFSIZE);
        fwrite(message, sizeof(char), len(message), fp);

    }
    fclose(fp);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr); 
    fputc('\n', stderr); 
    exit(1);
}