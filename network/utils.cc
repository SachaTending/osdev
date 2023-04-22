#include "libc.h"

namespace NetUtils // this is first try when im using namespace
                    //     - TendingStream73
{
    uint32_t flip_long(uint16_t long_int) {
        uint32_t fbyte = *((uint8_t*)(&long_int));
        uint32_t sbyte = *((uint8_t*)(&long_int) + 1);
        uint32_t tbyte = *((uint8_t*)(&long_int)  + 2);
        uint32_t f2byte = *((uint8_t*)(&long_int) + 3);
        return (fbyte << 24) | (sbyte << 16) | (tbyte << 8) | (f2byte);
    }
    uint16_t flip_short(uint16_t short_int) {
        uint32_t fbyte = *((uint16_t*)(&short_int));
        uint32_t sbyte = *((uint8_t*)(&short_int)+1);
        return (fbyte << 8) | (sbyte);
    }
    uint8_t flip_byte(uint8_t byte, int nbits) {
        uint8_t t = byte << (8 - nbits);
        return t | (byte >> nbits);
    }
    uint8_t htonb(uint8_t byte, int nbits) {
        return flip_byte(byte, nbits);
    }
    uint16_t htons(uint16_t hshort) {
        return flip_short(hshort);
    }
    uint32_t htonl(uint32_t hlong) {
        return flip_long(hlong);
    }
    uint8_t ntohb(uint8_t byte, int nbits) {
        return flip_byte(byte, nbits);
    }
    uint16_t ntohs(uint16_t netshort) {
        return flip_short(netshort);
    }
    uint32_t ntohl(uint32_t nlong) {
        return flip_long(nlong);
    }
}