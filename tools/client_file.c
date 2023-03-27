#include <string.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "../lib/include/library.h"


#define IP "192.168.2.22"
#define PORT 8183
#define MAX_BUFFER_SIZE 1001
#define PEP 0
#define TEST_FILE "6mb.bin"

int server;

int setup_socket(char* ip, unsigned short port)
{
    int sd, ret;
    struct sockaddr_in s_in;
    bzero((char *)&s_in, sizeof(s_in));
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr(IP);
    s_in.sin_port = htons(PORT);

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#if PEP
        
        ret = pep_connect(sd, (struct sockaddr*) &s_in, sizeof(s_in), PEP_NONINTERACTIVE);
#else
        ret = connect(sd, (struct sockaddr*) &s_in, sizeof(s_in));
#endif

    if(ret < 0){
        printf("Unable to connect %d.\n", ret); 
        return -1;
    }
    printf("Connected. %d\n", ret);
    return sd;
}

void intHandler(int dummy) {
    close(server);
    exit(0);
}

int main(int argc, char * argv[])
{
    int ret, thesis_size, read;
    struct tcp_info info;
    struct timeval  tv1, tv2;
    socklen_t tcp_info_length = sizeof(info);
    signal(SIGINT, intHandler);
    /* Open big file. */
    FILE* thesis = fopen(TEST_FILE, "r");
    char buffer[MAX_BUFFER_SIZE];

    if(thesis == NULL){
        printf("Could not open file\n");
        return -1;
    }

    server = setup_socket(IP, PORT);
    if(server < 0)
        return -1;

    fseek(thesis, 0L, SEEK_END);
    thesis_size = ftell(thesis);
    rewind(thesis);

    printf("sending file of size %d bytes\n", thesis_size);
    gettimeofday(&tv1, NULL);
    /* Ping and print RTT */
    while(thesis_size > 0)
    {
        read = fread(buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 1, thesis);
        //printf("reading %d bytes from file\n", MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE);
        ret = send(server, buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 0);
        //printf("sending %d bytes to servers (%d left)\n", ret, thesis_size);
        thesis_size -= ret;

        //ret = getsockopt(server, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
        //printf("rtt: %f ms\n", info.tcpi_rtt/1000);
    }
    gettimeofday(&tv2, NULL);
    printf("Done\n");
    printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));

    close(server);
    return 0;
}

