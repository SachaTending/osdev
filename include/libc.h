#pragma once // this is useful
#include "stdint.h"
size_t strlen(const char *c);
int strcpy(char *dst,const char *src);
int isspace(char c);

void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);

void callConstructors(void);
void itoa(char *buf, unsigned long int n, int base);
int atoi(char * string);
int oct2bin(unsigned char *str, int size);