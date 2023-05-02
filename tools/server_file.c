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
#include <time.h>
#include <sys/time.h>

#define PORT 8183
#define BUFFER_SIZE 512
#define OUTPUT_FILE "test.out"
#define LOG_FILE_NAME "logs/server.log"

static FILE* log_file;

int main(void)
{
    int sd, cd;
    socklen_t size;     
    unsigned char byte;
    struct sockaddr_in s_ain, c_ain;    
    int qlen = 5;
    int ret;
    char buffer[BUFFER_SIZE];
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);
    int total_recv = 0;
    struct timeval  tv1, tv2;

    FILE* thesis = fopen(OUTPUT_FILE, "w+");
    if(thesis == NULL){
        printf("[FILE] Unable to open output file\n");
        return -1;
    }

    /* Logging */
    log_file = fopen(LOG_FILE_NAME, "a");
    if(log_file == NULL){
        printf("Could not open log file!\n");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));

    bzero((char *)&s_ain, sizeof(s_ain));
    s_ain.sin_family = AF_INET;
    s_ain.sin_addr.s_addr = INADDR_ANY;
    s_ain.sin_port = htons(PORT);

    if(bind(sd, (struct sockaddr *)&s_ain, sizeof(s_ain)) == -1) return -1;
    if(listen(sd, 5) == -1) return -1;

    size = sizeof(c_ain);
    cd = accept(sd, (struct sockaddr *)&c_ain, &size);

    printf("[FILE] Client Connected\n");
    gettimeofday(&tv1, NULL);
    while (1)   
    {
        ret = read(cd, buffer, BUFFER_SIZE);  
        fwrite(buffer, BUFFER_SIZE, 1, thesis);
        if(ret > 0){
            //getsockopt(cd, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
            total_recv += ret;
            //printf("[FILE] Client: %d/%d (rtt: %f ms)\n", ret,total_recv, info.tcpi_rtt/1000);
        } else if (ret <= 0){
            break;
        }
    }
    gettimeofday(&tv2, NULL);
    printf("[FILE] Client Disconnected: %d bytes received.\n", total_recv);
    
    double total_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
    printf ("Total time = %f seconds\n", total_time);

    fprintf(log_file, "LOG: %d - (%s) %fs\n", total_recv, c_ain.sin_addr.s_addr == 184658112 ? "NOPEP" : "PEP", total_time);

    
    fclose(thesis);
    close(cd);
    close(sd);
}
