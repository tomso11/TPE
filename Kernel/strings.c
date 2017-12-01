#include "strings.h"

#define UINT_MAX 4294967295

/* Retorna la longitud de una cadena de carateres */
int str_len(char *str) {
	int i;
	for (i = 0; str[i] != '\0'; i++)
		;
	return i;
}

/* Compara dos cadenas de capacteres */
int strcmp(char * s, char * t) {
	return strncmp(s, t, UINT_MAX);
}

/* Compara dos cadenas de caracteres */
int strncmp(char * s, char * t, unsigned int n) {
	int i;
	for (i = 0; i < n && s[i] != '\0' && t[i] != '\0' && s[i] == t[i]; i++)
		;
	return s[i]-t[i];
}

/* Copia una cadena de caracteres a otra direccion de memoria. Devuelve la cantidad de caracteres copiados */
int strcpyto(char * to, char * from, char limit) {
	return strcpynto(to, from, limit, UINT_MAX);
}

int strcpy(char * to, const char * from) {
	int i;
	for (i = 0; from[i] != '\0'; i++)
		to[i] = from[i];
	to[i] = '\0';
	return i;
}

/* Copia una cadena de caracteres a otra direccion de memoria. Devuelve la cantidad de caracteres copiados */
int strcpynto(char * to, char * from, char limit, int n) {
	int i;

	for (i = 0; i < n && from[i] != '\0' && from[i] != limit; i++) {
		to[i] = from[i];
	}

	to[i] = '\0';
	return i;
}
