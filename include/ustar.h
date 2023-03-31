#pragma once

typedef struct {
    char name[100];
    char fmode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char last_mod[12];
    char checksum[8];
    char type[1];
    char linked_name[100];
    char magic[6];
    char ver[2];
    char uname[32];
    char gname[32];
    char dmajor[8];
    char dminor[8];
    char prefix[155];
    char data;
} __attribute__((packed)) ustar_t;