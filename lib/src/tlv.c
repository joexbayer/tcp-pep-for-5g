/**
 * @file tlv.c
 * @author Joe Bayer (joeba@uio.no)
 * @brief Main code for all TLV functionality.
 * @version 0.1
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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

/**
 * @brief Simply inline function to validate a header inside given buffer.
 * 
 * @param buffer 
 * @return int 
 */
int tlv_validate(void* buffer)
{
  return ((struct __tlv_header*)buffer)->magic == __TLV_MAGIC;
}


/**
 * @brief Searches for option of given type in TLV buffer.
 * 
 * @param type (type to search for)
 * @param buffer (tlv buffer)
 * @return struct tlv* (NULL on error)
 */
struct tlv* tlv_get_option(int type, void* buffer)
{
    char* tlv = (char*) buffer;
    struct tlv* option;

    if(!tlv_validate(tlv))
        return NULL;

    tlv_for_each(option, tlv)
        if(option->type == type)
            return option;

    return NULL;
}

/**
 * @brief Adds a TLV header to start of given buffer.
 * 
 * @param buffer (buffer to write TLV header to.)
 * @return int  (0 on success)
 */
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

/**
 * @brief Prints out information of all TLV information in given buffer.
 * Makes sure buffer contains correct header information before printing.
 * 
 * @param buffer containing TLV information.
 * @return int (-1 on error, 0 on success)
 */
int tlv_print(void* buffer)
{   
    char* tlv = (char*) buffer;
    struct __tlv_header* header = (struct __tlv_header*) tlv;
    struct tlv* option;
    tlv += sizeof(struct __tlv_header);

    if(!tlv_validate(buffer))
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

/**
 * @brief Adds option to given TLV buffer based on parameters.
 * Makes sure correct header exists and adds it if it does not exist.
 * 
 * @param buffer Buffer to write option to.
 * @param type Type of option should be __tlv_type.
 * @param value Value of option.
 * @param optional Optional 32bit value
 * @return int (0 on success)
 */
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

    if(!tlv_validate(buffer))
        tlv_add_header(buffer);
    
    tlv += sizeof(struct __tlv_header);
    tlv += sizeof(struct tlv)*header->len;
    memcpy(tlv, &option, sizeof(option));

    header->len++;
    
    return 0;
}
