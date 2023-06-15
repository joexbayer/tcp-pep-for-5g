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
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

#include <signal.h>

#define PORT 8183
#define BUFFER_SIZE 1024
#define OUTPUT_FILE "test.out"
#define LOG_FILE_NAME "logs/server.log"

#define LOG(format, ...) do { \
    char timestamp[20]; \
    time_t currentTime = time(NULL); \
    struct tm *localTime = localtime(&currentTime); \
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime); \
    fprintf(log_file, "LOG [%s] ", timestamp); \
    fprintf(log_file, format, ##__VA_ARGS__); \
    printf("LOG [%s] ", timestamp); \
    printf(format, ##__VA_ARGS__); \
} while(0)

typedef int socketfd_t;

static FILE* log_file;
FILE* thesis;
socketfd_t sd, cd;

void int_handlr(int dummy) {
    close(sd);
    close(cd);
    fclose(log_file);
    fclose(thesis);
    exit(0);
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    struct timeval  tv1, tv2;
    char buffer[BUFFER_SIZE];
    int ret, total_recv = 0;

    gettimeofday(&tv1, NULL);
    while (1)   
    {
        ret = read(sock, buffer, BUFFER_SIZE);
        if(ret > 0){
            //getsockopt(cd, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
            total_recv += ret;
            //printf("[FILE] Client: %d/%d (rtt: %f ms)\n", ret,total_recv, info.tcpi_rtt/1000);
        } else if (ret <= 0){
            break;
        }
    }
    gettimeofday(&tv2, NULL);
    //printf("[FILE] Client Disconnected: %d bytes received.\n", total_recv);
    close(sock);
    
    double total_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);

    LOG("%.1fmb - %fs\n", (double)(total_recv/1024/1024), total_time);
    fflush(log_file);
}


int main(void)
{
    socklen_t size;     
    unsigned char byte;
    struct sockaddr_in s_ain;    
    int qlen = 5;
    int ret;
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);
    int total_recv = 0;
    pthread_t thread_id;

    signal(SIGINT, int_handlr);

    /* Open file to write */
    thesis = fopen(OUTPUT_FILE, "w+");
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

    while (1)
    {
        struct sockaddr_in c_ain;   
        size = sizeof(c_ain);
        int cd = accept(sd, (struct sockaddr *)&c_ain, &size);
        total_recv = 0;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &cd) < 0)
        {
            perror("could not create thread");
            exit(1);
        }
    }

    
    close(sd);
    close(cd);
    fclose(log_file);
    fclose(thesis);

    return 0;
}
