#ifndef STDIO_H
#define STDIO_H


#include <stdarg.h>

int getchar();
void putchar(int c);
int printf(const char *format, ...);
int readline(char *str, unsigned int maxlen);
int readline_no_spaces(char *str, unsigned int maxlen);
int fprintf(unsigned int fds, const char *format, ...);
int scanf(const char *format, ...);

#endif
