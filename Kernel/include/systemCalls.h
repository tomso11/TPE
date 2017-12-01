#ifndef __SYSCALLS__H__
#define __SYSCALLS__H__

#define STDOUT 0
#define STDIN 1
#define STDERR 2

#include <stdint.h>

#include "process_info.h"
#include "ipc_info.h"

uint64_t sys_write(uint64_t fds, const char * str, uint64_t len);
uint64_t sys_read(uint64_t fds, char * str, uint64_t len);
uint64_t sys_time(uint64_t selection);
uint64_t sys_paint(uint64_t color, uint64_t x, uint64_t y);
uint64_t sys_wait(uint64_t milliseconds);
uint64_t sys_screen_res(uint64_t selection);
uint64_t sys_text_space(uint64_t selection);
uint64_t sys_malloc(uint64_t bytes);
uint64_t sys_data_address();
uint64_t sys_set_char_color(uint64_t r, uint64_t g, uint64_t b);
uint64_t sys_set_bg_color(uint64_t r, uint64_t g, uint64_t b);

uint64_t sys_exec(uint64_t ptr, uint64_t params, const char * name);
uint64_t sys_end();
uint64_t sys_yield();

uint64_t sys_mutex_op(uint64_t nameptr);
uint64_t sys_mutex_cl(uint64_t key);
uint64_t sys_mutex_lock(uint64_t key);
uint64_t sys_mutex_unlock(uint64_t key);

uint64_t sys_fifo_op(uint64_t nameptr);
uint64_t sys_fifo_cl(uint64_t key);
uint64_t sys_fifo_read(uint64_t key, uint64_t buffer, uint64_t bytes);
uint64_t sys_fifo_write(uint64_t key, uint64_t buffer, uint64_t bytes);

uint64_t sys_set_foreground(uint64_t pid);
uint64_t sys_kill(uint64_t pid);
uint64_t sys_pid();
uint64_t sys_ppid();
uint64_t sys_process_info(uint64_t pid, struct process_info_c * pi);

uint64_t sys_cond_open(char * name);
uint64_t sys_cond_wait(int cond_key, int lock_key);
uint64_t sys_cond_signal(int key);
uint64_t sys_cond_broadcast(int key);
uint64_t sys_cond_close(int key);

uint64_t sys_get_pids(int * pid_array);

uint64_t sys_get_conds_info(cond_info info_array[]);
uint64_t sys_get_mutexes_info(mutex_info info_array[]);
uint64_t sys_get_fifos_info(fifo_info info_array[]);

#endif
