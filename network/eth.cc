#include "network.h"
#include "logger.h"

logger eth("Ethernet");

using namespace NetUtils;

void eth_handl_packet(eth_frame_t *packet, int len) {
    void *data = (void *)packet+sizeof(eth_frame_t);
    int dlen = len-sizeof(eth_frame_t);
    if (ntohs(packet->type) == ETHERNET_TYPE_ARP)
        eth.log("Im receiver ARP packet, but i cannot handle it :(\n");
    else if (ntohs(packet->type) == ETHERNET_TYPE_IP)
        eth.log("Im received IP packet, but i cannot handle it :(\n");
}