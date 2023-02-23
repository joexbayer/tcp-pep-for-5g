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

#include "../lib/include/library.h"


#define IP "127.0.0.1"
#define PORT 8182

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
    int server;
    int ret;
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);

    server = setup_socket(IP, PORT);

    /* Ping and print RTT */
    while(1)
    {
        char* test = "ping";
        ret = send(server, test, strlen(test), 0);

        ret = getsockopt(server, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
        printf("rtt: %u microseconds\n", info.tcpi_rtt);

        sleep(1);
    }


    close(server);
    return 0;
}

