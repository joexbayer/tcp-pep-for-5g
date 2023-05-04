#include <string.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> /* for close */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdlib.h>

#include <math.h>

#include "../lib/include/library.h"

#define IP "192.168.2.22" /* IP of server */
#define PORT 8182
#define USE_PEP 0

int server;
int mixed;
int samples = 2000;
int interval = 10;

static double rtts[2000];
static int total_pings = 0;

int setup_socket(char* ip, unsigned short port)
{
    int sd, ret;
    struct sockaddr_in s_in;
    bzero((char *)&s_in, sizeof(s_in));

    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = inet_addr(IP);
    s_in.sin_port = htons(PORT);
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#if USE_PEP
        ret = pep_connect(sd, (struct sockaddr*) &s_in, sizeof(s_in), PEP_INTERACTIVE);
#else
        ret = connect(sd, (struct sockaddr*) &s_in, sizeof(s_in));
#endif

    if(ret < 0){
        printf("Unable to connect %d.\n", ret);
        return -1;
    }

    printf("Connected. %d\n", ret);
    return sd;
}

double average_variation(double values[], int size) {
    double sum = 0.0;
    double mean = 0.0;
    double absolute_differences[size];
    double sum_of_differences = 0.0;
    double average_variation = 0.0;
    double highest = 0;
    double lowest = 100000000;
    int i;

    /* Calculate the mean (average) of the values */
    for (i = 0; i < size; i++) {
        sum += values[i];

        if(values[i] > highest)
            highest = values[i];
        
        if(values[i] < lowest)
            lowest = values[i];

    }
    mean = sum / size;

    /* Find the absolute difference between each value and the mean */
    for (i = 0; i < size; i++) {
        absolute_differences[i] = fabs(values[i] - mean);
    }

    /* Sum up all the absolute differences */
    for (i = 0; i < size; i++) {
        sum_of_differences += absolute_differences[i];
    }

    /* Divide the sum of the differences by the total number of values */
    average_variation = sum_of_differences / size;

    printf("\navg rtt: %.2f ms, hi: %.2f ms, lo: %.2f, avg var: %.2f ms. %s\n",mean, highest, lowest, average_variation, mixed == 1 ? "(MIXED)" : "");
    return average_variation;
}

void intHandler(int dummy) {

    average_variation(rtts, total_pings);
    close(server);
    exit(0);

}

void parse_opts(int argc, char* argv[])
{
    int option;
    while ((option = getopt(argc, argv, "ms:i:")) != -1) {
        switch (option) {
            case 'm':
                mixed = 1;
                break;
            case 's':
                samples = atoi(optarg);
                printf("Samples set to %d\n", samples);
                break;
            case 'i':
                interval = atoi(optarg);
                printf("Interval set to %d\n", interval);
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

int main(int argc, char * argv[])
{
    int ret;
    struct tcp_info info;
    socklen_t tcp_info_length = sizeof(info);
    char test[120];

    parse_opts(argc, argv);

    signal(SIGINT, intHandler);

    server = setup_socket(IP, PORT);

    printf("Running RTT test %dms interval, %d samples\n", interval, samples);

    /* Ping and print RTT */
    while(1)
    {
        ret = send(server, test, 120, 0);
        if(ret <= 0){
            printf("Error sending ping.\n");
            break;
        }

        ret = getsockopt(server, SOL_TCP, TCP_INFO, &info, &tcp_info_length);
        //printf("rtt: %u ms\n", info.tcpi_rtt/1000);
        rtts[total_pings] = info.tcpi_rtt/1000;
        total_pings++;
        if(total_pings == samples)
            break;

        usleep(interval*1000);
    }

    average_variation(rtts, total_pings);

    close(server);

    return 0;
}

