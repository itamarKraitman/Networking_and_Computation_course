#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h> // gettimeofday()

#define ICMP_ECHO 8

 // IPv4 header len without options
#define IP4_HDRLEN 20

// ICMP header len for echo req
#define ICMP_HDRLEN 8 

// Checksum algo
unsigned short calculate_checksum(unsigned short * paddress, int len);

#define SOURCE_IP "127.0.0.1"
// i.e the gateway or ping to google.com for their ip-address
#define DESTINATION_IP "172.217.21.14"

int main ()
{
/*********************build the ICMP header***********/
    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "This is the ping.\n";

    int datalen = strlen(data) + 1;

 /*********************massege************************/

    // Message Type (8 bits): ICMP_ECHO_REQUEST
    icmphdr.icmp_type = ICMP_ECHO;

    // Message Code (8 bits): echo request
    icmphdr.icmp_code = 0;

    // Identifier (16 bits): some number to trace the response.
    // It will be copied to the response packet and used to map response to the request sent earlier.
    // Thus, it serves as a Transaction-ID when we need to make "ping"
    icmphdr.icmp_id = 18; // hai

    // Sequence Number (16 bits): starts at 0
    icmphdr.icmp_seq = 0;

    // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
    icmphdr.icmp_cksum = 0;

    // Combine the packet 
    char packet[IP_MAXPACKET];

    // First, IP header.
    memcpy(packet, &icmphdr, ICMP_HDRLEN);

    // After ICMP header, add the ICMP data.
    memcpy (packet + IP4_HDRLEN + ICMP_HDRLEN, data, datalen);

    // Calculate the ICMP header checksum
    ((struct icmp*) packet) -> icmp_cksum =calculate_checksum((unsigned short *) packet, ICMP_HDRLEN + datalen);

    struct sockaddr_in dest_in;
    bzero(&dest_in, sizeof (struct sockaddr_in));
    dest_in.sin_family = AF_INET;

    dest_in.sin_addr.s_addr = inet_addr(DESTINATION_IP);
    int sock = -1;
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) 
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        return 1;
    }

/***************send the ICMP request******************/
int sent =-1;
struct timeval start, end;
gettimeofday(&start, NULL);

sent = sendto(sock,packet,ICMP_HDRLEN+datalen,0,(struct sockaddr *) &dest_in,sizeof(dest_in));
if (sent == -1)
{
    fprintf(stderr,"the sending has faild");
}
printf("sent a packet:\n\tsize: %d bytes\n\tdata: %s\n", sent, data);

/********** Receive the ICMP ECHO REPLY packet: *******/
bzero(&packet, sizeof(packet));
int recvied = -1;
socklen_t length = sizeof(dest_in);
while(recvied < 0) {
    recvied = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *) &dest_in, &length);
}
printf("recived:\n\tsize: %d bytes\n\tdata: %s\n\n", recvied, packet);

/************************* RTT ***********************/
gettimeofday(&end, NULL);
float milisecondes=((end.tv_sec*1000)-(start.tv_sec*1000))+((end.tv_usec/1000)-(start.tv_usec/1000));
unsigned long microseconds= ((end.tv_sec*1000000)-(start.tv_sec*1000000))+((end.tv_usec)-(start.tv_usec));
printf("RTT time in miliseconds: %f\n", milisecondes);
printf("RTT time in microseconds: %ld\n", microseconds);

/*******************close thr socket*****************/

  close(sock);
  return 0;
}

// Compute checksum (RFC 1071).
/**************************** Compute checksum (RFC 1071) ****************************/
unsigned short calculate_checksum(unsigned short * paddress, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short * w = paddress;
	unsigned short answer = 0;

	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1)
	{
		*((unsigned char *)&answer) = *((unsigned char *)w);
		sum += answer;
	}

	// add back carry outs from top 16 bits to low 16 bits
	sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
	sum += (sum >> 16);                 // add carry
	answer = ~sum;                      // truncate to 16 bits
  
	return answer;
}


