#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#define PORT 6769
#define ADDRESS "127.0.0.1"
#define SIZE 1048576
#define MTU 1024

int main()
{
    // Setting up variables for later use
    int conn_status;
    int sock, sock_recv;
    struct sockaddr_in server_addr;
    socklen_t length;
    double average_time_cubic;
    double average_time_reno;

    char buf[256]; // for CC algorithm change
    char buffer[MTU];

    struct timeval start, end;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
    }

    // Default CC algorithm is Cubic, we just make sure this is the correct now
    // So there will be no issues with recieving from sender.

    length = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    length = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("Congestion Control Strategy: %s\n", buf);

    // Setting server_addr memory to 0 in order to make sure we open a fresh port without overlapping
    // any existing data
    memset(&server_addr, 0, sizeof(server_addr));

    // Setting up socket data
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    conn_status = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn_status < 0)
    {
        perror("bind");
        printf("\n");
        close(sock);
        exit(1);
    }

    /*
    Next block of code is setting up variables to measure time as requested in the assignment,
    We will be using the <sys/time.h> library to do all the required calculations.
    Afterwards, we will switch to Reno CC algorithm and take the same measurements.
    */
    int e;
    int recieved = 0;

    for (int i = 0; i < 5; i++)
    {
        e = listen(sock, 10);
        if (e < 0)
        {
            perror("listen");
        }
        sock_recv = accept(sock, NULL, NULL);
        if (sock_recv < 0)
        {
            perror("accept");
            exit(1);
        }
        int n = 0;
        gettimeofday(&start, 0);

        while ((n = recv(sock_recv, &buffer, sizeof(buffer), 0)) > 0)
        {
            recieved += n;
            if (recieved == SIZE * 30)
            {
                break;
            }
        }
        gettimeofday(&end, 0);
        average_time_cubic += (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1e6);

        bzero(buffer, MTU);
        close(sock_recv);
    }
    printf("Recieved message 5 times, switching CC algorithm..\n");
    // Changing CC algorithm to Reno

    strcpy(buf, "reno");
    length = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, length) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    length = sizeof(buf);

    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("New Congestion Control Strategy: %s\n", buf);

    // Same logic as above for measuring average time, this time for Reno CC algorithm
    for (int i = 0; i < 5; i++)
    {
        e = listen(sock, 10);
        if (e < 0)
        {
            perror("listen");
        }
        sock_recv = accept(sock, NULL, NULL);
        if (sock_recv < 0)
        {
            perror("accept");
            exit(1);
        }
        int n = 0;
        int recieved = 0;
        gettimeofday(&start, 0);
        while ((n = recv(sock_recv, &buffer, sizeof(buffer), 0)) > 0)
        {
            recieved += n;
            if (recieved == SIZE * 30)
            {
                break;
            }
        }
        gettimeofday(&end, 0);
        average_time_reno += (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1e6);
        bzero(buffer, MTU);
        close(sock_recv);
    }
    printf("Recieved message 5 times using Reno, calculating average delivery time\n");
    printf("=== Summery Of Average Time: ===\n");
    printf("Cubic CC algorithm: %f seconds\n", average_time_cubic / 5);
    printf("Reno CC algorithm: %f seconds\n", average_time_reno / 5);
    close(sock);
    return 0;
}