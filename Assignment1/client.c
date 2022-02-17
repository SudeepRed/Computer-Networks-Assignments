#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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
    while (1)
    {
        
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        
        char *c = "close";
        if (strncmp(c, buffer, sizeof(c)) == 0)
        {
            n = write(sockfd, buffer, strlen(buffer));
            break;
        }
        else
        {
            n = write(sockfd, buffer, strlen(buffer));
            bzero(buffer, 255);
            n = read(sockfd, buffer, 255);
            if (n < 0)
                error("read error");

            char file[25];
            char ser_no[15];
            int ptr = 0;
            char msg[256];
            int b = 0;
            for (int i = 0; i < strlen(buffer) + 1; i++)
            {
                if (buffer[i] == '$')
                {
                    ptr = i;
                    b = 1;
                }
                else if (buffer[i] == '#')
                    break;
                else if (b == 1)
                {
                    msg[i - ptr - 1] = buffer[i];
                }
                else
                {
                    ser_no[i] = buffer[i];
                }
            }

            int d;
            sscanf(ser_no, "%d", &d);
            sprintf(file, "Server%d.txt", d);
            FILE *f;
            f = fopen(file, "w");
            fprintf(f, "%s", msg);
            fclose(f);
            f = fopen(file, "r");
            bzero(buffer, sizeof(buffer));
            bzero(ser_no, sizeof(ser_no));
            int arr[5][3];
            int lines = 0;
            int i = 0;
            while (fgets(buffer, sizeof(buffer), f) != NULL)
            {
                lines++;
                if (lines % 4 == 1)
                {
                    sscanf(buffer, "%d", &arr[i][0]);
                }
                else if (lines % 4 == 3)
                {
                    sscanf(buffer, "%d", &arr[i][1]);
                }
                else if (lines % 4 == 0)
                {
                    sscanf(buffer, "%d", &arr[i][2]);
                    i++;
                }
                
            }

            int pid[3];
            pid[0] = arr[0][0];
            pid[1] = arr[0][1];
            pid[2] = arr[0][2];
            for (int i = 1; i < 5; i++)
            {
                if (pid[1] + pid[2] < arr[i][1] + arr[i][2])
                {
                    pid[0] = arr[i][0];
                    pid[1] = arr[i][1];
                    pid[2] = arr[i][2];
                }
            }
            fseek(f,0,SEEK_SET);
            bzero(buffer,sizeof(buffer));
            while (fgets(buffer, sizeof(buffer), f) != NULL)
            {
                lines++;
                long temp;
                if (lines % 4 == 1)
                {
                    
                    sscanf(buffer, "%ld", &temp);
                    
                    if (temp == pid[0])
                    {
                        bzero(ser_no,sizeof(ser_no));
                        fgets(ser_no, sizeof(ser_no), f);
                        ser_no[strcspn(ser_no, "\n")] = 0;
                        
                        break;
                    }
                }
            }
            sprintf(buffer, "Highest Consuming process PID-%d, P_NAME-%s, CPU_TIME-%d, KERNEL_TIME-%d\n", pid[0], ser_no, pid[1], pid[2]);
            
            printf("%s", buffer);
            n = write(sockfd, buffer, strlen(buffer));
            fclose(f);
            break;
        }
    }
    close(sockfd);
    return 0;
}