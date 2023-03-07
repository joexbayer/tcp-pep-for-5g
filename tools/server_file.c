#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdlib.h>

#define PORT 8183

int main(void)
{
    int sd, cd;
    socklen_t size;     
    unsigned char byte;
    struct sockaddr_in s_ain, c_ain;    
    int qlen = 5;
    int ret;
    char buffer[512];
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);
    int total_recv = 0;
    FILE* thesis = fopen("thesis.pdf", "w+");

    if(thesis == NULL){
        printf("[FILE] Unable to open output file\n");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    setsockopt(sd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));

    bzero((char *)&s_ain, sizeof(s_ain));
    s_ain.sin_family = AF_INET;
    s_ain.sin_addr.s_addr = INADDR_ANY;
    s_ain.sin_port = htons(PORT);

    if(bind(sd, (struct sockaddr *)&s_ain, sizeof(s_ain)) == -1) {
        return -1;
    }

    if(listen(sd, 5) == -1) {
        return -1;
    }

    size = sizeof(c_ain);
    cd = accept(sd, (struct sockaddr *)&c_ain, &size);

    printf("[FILE] Client Connected\n");
    while (1)   
    {
        ret = read(cd, buffer, 512);  
        fwrite(buffer, 512, 1, thesis);
        if(ret > 0){
            getsockopt(cd, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
            total_recv += ret;
            printf("[FILE] Client: %d/%d (rtt: %u microseconds)\n", ret,total_recv, info.tcpi_rtt);
        } else if (ret <= 0){
            break;
        }
    }
    printf("[FILE] Client Disconnected: %d bytes received.\n", total_recv);
    
    fclose(thesis);
    close(cd);
    close(sd);
}
