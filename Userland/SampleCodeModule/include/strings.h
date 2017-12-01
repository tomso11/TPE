#ifndef __STRINGS__H__
#define __STRINGS__H__

int strlen(const char *str);
int strcmp(const char * s, const char * t);
int strncmp(const char * s, const char * t, unsigned int n);
int strcpyto(char * to, const char * from, char limit);
int strcpynto(char * to, const char * from, char limit, unsigned int n);
int strcpy(char * to, const char * from);

#endif
