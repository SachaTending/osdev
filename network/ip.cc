#include "network.h"
#include "libc.h"
#include "pmm.h"
using namespace NetUtils;



namespace IP {
    uint16_t calculate_checksum(ip_packet_t *packet) {
        int arr_size = sizeof(ip_packet_t) / 2;
        uint16_t *arr1 = (uint16_t *)packet;
        uint8_t  *arr2 = (uint8_t  *)packet;
        uint32_t sum;
        for (int i=0;i<arr_size;i++) {
            sum += flip_short(arr1[i]);
        }
        uint32_t carry = sum >> 16;
        sum = sum & 0x0000ffff;
        sum = sum + carry;
        uint16_t ret = ~sum;
        return ret;
    }
    ip_packet_t *construct_pack(uint8_t ttl, uint8_t *dst_ip, void *data, int len) {
        ip_packet_t *packet = (ip_packet_t *)malloc(sizeof(ip_packet_t) + len);
        memset((void *)packet, 0, sizeof(ip_packet_t));
        packet->version = IP_IPV4;
        packet->ihl = 4;
        packet->tos = 0;
        packet->length = sizeof(ip_packet_t)+len;
        packet->id = 0;
        packet->flags = 0;
        packet->fragment_offset_high = 0;
        packet->fragment_offset_low = 0;
        
        packet->ttl = ttl;
        packet->protocol = PROTOCOL_UDP;
        return packet;
    }
    ip_packet_t *construct_pack(uint8_t *dst_ip, void *data, int len) {
        return construct_pack(64, dst_ip, data, len);
    }
}