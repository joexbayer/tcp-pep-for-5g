#include <string.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <stdlib.h>

#include "../lib/include/library.h"


#define IP "127.0.0.1"
#define PORT 8183
#define MAX_BUFFER_SIZE 1001

int setup_socket(char* ip, unsigned short port)
{
    int server;
    struct sockaddr_in s_in;
    bzero((char *)&s_in, sizeof(s_in));
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr(IP);
    s_in.sin_port = htons(PORT);

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //int flags = 1;
    //setsockopt(server, SOL_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));

    #if 0
        int ret = connect(server, (struct sockaddr*) &s_in, sizeof(s_in));
    #else
        int ret = pep_connect(server, (struct sockaddr*) &s_in, sizeof(s_in), PEP_NONINTERACTIVE);
    #endif

    if(ret >= 0)
        printf("Connected. %d\n", ret);
    else {
        printf("Unable to connect %d.\n", ret);
        return -1;
    }

    return server;
}

int main(int argc, char * argv[])
{
    int server, ret, thesis_size, read;
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);
    /* Open big file. */
    FILE* thesis = fopen("thesis/uio-master.pdf", "r");
    char buffer[MAX_BUFFER_SIZE];

    if(thesis == NULL){
        printf("Could not open file\n");
        return -1;
    }

    server = setup_socket(IP, PORT);

    fseek(thesis, 0L, SEEK_END);
    thesis_size = ftell(thesis);
    rewind(thesis);

    printf("sending file of size %d bytes\n", thesis_size);
    
    /* Ping and print RTT */
    while(thesis_size > 0)
    {
        read = fread(buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 1, thesis);
        printf("reading %d bytes from file\n", MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE);
        ret = send(server, buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 0);
        printf("sending %d bytes to servers\n", ret);
        thesis_size -= ret;

        ret = getsockopt(server, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
        printf("rtt: %u microseconds\n", info.tcpi_rtt);
    }

    close(server);
    return 0;
}

