#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <iostream>
#include <time.h>
#define PORTNUMBER 9000

using namespace std;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main()
{

    int sockfd, n;
    char buffer[255];
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //socket function failure
    if (sockfd < 0)
    {
        error("Error opening socket");
    }
    //clearing
    bzero((char *)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //htons host to netwrk short
    server_addr.sin_port = htons(PORTNUMBER);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("biniding fail");
    }
    if (listen(sockfd, 5) < 0)
        error("listen fail");
    int c_no = 0;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    client_len = sizeof(client_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (newsockfd < 0)
        printf("failed newsocket accept");
    //Recording the start time
    time_t timestart=time(0);
    long times=120;
    // sending messages for 2mins
    while((time(0)-timestart)<=times){
        bzero(buffer,255);
        char s[]="From Server:Iam fine bruh!\n";
        read(newsockfd, buffer, 255);
        // cout<<buffer;
        write(newsockfd, s, strlen(s));
        
        
    }
    close(sockfd);
    return 0;
}