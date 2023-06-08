#include <pep/tcp.h>
#include <pep/common.h>

int pep_tcp_receive(struct socket *sock, u8* buffer, u32 size)
{
	struct msghdr msg = {
		.msg_flags = MSG_WAITALL,
	};

	struct kvec vec;
	int rc = 0;

	vec.iov_base = buffer;
	vec.iov_len  = size;

	//printk(KERN_INFO "[PEP] kernel_recvmsg: calling recvmsg \n");
pep_tcp_receive_read_again:
	rc = kernel_recvmsg(sock, &msg, &vec, 1, vec.iov_len, MSG_WAITALL);
	if (rc > 0)
	{
		//printk(KERN_INFO "[PEP] kernel_recvmsg: recvmsg returned %d\n", rc);
		return rc;
	}

	if(rc == -EAGAIN || rc == -ERESTARTSYS)
	{
		goto pep_tcp_receive_read_again;
	}

	//printk(KERN_INFO "[PEP] kernel_recvmsg: recvmsg returned %d\n", rc);
	return rc;
}

int pep_tcp_send(struct socket *sock, u8* buffer, u32 size)
{
        struct msghdr msg = {
                .msg_flags = /*MSG_DONTWAIT | */MSG_NOSIGNAL,
        };
        struct kvec vec;
        int len, written = 0, left = size;

pep_tcp_send_again:

        vec.iov_len = left;
        vec.iov_base = (char *)buffer + written;

        len = kernel_sendmsg(sock, &msg, &vec, left, left);
        if((len == -ERESTARTSYS) || (len == -EAGAIN))
                goto pep_tcp_send_again;
        if(len > 0)
        {
                written += len;
                left -= len;
                if(left)
                        goto pep_tcp_send_again;
        }
        return written ? written : len;
}