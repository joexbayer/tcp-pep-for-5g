#ifndef D1044C73_E12D_49B8_AC3A_013D38550457
#define D1044C73_E12D_49B8_AC3A_013D38550457

struct socket* pep_endpoint_connect(u32 ip, u16 port);
void pep_endpoint_data_ready(struct sock* sk);
void pep_endpoint_receive_work(struct work_struct *work);

#endif /* D1044C73_E12D_49B8_AC3A_013D38550457 */
