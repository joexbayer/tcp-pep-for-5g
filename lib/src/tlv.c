#ifndef CONFIG_KMODULE

#include "tlv.h"
#include <stdio.h>
#include <string.h>
#define tlvprintf(f_, ...) printf((f_), ##__VA_ARGS__)

#else
#include "../include/tlv.h"
#include <linux/module.h>
#include <linux/string.h>
#define tlvprintf(f_, ...) printk((f_), ##__VA_ARGS__)

#endif

int tlv_add_header(void* buffer)
{
    char* tlv = (char*) buffer;
    struct __tlv_header header = {
        .version = __TLV_VERSION,
        .len = 0,
        .magic = __TLV_MAGIC
    };
    memcpy(tlv, &header, sizeof(struct __tlv_header));

    return 0;
}


int tlv_print(void* buffer)
{   
    char* tlv = (char*) buffer;
    struct __tlv_header* header = (struct __tlv_header*) tlv;
    struct tlv* option;
    int i = 0;
    tlv += sizeof(struct __tlv_header);

    if(header->magic != __TLV_MAGIC)
        return -1;

    tlvprintf("********** TLV **********\n");
    tlvprintf("Header: \n");
    tlvprintf("\tVersion %d\n\tOptions: %d\n\tMagic 0x%x\n", header->version, header->len, header->magic);  
  
    tlv_for_each(option, buffer)
    {
        tlvprintf("TLV Option\n");
        tlvprintf("\tType %x\n\tLength: %d\n\tvalue %d\n", option->type, option->length, option->value);
        if(option->optional != 0)
            tlvprintf("\tOptional: %d\n", option->optional);
    }

    return 0;
}

int tlv_add_option(void* buffer, unsigned char type, unsigned short value, unsigned int optional)
{
    char* tlv = (char*) buffer;
    struct __tlv_header* header = (struct __tlv_header*) buffer;
    struct tlv option = {
        .type = type,
        .length = optional == 0 ? sizeof(value) : sizeof(value) + sizeof(optional),
        .value = value,
        .optional = optional
    };

    if(header->magic != __TLV_MAGIC)
        tlv_add_header(buffer);
    
    tlv += sizeof(struct __tlv_header);
    tlv += sizeof(struct tlv)*header->len;
    memcpy(tlv, &option, sizeof(option));

    header->len++;
    
    return 0;
}
