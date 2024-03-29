
#include "stdio.h"
#include "time.h"

int GMT = DEFAULT_GMT;

/*
** Configura el GMT del reloj.
** Retorna 1 si es un GMT valido, 0 sino.
*/
int setGMT(int gmt) {
	if (gmt >= MIN_GMT && gmt <= MAX_GMT) {
		GMT = gmt;
		return 1;
	}
	return 0;
}

/* Retorna el GMT actual */
int getGMT(int gmt) {
	return GMT;
}

/* Retorna los segundos */
int secondsC() {
    return sys_time(0);
}


/* Retorna los minutos actuales*/
int minutesC() {
    return sys_time(1);
}


/* Retorna la hora actual */
int hourC() {
   	int h = sys_time(2) + GMT;
	if (h >= 0 && h <= 23)
		return h;
	return h < 0 ? h + 24 : h - 24;
}

/* Pone al sistema en espera por una cantidad de milisegundos recibida por parámetro */
void sleep_primitive(uint64_t milliseconds) {
	sys_wait(milliseconds);
}