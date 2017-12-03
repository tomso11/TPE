#ifndef STLDIB_H
#define STLDIB_H

#define NULL ((void *) 0)

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void * malloc(int bytes);
void free(void * ptr);
int itoa(int value, char *str, int base);
int atoi(const char *str);

#endif