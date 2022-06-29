#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6769
#define ADDRESS "127.0.0.1"
#define SIZE 1048576 // 1024 * 1024 * 100 = 100MB of data
#define MTU 1500

// void send_file(FILE *fp, int sock); // declaring this function for later use, no header file needed here

int main()
{
    // creating a TCP socket
    int sock;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    int rval = inet_pton(AF_INET, (const char *)ADDRESS, &server_address.sin_addr);
    if (rval <= 0)
    {
        perror("rvel");
        return -1;
    }

    // setting up buffer for CC algo switch later on
    char buf[256];
    socklen_t len;

    // defining file pointers
    FILE *fp;
    char *filename = "1mb.txt";
    char buffer[MTU];

    // recall socket is 'sock'
    int j, r;
    size_t n;
    for (r = 1; r <= 5; r++)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        int conn_status = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
        if (conn_status < 0)
        {
            perror("conn_status");
            printf("\n");
            exit(1);
        }
        len = sizeof(buf);
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
        {
            perror("getsockopt");
            return -1;
        }

        fp = fopen(filename, "r");
        bzero(buffer, sizeof(buffer));
        while ((n = fread(buffer, 1, sizeof buffer, fp)) > 0)
        {
            j = send(sock, buffer, sizeof(buffer), 0);
            if (j < 0)
            {
                perror("send");
                exit(1);
            }
        }
        if (ferror(fp))
        {
            perror("Error: ");
        }
        close(sock);
        fclose(fp);
    }

    // END of send segment

    // send_file(fp, sock);
    printf("Sent Data 5 times using cubic CC algorithm\n");
    printf("Switching To Reno\n");

    // Switching the CC algorithm to be Reno
    // The code bit for switching algorithms is courtesy of StackOverflow

    for (r = 1; r <= 5; r++)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        int conn_status = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
        if (conn_status < 0)
        {
            perror("conn_status");
            printf("\n");
            exit(1);
        }
        strcpy(buf, "reno");
        len = strlen(buf);
        if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
        {
            perror("setsockopt");
            printf("\n");

            return -1;
        }
        len = sizeof(buf);
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
        {
            perror("getsockopt");
            printf("\n");

            return -1;
        }
        fp = fopen(filename, "r");
        bzero(buffer, sizeof(buffer));
        while ((n = fread(buffer, 1, sizeof buffer, fp)) > 0)
        {
            j = send(sock, buffer, sizeof(buffer), 0);
            if (j < 0)
            {
                perror("send");
                exit(1);
            }
        }
        if (ferror(fp))
        {
            perror("Error: ");
        }
        close(sock);
        fclose(fp);
    }
    printf("Sent Data 5 times using reno CC algorithm\n");
    printf("Closing..\n");

    // Closing the socket
    return 0;
}