#ifndef LIBRARY_H
#define LIBRARY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "library.h"

#define LIBRARY_VERSION     "0.0.0"

enum PEP_OPTIONS {
    PEP_INTERACTIVE,
    PEP_NONINTERACTIVE
};

int pep_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int pep_option);

#ifdef __cplusplus
}
#endif

// LIBRARY_H
#endif
