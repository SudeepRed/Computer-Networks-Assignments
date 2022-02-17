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

long *readFile(int, char *, long);
void handeFile(int, char *, int);
void writeFile(char *, int, char[], int, int);
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
void writeFile(char *file, int pid, char p_name[], int stime, int utime)
{
    FILE *f;
    bzero(file, sizeof(file));
    sprintf(file, "%d.txt", pid);
    f = fopen(file, "w");

    fprintf(f, "pid\t%d\nProcessName\t%s\nUserTime\t%d\nKernelTime\t%d\n", pid, p_name, stime, utime);
    fclose(f);
}
void handleFile(int fd, char *file, int pid)
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
                sscanf(buffer, "%d", &pid);
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
    writeFile(file, pid, processname, stime, utime);
}
void doTheJob(){
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc/");
    long pid[5][3];
    for (int i = 0; i < 5; i++)
    {
        pid[i][0]=0;
        pid[i][1]=0;
        pid[i][2]=0;
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

    for (int i = 0; i < 5; i++)
    {
        char *file = (char *)malloc(100 * sizeof(char));
        sprintf(file, "/proc/%ld/stat", pid[i][0]);
        int fd = open(file, O_RDONLY);
        handleFile(fd,file,pid[i][0]);
        
    }

}
int main(int argc, char const *argv[])
{
    doTheJob();
    
    return 0;
}
