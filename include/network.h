#pragma once
#include "stdint.h"

namespace NetUtils
{
    uint8_t htonb(uint8_t byte, int nbits);
    uint16_t htons(uint16_t hshort);
    uint32_t htonl(uint32_t hlong);
    uint8_t ntohb(uint8_t byte, int nbits);
    uint16_t ntohs(uint16_t netshort);
    uint32_t ntohl(uint32_t nlong);
}
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP  0x0800

#define HARDWARE_TYPE_ETHERNET 0x01
typedef struct eth_frame {
    uint8_t dst_mac_addr[6];
    uint8_t src_mac_addr[6];
    uint16_t type;
    uint8_t data[];
} __attribute__((packed)) eth_frame_t;

void eth_handl_packet(eth_frame_t *packet, int len);