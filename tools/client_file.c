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
#include <time.h>
#include <stdarg.h>

#include "../lib/include/library.h"

#define IP "172.16.11.5"
#define PORT 8183
#define MAX_BUFFER_SIZE 1500
#define TEST_FILE "32mb.bin"
#define LOG_FILE_NAME "logs/client.log"
#define MS "100ms"

static int PEP = 0;

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
socketfd_t server;

void parse_opts(int argc, char* argv[])
{
    int option;
    while ((option = getopt(argc, argv, "p")) != -1) {
        switch (option) {
            case 'p':
                PEP = 1;
                break;
            case ':':
                printf("Missing option: %c\n", optopt);
                exit(-1);
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
            default:
                // Should not reach here
                break;
        }
    }
}

socketfd_t setup_socket(char* ip, unsigned short port)
{
    socketfd_t sd, ret;
    struct sockaddr_in s_in;
    bzero((char *)&s_in, sizeof(s_in));
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr(IP);
    s_in.sin_port = htons(PORT);

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(PEP){
        ret = pep_connect(sd, (struct sockaddr*) &s_in, sizeof(s_in), PEP_NONINTERACTIVE);
    } else {
        ret = connect(sd, (struct sockaddr*) &s_in, sizeof(s_in));
    }

    if(ret < 0){
        printf("Unable to connect %d.\n", ret); 
        return -1;
    }
    printf("Connected. %s\n", ret == 0 ? "[NO PEP]" : "[PEP]");
    return sd;
}

void int_handlr(int dummy) {
    close(server);
    fclose(log_file);
    fclose(thesis);
    exit(0);
}

int main(int argc, char * argv[])
{
    int ret, thesis_size, to_send, read;
    struct tcp_info info;
    struct timeval  tv1, tv2;
    socklen_t tcp_info_length = sizeof(info);
    char buffer[MAX_BUFFER_SIZE];

    parse_opts(argc, argv);

    signal(SIGINT, int_handlr);
    
    /* Open big file. */
    thesis = fopen(TEST_FILE, "r");
    if(thesis == NULL){
        printf("Could not open file\n");
        return -1;
    }

    /* Logging */
    log_file = fopen(LOG_FILE_NAME, "a");
    if(log_file == NULL){
        printf("Could not open log file!\n");
        return -1;
    }

    /* Server socket */
    server = setup_socket(IP, PORT);
    if(server < 0)
        return -1;

    /* File size */
    fseek(thesis, 0L, SEEK_END);
    //thesis_size = ftell(thesis);
    thesis_size = 32*1024*1024;
    to_send = thesis_size;
    rewind(thesis);

    printf("Sending file of size %d bytes\n", thesis_size);
    
    gettimeofday(&tv1, NULL);
    while(thesis_size > 0)
    {
        //ret = fread(buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 1, thesis);
        //printf("reading %d bytes from file\n", MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE);
        ret = send(server, buffer, MAX_BUFFER_SIZE > thesis_size ? thesis_size : MAX_BUFFER_SIZE, 0);
        //printf("sending %d bytes to servers (%d left)\n", ret, thesis_size);
        thesis_size -= ret;
    }
    gettimeofday(&tv2, NULL);

    double total_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
    LOG("%.1fmb %fs (%s)\n", (double)(to_send/1024/1024), total_time, PEP ? "PEP" : "NOPEP");

    close(server);
    fclose(thesis);
    fclose(log_file);
    return 0;
}

