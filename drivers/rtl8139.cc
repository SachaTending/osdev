#include "io.h"
#include "logger.h"
#include "pci.h"
#include "module.h"
#include "idt.h"
#include "pmm.h"
#include "libc.h"
#include "network.h"

uint16_t io_base;
uint32_t mem_base;

char * rx_buffer;
char tx_buffer[8192*1024+16];

uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};
uint8_t TSD_array[4]  = {0x10, 0x14, 0x18, 0x1C};

int reg_loc = 0;

uint8_t mac_addr[6];

logger rtl("RTL8139");

void print_mac() {
    rtl.log("MAC: %x:%x:%x:%x:%x:%x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void read_mac() {
    uint32_t mac1 = inl(io_base+0x00);
    uint16_t mac2 = inw(io_base+0x04);
    mac_addr[0] = mac1 >> 0;
    mac_addr[1] = mac1 >> 8;
    mac_addr[2] = mac1 >> 16;
    mac_addr[3] = mac1 >> 24;

    mac_addr[4] = mac2 >> 0;
    mac_addr[5] = mac2 >> 8;
}

void rtl8139_send(uint32_t *pack, uint32_t len) {
    outl(io_base+TSAD_array[reg_loc], (uint32_t)pack);
    outl(io_base+TSD_array[reg_loc++], len);
    //printf("0x%x 0x%x %u 0x%x %d\n", (uint64_t)pack, (uint32_t)pack, len, TSD_array[reg_loc], reg_loc);
    if (reg_loc > 3) reg_loc=0;
}
uint32_t rtl8139_current_packet_ptr;
#define RX_READ_POINTER_MASK (~3)
void rtl8139_recv_packet() {
    uint16_t * packet = (uint16_t*)((rx_buffer) + rtl8139_current_packet_ptr);
    uint16_t packet_length = *(packet + 1);
    rtl.log("length: %u\n", packet_length);
    rtl.log("packet:\n");
    printf("%s\n", packet);
    packet = packet + 2;
    void * packet_backup = (void *)malloc(packet_length);
    memcpy(packet_backup, (const void *)packet, packet_length);
    eth_handl_packet((eth_frame_t *)packet_backup, packet_length);
    free(packet_backup);
    rtl8139_current_packet_ptr = (rtl8139_current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;
    if(rtl8139_current_packet_ptr > 8192)
        rtl8139_current_packet_ptr -= 8192;
    outw(io_base+0x38, rtl8139_current_packet_ptr-0x10);
}

#define ROK                 (1<<0)
#define TOK                 (1<<2)
void rtl8139_irq(struct stackframe_t *stack) {
    rtl.log("INT\n");
    uint16_t status = inw(io_base + 0x3e);
    if (status & TOK) rtl.log("Packet sended.\n");
    else if (status & ROK) {
        rtl.log("Packet received\n");
        rtl8139_recv_packet();
    }
    outw(io_base + 0x3E, 0x05);
}
uint32_t pack[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};
void rtl8139_trig(pci_dev_t dev) {
    uint32_t ret = pci_get_bar(dev, 0);
    io_base = ret & (~0x3);
    mem_base = ret & (~0xf);
    rtl.log("Initializating controller...\n");
    pci_dma_init(dev);
    rtl.log("io_base = 0x%x\n", io_base);
    outb(io_base+0x52, 0x0);
    rtl.log("controller enabled.\n");
    outb(io_base+0x37, 0x10);
    rtl.log("waiting controller for reset.\n"); 
    while((inb(io_base+0x37) & 0x10) != 0) {
        rtl.log("wait\n");
    }
    rtl.log("controller resetted.\n");
    rx_buffer = new char[8192 + 1500 + 16];
    outl(io_base+0x30, (uint32_t)rx_buffer);
    rtl.log("buffer: 0x%x\n", (uint32_t)rx_buffer);
    outw(io_base+0x3c, 0x0005);
    outl(io_base+0x44, 0xf | (1 << 7));
    outb(io_base+0x37, 0x0C);
    read_mac();
    print_mac();
    uint32_t irq = pciConfigReadW(dev, 0x3C);
    irq = irq & 0x00ff;
    rtl.log("IRQ: %u\n", irq);
    idt_set_handl(irq, rtl8139_irq);
    rtl8139_send((uint32_t *)&pack, 16);
}

void rtl8139_init() {
    pci_add_trig(rtl8139_trig, 0x10EC, 0x8139);
    rtl.log("Installed PCI trigger.\n");
}

MODULE rtl8139 = {
    .name = {'r', 't', 'l', '8', '1', '3', '3'},
    .mod_entry = rtl8139_init,
    .type = MOD_PCI
};
