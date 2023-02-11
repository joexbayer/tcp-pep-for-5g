#include <string.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "lib/include/library.h"

int main(int argc, char * argv[])
{
    int server;
    struct sockaddr_in s_in;
    unsigned char byte;
    int ret;

    bzero((char *)&s_in, sizeof(s_in));
    s_in.sin_family = AF_INET;
    /* both ip and port are for actual endpoint. */
    s_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    s_in.sin_port = htons(8182);

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //connect(server, (struct sockaddr*) &s_in, sizeof(s_in));

    int flags = 1;
    setsockopt(server, SOL_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));
    ret = pep_connect(server, (struct sockaddr*) &s_in, sizeof(s_in), PEP_NONINTERACTIVE);
    if(ret > 0)
        printf("Connected. %d\n", ret);

    char* test = "PEP test string sent from client!!";

    ret = send(server, test, strlen(test), 0);
    {
        printf("Success!\n");
    }

    close(server);
    return 0;
}

