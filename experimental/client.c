#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> // for close
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
    int server;
    struct sockaddr_in s_in;
    unsigned char byte;

    bzero((char *)&s_in, sizeof(s_in));
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr("127.0.0.1");
    s_in.sin_port = htons(8181);

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( connect(server, (struct sockaddr*) &s_in, sizeof(s_in)) == -1 ) {
        return -1;
    }       

    printf("Connected.\n");
    close(server);
    return 0;
}

