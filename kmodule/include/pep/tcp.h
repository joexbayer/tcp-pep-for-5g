#ifndef __PEP_TCP_H
#define __PEP_TCP_H

#include <pep/common.h>

int pep_tcp_receive(struct socket *sock, u8* buffer, u32 size);
int pep_tcp_send(struct socket *sock, u8* buffer, u32 size);

#endif // !__PEP_TCP_H