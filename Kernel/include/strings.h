#ifndef __STRINGS__H__
#define __STRINGS__H__


int str_len(char *str);
int strcmp(char * s, char * t);
int strncmp(char * s, char * t, unsigned int n);
int strcpyto(char * to, char * from, char limit);
int strcpynto(char * to, char * from, char limit, int n);
int strcpy(char * to, const char * from);

#endif
