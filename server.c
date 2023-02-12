#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netdb.h>

int main(void)
{
    int sd, cd;
    socklen_t size;     
    unsigned char byte;
    struct sockaddr_in s_ain, c_ain;    
    int qlen = 5;
    int ret;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    setsockopt(sd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));

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

    char buffer[34];

    while(1) {
        size = sizeof(c_ain);
        cd = accept(sd, (struct sockaddr *)&c_ain, &size);
        printf("Client Connected\n");

        ret = recv(cd, buffer, 34, 0);  
        if(ret > 0)
            printf("Client: %s\n", buffer);
        
        close(cd);
    }
}
