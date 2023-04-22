#pragma once
#include "stdint.h"

// Structs
typedef struct eth_frame {
    uint8_t dst_mac_addr[6];
    uint8_t src_mac_addr[6];
    uint16_t type;
    uint8_t data[];
} __attribute__((packed)) eth_frame_t;

typedef struct ip_packet {
    char version_ihl_ptr[0];
    uint8_t version:4;
    uint8_t ihl:4;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    char flags_fragment_ptr[0];
    uint8_t flags:3;
    uint8_t fragment_offset_high:5;
    uint8_t fragment_offset_low;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t data[];
} __attribute__((packed)) ip_packet_t;

// Namespaces
namespace NetUtils
{
    uint32_t flip_long(uint16_t long_int);
    uint16_t flip_short(uint16_t short_int);
    uint8_t flip_byte(uint8_t byte, int nbits);

    uint8_t htonb(uint8_t byte, int nbits);
    uint16_t htons(uint16_t hshort);
    uint32_t htonl(uint32_t hlong);
    uint8_t ntohb(uint8_t byte, int nbits);
    uint16_t ntohs(uint16_t netshort);
    uint32_t ntohl(uint32_t nlong);
}

namespace IP
{
    uint16_t calculate_checksum(ip_packet_t *packet);
    ip_packet_t *construct_pack(uint8_t ttl, uint8_t *dst_ip, void *data, int len);
    ip_packet_t *construct_pack(uint8_t *dst_ip, void *data, int len);
}


// Defines
#define ETHERNET_TYPE_ARP 0x0806
#define ETHERNET_TYPE_IP  0x0800

#define HARDWARE_TYPE_ETHERNET 0x01

#define IP_IPV4 4

#define IP_PACKET_NO_FRAGMENT 2
#define IP_IS_LAST_FRAGMENT 4

#define PROTOCOL_UDP 17
#define PROTOCOL_TCP 6

// Functions
void eth_handl_packet(eth_frame_t *packet, int len);
