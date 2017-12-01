#include "strings.h"
#include "limits.h"
#include "ctype.h"
#include "stdlib.h"

/* Retorna la longitud de un string */
int strlen(const char *str) {
	int i;
	for (i = 0; str[i] != '\0'; i++)
		;
	return i;
}

/* Compara dos cadenas de caracteres */
int strcmp(const char * s, const char * t) {
	return strncmp(s, t, UINT_MAX);
}

/* Compara dos cadenas de caracteres */
int strncmp(const char * s, const char * t, unsigned int n) {
	int i;
	for (i = 0; i < n-1 && s[i] != '\0' && t[i] != '\0' && s[i] == t[i]; i++)
		;
	return s[i]-t[i];
}

int strcpy(char * to, const char * from) {
	int i;
	for (i = 0; from[i] != '\0'; i++)
		to[i] = from[i];
	to[i] = '\0';
	return i;
}

/* Copia una cadena de caracteres a otra y devuelve la cantidad de caracteres copiados */
int strcpyto(char * to, const char * from, char limit) {
	return strcpynto(to, from, limit, UINT_MAX);
}

/* Copia una cadena de caracteres a otra y devuelve la cantidad de caracteres copiados */
int strcpynto(char * to, const char * from, char limit, unsigned int n) {
	int i;
	for (i = 0; i < n && from[i] != limit && from[i] != '\0'; i++) {
		to[i] = from[i];
	}
	to[i] = '\0';
	return i;
}