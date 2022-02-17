#define _GNU_SOURCE
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
#define PORTNUMBER 9000
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
long *readFile(int, char *, long);
void *handeFile(int, char *, char *);
void *writeFile(char *, int *);
long *readFile(int fd, char *file, long pid)
{
    char buffer[512];
    unsigned char c;
    int words = 1;
    int ptr = 0;
    char processname[255];
    //int pid;
    int stime;
    int utime;
    while (read(fd, &c, 1))
    {
        if (c == ' ')
        {
            if (words == 1)
            {
                sscanf(buffer, "%ld", &pid);
            }
            if (words == 2)
            {
                sscanf(buffer, "%s", processname);
            }
            if (words == 14)
            {
                sscanf(buffer, "%d", &stime);
            }
            if (words == 15)
            {
                sscanf(buffer, "%d", &utime);
            }
            if (words > 15)
            {
                break;
            }
            bzero(buffer, sizeof(buffer));
            ptr = 0;
            words++;
        }
        else
        {
            buffer[ptr] = c;
            ptr++;
        }
    }
    close(fd);
    static long arr[3];
    arr[0] = pid;
    arr[1] = stime;
    arr[2] = utime;
    return arr;
}
void *writeFile(char *msg, int *c_no)
{
    FILE *f;
    char *file = (char *)malloc(100 * sizeof(char));
    sprintf(file, "Client%d.txt", *c_no);
    f = fopen(file, "w");
    fprintf(f, "%s", msg);
    fclose(f);
    free(file);
}
void *handleFile(int fd, char *file, char *ret)
{

    char buffer[512];
    // static char ret[512];
    unsigned char c;
    int words = 1;
    int ptr = 0;
    char processname[20];
    int pid;
    int stime;
    int utime;
    while (read(fd, &c, 1))
    {
        if (c == ' ')
        {

            if (words == 1)
            {
                sscanf(buffer, "%d", &pid);
                char x[10];
                sprintf(x, "%d\n", pid);
                strcat(ret, x);
            }
            if (words == 2)
            {
                sscanf(buffer, "%s", processname);
                char x[25];
                sprintf(x, "%s\n", processname);
                strcat(ret, x);
            }
            if (words == 14)
            {
                sscanf(buffer, "%d", &stime);
                char x[10];
                sprintf(x, "%d\n", stime);
                strcat(ret, x);
            }
            if (words == 15)
            {
                sscanf(buffer, "%d", &utime);
                char x[10];
                sprintf(x, "%d\n", utime);
                strcat(ret, x);
            }
            if (words > 15)
            {
                break;
            }
            bzero(buffer, sizeof(buffer));
            ptr = 0;
            words++;
        }
        else
        {
            buffer[ptr] = c;
            ptr++;
        }
    }
    close(fd);
}
void doTheJob(int c_no)
{
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc/");
    long pid[5][3];
    for (int i = 0; i < 5; i++)
    {
        pid[i][0] = 0;
        pid[i][1] = 0;
        pid[i][2] = 0;
    }
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char dName[512];
            for (int i = 0; i < 50; i++)
                dName[i] = '&';
            strcpy(dName, dir->d_name);
            char *p;
            int s = 0;

            if (isdigit(dName[0]))
            {
                long temp_id;
                sscanf(dName, "%ld", &temp_id);
                //printf("%ld ",temp_id);
                char *file = (char *)malloc(512 * sizeof(char));
                sprintf(file, "/proc/%ld/stat", temp_id);
                int fd = open(file, O_RDONLY);
                if (fd)
                {
                    long *p = readFile(fd, file, temp_id);
                    long minTime = 10000;
                    for (int i = 0; i < 5; i++)
                    {

                        if (pid[i][2] + pid[i][1] < minTime)
                            minTime = pid[i][2] + pid[i][1];
                    }
                    long curTime = *(p + 1) + *(p + 2);
                    for (int i = 0; i < 5; i++)
                    {
                        if (curTime > minTime && minTime == pid[i][2] + pid[i][1])
                        {
                            pid[i][0] = temp_id;
                            pid[i][1] = *(p + 1);
                            pid[i][2] = *(p + 2);
                            break;
                        }
                    }
                }
            }

            bzero(dName, sizeof(dName));
        }
        closedir(d);
    }
    char buffer[512];
    bzero(buffer, 512);
    for (int i = 0; i < 5; i++)
    {
        char *file = (char *)malloc(100 * sizeof(char));
        sprintf(file, "/proc/%ld/stat", pid[i][0]);
        int fd = open(file, O_RDONLY);
        handleFile(fd, file, buffer);
        close(fd);
        free(file);
    }

    writeFile(buffer, &c_no);
}
void *handleConnection(void *pc)
{

    char buffer[512];
    int newsockfd = *((int *)pc + 0);
    int c_no = *((int *)pc + 1);
    //free(pc);
    while (1)
    {
        bzero(buffer,sizeof(buffer));
        char *c = "close";
        int n = read(newsockfd, buffer, sizeof(buffer));
        if (n)
        {
            if (strncmp(c, buffer, sizeof(c)) == 0)
            {
                break;
            }
            else
            {
                char s[1000];
                bzero(s, strlen(s));
                printf("%s", buffer);
                bzero(buffer, 512);
                doTheJob(c_no);
    
                sprintf(s, "%d$", gettid());
                FILE *f;
                sprintf(buffer, "Client%d.txt", c_no);
                f = fopen(buffer, "r");
                bzero(buffer, 512);
                fread(buffer, sizeof(buffer), 1, f);
                fclose(f);

                strcat(s, buffer);
                sprintf(buffer, "#");
                strcat(s, buffer);
                n = write(newsockfd, s, strlen(s));

                if (n < 0)
                {
                    error("ERROR writing to socket");
                    return NULL;
                }
                
            }
        }
    }
    return NULL;
}
int main(int argc, char *argv)
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
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len;
        client_len = sizeof(client_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (newsockfd < 0)
            printf("failed newsocket accept");
        c_no++;
        pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = newsockfd;
        int arr[2];
        arr[1] = c_no;
        arr[0] = newsockfd;

        pthread_create(&t, NULL, handleConnection, &arr);
    }

    close(sockfd);
    return 0;
}
