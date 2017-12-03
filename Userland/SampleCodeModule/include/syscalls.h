#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

#include "process_info.h"

#include "ipc_info.h"

#define STDOUT 0
#define STDIN 1
#define STDERR 2

int write(unsigned int fd, const void *buffer, unsigned int bytes);
int read(unsigned int fd, void *buffer, unsigned int bytes);
int time(int selector);
int paint(int color, int x, int y);
int wait(uint64_t milliseconds);
int screen_Xresolution();
int screen_Yresolution();
int text_rows();
int text_cols();
void * reserve (unsigned int bytes);
int free_reserve(void * address);
void * data_address();
int set_char_color (uint64_t r, uint64_t g, uint64_t b);
int set_bg_color (uint64_t r, uint64_t g, uint64_t b);
int mutex_open(char * name);
int mutex_close(int key);
int mutex_lock(int key);
int mutex_unlock(int key);
int fifo_open(char * name);
int fifo_close(int key);

int exec(void * ptr, uint64_t params, const char * name); /* Devuelve PID de nuevo proceso */
void end();
void yield();
int kill(uint64_t pid);
int set_foreground(uint64_t pid);
int pid();
int ppid();

int get_process_info(uint64_t pid, struct process_info_c * pi);

int cond_open(char * name);

/* Bloquea el proceso que lo llama. Debe encontrarse dentro de la zona critica
** dada por el mutex lock_key. Se desbloqueara este lock, y cuando sea despertado
** este proceso, lo hara con el lock recuperado. */

int cond_wait(int cond_key, int lock_key);

/* Despierta a un proceso que estaba esperando sobre cond_key. */
int cond_signal(int cond_key);

/* Despierta a todos los procesos esperando sobre cond_key. */
int cond_broadcast(int cond_key);
int cond_close(int cond_key);

int get_current_pids(int * pid_array);

int get_conds_info(cond_info info_array[]);
int get_mutexes_info(mutex_info info_array[]);
int get_fifos_info(fifo_info info_array[]);

#endif
