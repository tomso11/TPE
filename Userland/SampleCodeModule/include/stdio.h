
#include <stdioASM.h>
#include <stdarg.h>
#define NULL 0
int putchar(char c);
void printFF(const char * format, char ** s, int * n);
void putNumber(int n);
void * malloc(int index);
int scanFF(const char * format, char ** s, int * n);
int strcmpN(char * str1, char * str2,int number);
int strcmp(char * str1, char * str2);
char getchar();
int itoa(int value, char *str, int base);
int atoi(const char *str);
int printf(const char *format, ...);
int fprintf(unsigned int fds, const char *format, ...);
static int fprinttype(unsigned int fds, const char *format, va_list args);
static int prints(unsigned int fds, const char *str);