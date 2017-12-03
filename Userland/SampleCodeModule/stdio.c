#include "stdio.h"
#include "stdioASM.h"
#include <ctype.h>
//#include "stringlib.h"
//#include "syscalls.h"
#define LETTER  0
#define FORMAT  1
#define NULL 0
#define BUFFERSIZE 2500
#define STDOUT 0
#define STDIN 1
#define STDERR 2
#define BUFF_SIZE 1024
#define AUX_SIZE 64
#define SPACE 0


int putchar(char c){
	char * msg = malloc(1);
	*msg = c;
	writeC(msg,1);
	return 0;
}

char getchar(){
	char * buffer = malloc(1);
	*buffer = 0;
	while( *buffer == 0){
		readC(buffer,1);
	}

	return *buffer;
}


void putNumber(int n){
	char * buffer = malloc(20);
	int size = 0;
	if(n < 0){
		putchar('-');
		n = -n;
	}
	while(n/10){
		*buffer = n%10 +'0';
		buffer++;
		size++;
		n = n/10;
	}
	*buffer = n + '0';
	size++;
	while(size != 0){
		putchar(*buffer);
		buffer--;
		size--;
	}
	
}


void printFF(const char * format, char ** s, int * n){
 int state = LETTER;
 while(*format != 0){
 	switch(state){

		case LETTER:
			if(*format == '%'){
				state = FORMAT;
				format++;
				break;
			}

			putchar(*format);
			format++;
			break;
		case FORMAT:
			if(*format == 'd'){
				if(n != NULL){
					int nummber = *n;
				n++;
				putNumber(nummber);
				state = LETTER;
				format++;
				break;
				
				}
				
			}
			if(*format == 's'){
				if(s != NULL){
					char * string = s[0];
				s++;
				while(*string != 0){
					putchar(*string);
					string++;
				}
				state = LETTER;
				format++;
				break;
				
				}
				
			}
			else {
				putchar('%');
				format++;
				break;
			}

		}
	}
	if(*(format-1) == '%'){
		putchar('%');
	}
	return;

}

int scanFF(const char * format, char ** s, int * n){
 int bufferindex;
 int state = LETTER;
 int indexS = 0;
 int indexN = 0;
 while(*format != 0){
 	switch(state){

		case LETTER:
			if(*format == '%'){
				state = FORMAT;
				format++;
				break;
			}
			char c;
			while((c=getchar()) == *format){
				;
			}
			format++;
			break;
		case FORMAT:
			if(*format == 'd'){
				if(n != NULL){
					int nummber = n[indexN];
				n++;
				indexN++;
				putNumber(nummber);
				state = LETTER;
				format++;
				break;
				
				}
				
			}
			if(*format == 's'){
				if(s != NULL){
				char * string = s[indexS];
				indexS++;
				char c;
				bufferindex = 0;
				while((c = getchar()) != '\n'){
					if(c == '\b'){
						if(bufferindex != 0){
							string--;
							bufferindex--;
						}
					}
					else{
						if(bufferindex == BUFFERSIZE){
							break;
						}
						*string = c;
						string++;
						bufferindex++;
					}	
				}
				*string = 0;
				state = LETTER;
				format++;
				break;
				
				}
				
			}
		}
	}
	return bufferindex == BUFFERSIZE;

}

void * malloc(int bytes){
	static void * position = 0x1000000;
	void * aux = position;
	position+=bytes;
	return aux;
}

int strcmp(char * str1, char * str2){
	while((*str1 != 0) && (*str2 != 0)){
		if((*str1 - *str2) > 0){
			return 1;
		}
		if((*str1 - *str2) < 0){
			return -1;
		}
		str1++;
		str2++;
	}
	if(*str1 == 0 && *str2 == 0){
		return 0;
	}
	if(*str1 == 0){
		return -1;
	}
	return 1;
}

int strcmpN(char * str1, char * str2,int number){
	while((*str1 != 0) && (*str2 != 0) && number){
		if((*str1 - *str2) > 0){
			return 1;
		}
		if((*str1 - *str2) < 0){
			return -1;
		}
		str1++;
		str2++;
		number--;
	}
	if(!number){
		return 0;
	}
	if(*str1 == 0 && *str2 == 0){
		return 0;
	}
	if(*str1 == 0){
		return -1;
	}
	return 1;
}



void putstring(char * str){
	
	prints(STDOUT,str);
}

int printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	return fprinttype(STDOUT, format, args);
}

int fprintf(unsigned int fds, const char *format, ...) {
	va_list args;
	va_start(args, format);
	return fprinttype(fds, format, args);
}

/* Imprime en el descriptor fds con el formato indicado. Soporta los siguientes: d,i,s,c. Retorna la cantidad de caracteres escritos.
** En caso de simbolo invalido imprime todos los caracteres hasta la ocurrencia de dicho simbolo donde deja de imprimir y retorna.*/
static int fprinttype(unsigned int fds, const char *format, va_list args) {
	int len = 0; // cantidad de caracteres escritos
	char aux[AUX_SIZE];

	while(*format != '\0') {
		char c = *format++;
		char symbol;
		if(c != '%') {
			putchar(c);
			len++;
		}
		else {
			symbol = *format++;
			switch(symbol) {
				case 'd':
					//len += printi(fds, va_arg(args, int), aux);
					break;
				case 'i':
					//len += printi(fds, va_arg(args, int), aux);
					break;
				case 's':
					len += prints(fds, va_arg(args, char *));
					break;
				case 'c':
					putchar(va_arg(args,int) & 0xFF);
					len++;
					break;
				case '%':
					putchar('%');
					len++;
					break;
				default:  // simbolo invalido. Termina la funcion y retorna
					return len;
			}
		}
	}
	va_end(args);
	return len;
}



/*Imprime una cadena de carateres a pantalla */
static int prints(unsigned int fds, const char *str) {
	int i;
	for (i=0; str[i] != '\0'; i++)
		putchar(str[i]);
	return i;
}





/* Copia en str los valores ascii de los digitos de value en la base indicada.
** Devuelve la cantidad de digitos copiados. */
/*int itoa(int value, char *str, int base) {
	char *p = str;
	char *p1, *p2;
	int len = 0;

	if(value < 0 && base == 10) {
		value = -value;
		len++;
		*p++ = '-';
		str++;
	}

	//Calculate characters for each digit
	do
	{
		int remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		len++;
	}
	while (value /= base);

	// Terminate string in str.
	*p = '\0';

	//Reverse string in str.
	p1 = str;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return len;
}*/

/* A partir de un string retorna su valor entero */
/*int atoi(const char *str) {
	while (isspace(*str))
		str++;

	int num = 0;
	int sign = 1;
	if (*str == '-') {
		str++;
		sign = -1;
	}
	else if (*str == '+')
		str++;

	while (*str != '\0' && isdigit(*str)) {
		num = num * 10 + (*str) - '0';
		str++;
	}

	return num * sign;
}*/


