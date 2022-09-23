#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
    int sd, cd;
    socklen_t size;     
    unsigned char byte;
    struct sockaddr_in s_ain, c_ain;    

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bzero((char *)&s_ain, sizeof(s_ain));
    s_ain.sin_family = AF_INET;
    s_ain.sin_addr.s_addr = INADDR_ANY;
    s_ain.sin_port = htons(8182);

    if(bind(sd, (struct sockaddr *)&s_ain, sizeof(s_ain)) == -1) {
        return -1;
    }

    if(listen(sd, 5) == -1) {
        return -1;
    }

    while(1) {
        size = sizeof(c_ain);
        cd = accept(sd, (struct sockaddr *)&c_ain, &size);
        printf("Client Connected\n");
    }
}
