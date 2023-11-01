#include "../include/tlv.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main()
{
    struct tlv* option;
    char* buffer[tlv_size(4)];

    assert((tlv_add_header(buffer)) >= 0);

    assert((tlv_add_option(buffer, TLV_CONNECT, 1, 0)) >= 0);
    assert((tlv_add_option(buffer, TLV_INFO, 2, 0)) >= 0);
    assert((tlv_add_option(buffer, TLV_ERROR, 3, 1337)) >= 0);

    /* test if all get operations are successfull and correct value*/
    assert((option = tlv_get_option(TLV_CONNECT, buffer)) != NULL);
    assert(option->value == 1);
    assert(option->optional == 0);

    assert((option = tlv_get_option(TLV_INFO, buffer)) != NULL);
    assert(option->value == 2);
    assert(option->optional == 0);

    assert((option = tlv_get_option(TLV_ERROR, buffer)) != NULL);
    assert(option->value == 3);
    assert(option->optional == 1337);

    /* test if get operation fails on invalid type */
    assert((option = tlv_get_option(0x0, buffer)) == NULL);

    /* test if get operation fails on invalid buffer */
    assert((option = tlv_get_option(TLV_CONNECT, NULL)) == NULL);

    /* test if validate operation fails on invalid buffer */
    assert((tlv_validate(NULL)) == 0);

    printf("\nAll tests passed!\n");

    return 0;
}
