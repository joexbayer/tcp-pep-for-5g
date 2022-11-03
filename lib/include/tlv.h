#ifndef __TLV_H
#define __TLV_H

#ifdef __cplusplus
extern "C"
{
#endif

#define LIBRARY_VERSION     "0.0.0"
#define __TLV_VERSION 1
#define __TLV_MAGIC 0xbeef

enum __tlv_types {
    TLV_INFO = 0x1,    // Info TLV
    TLV_CONNECT = 0xA,    // Connect TLV          
    TLV_EXT_TCP = 0x14,   // Extended TCP header
    TLV_SUPP_EXT = 0x15,   // Supported TCP extension
    TLV_COOKIE = 0x16,   // Cookie TLV
    TLV_ERROR = 0x1E    // Error TLV
};

struct __tlv_header {
    unsigned char version;
    unsigned char len;
    unsigned short magic;
};

struct tlv {
    unsigned char type;
    unsigned char length;
    unsigned short value;
    unsigned int optional;
};

#define tlv_for_each(option, buffer) \
    for ((option) = buffer+(sizeof(struct __tlv_header)); \
        (((unsigned long)option)-((unsigned long)buffer)) <= (((struct __tlv_header*)buffer)->len*(sizeof(struct tlv))) && ((struct __tlv_header*)buffer)->magic == __TLV_MAGIC; \
         (option) = (struct tlv*) (((char*)option)+ sizeof(struct tlv)))

#define tlv_size(options) (sizeof(struct __tlv_header)+(options*sizeof(struct tlv)))

int tlv_add_header(void* buffer);
int tlv_print(void* buffer);
int tlv_add_option(void* buffer, unsigned char type, unsigned short value, unsigned int optional);

#ifdef __cplusplus
}
#endif

#endif /* __TLV_H */
