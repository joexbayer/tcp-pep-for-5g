#include "../include/tlv.h"
#include <assert.h>

int main()
{
    char* buffer[tlv_size(4)];

    assert((tlv_add_header(buffer)) >= 0);

    assert((tlv_add_option(buffer, TLV_CONNECT, 1, 0)) >= 0);
    assert((tlv_add_option(buffer, TLV_INFO, 2, 0)) >= 0);
    assert((tlv_add_option(buffer, TLV_ERROR, 3, 1337)) >= 0);

    tlv_print(buffer);

    return 0;

}
