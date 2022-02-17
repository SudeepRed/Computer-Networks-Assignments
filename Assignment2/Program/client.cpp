#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<string>
#include <csignal>
# include<time.h>
#include<iostream>
using namespace std;
sig_atomic_t volatile done = 0;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
    
    if (argc < 2)
    {
        fprintf(stderr, "Error");
        exit(1);
    }
    int sockfd, port_number, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[255];
    if (argc < 3)
    {
        fprintf(stderr, "eeeeeeee");
        exit(0);
    }
    port_number = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR op socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error no host");
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // //htons host to netwrk short
    serv_addr.sin_port = htons(port_number);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("connet failed");
    }
    bzero(buffer, 255);
    //Record Start time
    time_t startTime = time(0);
    long times = 120;
    // Checking if 2mins(or 120 sec) are over
    while((time(0)-startTime)<=times){
        bzero(buffer,255);
        char s[]="Hello! How are U?\n";
        write(sockfd, s, strlen(s));
        read(sockfd,buffer,sizeof(buffer));
        // cout<<buffer;
        
    }
    close(sockfd);
    return 0;
}