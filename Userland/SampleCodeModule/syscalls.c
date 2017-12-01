#include <stdint.h>
#include "syscalls.h"
#include "stdio.h"
#include "process_info.h"

/* Contiene todas las llamadas a int80 */
uint64_t _int80h(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx);

/* SystemCall Read */
int read(unsigned int fd, void *buffer, unsigned int bytes) {
    return _int80h(3, fd, (uint64_t) buffer, bytes);
}

/* SystemCall Write */
int write(unsigned int fd, const void *buffer, unsigned int bytes) {
    return _int80h(4, fd, (uint64_t) buffer, bytes);
}

/* SystemCall Time */
int time(int selector) {
    return _int80h(5, selector, 0, 0);
}

/* SystemCall Paint */
int paint(int color, int x, int y) {
    return _int80h(6, color, x, y);
}

/* SystemCall Wait */
int wait(unsigned long int milliseconds) {
	return _int80h(7, milliseconds, 0, 0);
}

/* SystemCall para obtener la resolucion de pantalla */
int screen_Xresolution() {
    return _int80h(8, 0, 0, 0);
}

/* SystemCall para obtener la resolucion de pantalla */
int screen_Yresolution() {
    return _int80h(8, 1, 0, 0);
}

/* SystemCall para obtener las filas*/
int text_rows() {
    return _int80h(9, 0, 0, 0);
}

/* SystemCall para obtener las columnas */
int text_cols() {
    return _int80h(9, 1, 0, 0);
}

/* SystemCall para reservar espacio en memoria*/
void * reserve (unsigned int bytes) {
	return (void *) _int80h(10, bytes, 0, 0);
}

/* SystemCall para reservar espacio en memoria*/
int free_reserve (void * address) {
    return _int80h(11, (uint64_t) address, 0, 0);
}

/* SystemCall para obtener la direccion de memoria de datos */
void * data_address() {
    return (void *) _int80h(12, 0, 0, 0);
}

int set_char_color (uint64_t r, uint64_t g, uint64_t b) {
    return _int80h(13,r,g,b);
}

int set_bg_color (uint64_t r, uint64_t g, uint64_t b) {
    return _int80h(14,r,g,b);
}

int exec(void * ptr, uint64_t params, const char * name) {
  return _int80h(15, (uint64_t) ptr, params, (uint64_t) name);
}

void end() {
  _int80h(16, 0, 0, 0);
}

void yield() {
  _int80h(17, 0, 0, 0);
}

int mutex_open(char * name) {
  return _int80h(18, (uint64_t) name, 0, 0);
}

int mutex_close(int key) {
  return _int80h(19, (uint64_t) key, 0, 0);
}

int mutex_lock(int key) {
  return _int80h(20, (uint64_t) key, 0, 0);
}

int mutex_unlock(int key) {
  return _int80h(21, (uint64_t) key, 0, 0);
}

int set_foreground(uint64_t pid) {
  return _int80h(22, pid, 0, 0);
}

int fifo_open(char * name) {
  return _int80h(23, (uint64_t) name, 0, 0);
}

int fifo_close(int key) {
  return _int80h(24, (uint64_t) key, 0, 0);
}

int kill(uint64_t pid) {
  return _int80h(25, pid, 0, 0);
}

int pid() {
  return _int80h(26, 0, 0, 0);
}

int ppid() {
  return _int80h(27, 0, 0, 0);
}

int get_process_info(uint64_t pid, struct process_info_c * pi) {
  return _int80h(28, pid, (uint64_t) pi, 0);
}

int cond_open(char * name) {
  return _int80h(29, (uint64_t) name, 0, 0);
}

int cond_wait(int cond_key, int lock_key) {
  return _int80h(30, cond_key, lock_key, 0);
}

int cond_signal(int key) {
  return _int80h(31, key, 0, 0);
}

int cond_broadcast(int key) {
  return _int80h(32, key, 0, 0);
}

int cond_close(int key) {
  return _int80h(33, key, 0, 0);
}

int get_current_pids(int * pid_array) {
  return _int80h(34, (uint64_t) pid_array, 0, 0);
}

int get_conds_info(cond_info info_array[]) {
  return _int80h(35, (uint64_t) info_array, 0, 0);
}

int get_mutexes_info(mutex_info info_array[]) {
  return _int80h(36, (uint64_t) info_array, 0, 0);
}

int get_fifos_info(fifo_info info_array[]) {
  return _int80h(37, (uint64_t) info_array, 0, 0);
}
