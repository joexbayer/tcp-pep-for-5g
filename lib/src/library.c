/**
 * @file library.c
 * @author Joe Bayer (joeba@uio.no)
 * @brief Main library C file.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "library.h"
#include "tlv.h"

/* PEP IP/PORT from the perspective of the client */
#define PEP_IP "172.16.10.254"
#define PEP_PORT 8181

/**
 * @brief Attempts to establish a connection with a Performance Enhancing Proxy (PEP) server by wrapping the original
 * socket address and port information in a TLV header and sending it to the PEP server's IP address
 * and port using the provided socket file descriptor. The original socket address
 * and port information is added as an option in the TLV header.
 * @param sockfd The socket file descriptor.
 * @param addr A pointer to the original socket address.
 * @param addrlen The length of the original socket address.
 * @param pep_option An optional flag for use with the PEP server.
 * @return int Returns -1 on error or the number of bytes sent on success.
 */
int pep_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int pep_option)
{
    struct sockaddr_in new_addr;
    struct sockaddr_in* original_addr;
    int sample_tlv_size = 12;

    if(addrlen != sizeof(*original_addr))
        return -1; // TODO: Better error?
    
    original_addr = (struct sockaddr_in*) addr;

    bzero((char *)&new_addr, sizeof(new_addr));
    new_addr.sin_family = AF_INET;
    new_addr.sin_addr.s_addr = inet_addr(PEP_IP);
    new_addr.sin_port = htons(PEP_PORT);
    
    char* data[sample_tlv_size];
    tlv_add_header(data);
    tlv_add_option(data, TLV_CONNECT, original_addr->sin_port, original_addr->sin_addr.s_addr);

    //tlv_print(data);

    return sendto(sockfd, data, sample_tlv_size, MSG_FASTOPEN, (struct sockaddr *) &new_addr, sizeof(new_addr)); 
}


