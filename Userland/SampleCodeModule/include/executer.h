#ifndef EXECUTER_H
#define EXECUTER_H

/* Wrappers para ejecutar una función. Se encargan de llamar a end() y liberar los recursos
** que sean necesarios. La función que se desea ejecutar recibe los parámetros
** (int argc, char * argv[]) 
** Devuelven el PID del nuevo proceso. */

/* Recibe un string cuyos parámetros están separados por un único espacio. No hay espacios al
** comienzo ni al final. La función será ejecutada con dichos parámetros como argv[] */
int execp(void * function_ptr, const char * arg, const char * name);

/* Igual que arriba solo que no se pasan parámetros */
int execpn(void * function_ptr, const char * name);

#endif