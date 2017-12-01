#include "videoDriver.h"
#include "keyboardDriver.h"
#include "systemCalls.h"
#include "interrupts.h"
#include "rtc.h"
#include "timer.h"
#include "lib.h"
#include "dirs.h"
#include "memoryAllocator.h"
#include "processManager.h"
#include "kernelMutex.h"
#include "fifo.h"
#include "condVariable.h"
#include "defs.h"
#include "ipc_info.h"

#define ERR_COLOR 255,0,0  // Rojo

#define INIT 0
#define SHELL 1

#define SYS_SIZE (sizeof(syscalls)/sizeof(syscalls[0]))

static uint64_t sys_write_wr(uint64_t fds, uint64_t str, uint64_t length);
static uint64_t sys_read_wr(uint64_t fds, uint64_t buffer, uint64_t len);
static uint64_t sys_time_wr(uint64_t selection, uint64_t unused1, uint64_t unused2);
static uint64_t sys_wait_wr(uint64_t milliseconds, uint64_t unused1, uint64_t unused2);
static uint64_t sys_screen_res_wr(uint64_t selection, uint64_t unused1, uint64_t unused2);
static uint64_t sys_text_space_wr(uint64_t selection, uint64_t unused1, uint64_t unused2);
static uint64_t sys_malloc_wr(uint64_t bytes, uint64_t unused1, uint64_t unused2);
static uint64_t sys_data_address_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_free_wr(uint64_t selection, uint64_t unused2, uint64_t unused3);
static uint64_t sys_exec_wr(uint64_t ptr, uint64_t params, uint64_t name);
static uint64_t sys_end_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_yield_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_mutex_op_wr(uint64_t nameptr, uint64_t unused2, uint64_t unused3);
static uint64_t sys_mutex_cl_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_mutex_lock_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_mutex_unlock_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_fifo_op_wr(uint64_t nameptr, uint64_t unused2, uint64_t unused3);
static uint64_t sys_fifo_cl_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_set_foreground_wr(uint64_t pid, uint64_t unused2, uint64_t unused3);
static uint64_t sys_kill_wr(uint64_t pid, uint64_t unused2, uint64_t unused3);
static uint64_t sys_pid_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_ppid_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_process_info_wr (uint64_t pid, uint64_t pi, uint64_t unused3);
static uint64_t sys_cond_open_wr(uint64_t name, uint64_t unused2, uint64_t unused3);
static uint64_t sys_cond_wait_wr(uint64_t cond_key, uint64_t lock_key, uint64_t unused3);
static uint64_t sys_cond_signal_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_cond_broadcast_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_cond_close_wr(uint64_t key, uint64_t unused2, uint64_t unused3);
static uint64_t sys_get_pids_wr(uint64_t pid_array, uint64_t unused2, uint64_t unused3);
static uint64_t sys_get_conds_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3);
static uint64_t sys_get_mutexes_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3);
static uint64_t sys_get_fifos_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3);


static unsigned int fifo_to_fds(int key);
static int fds_to_fifo(unsigned int fds);


static unsigned int fifo_to_fds(int key) {
	return key + FILE_DESCRIPTORS;
}

static int fds_to_fifo(unsigned int fds) {
	return fds - FILE_DESCRIPTORS;
}


/* Vector de system calls */
static uint64_t (*syscalls[]) (uint64_t,uint64_t,uint64_t) = { 0,0,0, 		/* 0, 1, 2 system calls reservados*/
															   sys_read_wr,         /* 3 */
															   sys_write_wr,        /* 4 */
															   sys_time_wr,         /* 5 */
															   sys_paint,           /* 6 */
															   sys_wait_wr,         /* 7 */
															   sys_screen_res_wr,   /* 8 */
															   sys_text_space_wr,   /* 9 */
															   sys_malloc_wr,       /* 10 */
															   sys_free_wr,			/* 11 */
															   sys_data_address_wr, /* 12 */
															   sys_set_char_color,  /* 13 */
															   sys_set_bg_color,    /* 14 */
															   sys_exec_wr,			/* 15 */
															   sys_end_wr,			/* 16 */
															   sys_yield_wr,		/* 17 */
															   sys_mutex_op_wr,		/* 18 */
															   sys_mutex_cl_wr,		/* 19 */
															   sys_mutex_lock_wr, 	/* 20 */
															   sys_mutex_unlock_wr,	/* 21 */
															   sys_set_foreground_wr, /* 22 */
															   sys_fifo_op_wr, /* 23 */
															   sys_fifo_cl_wr, /* 24 */
															   sys_kill_wr, /* 25 */
															   sys_pid_wr,   /* 26 */
															   sys_ppid_wr,   /* 27 */
															   sys_process_info_wr, /* 28 */
															   sys_cond_open_wr,   /* 29 */
															   sys_cond_wait_wr,  /* 30 */
															   sys_cond_signal_wr, /* 31 */
															   sys_cond_broadcast_wr,  /* 32 */
															   sys_cond_close_wr,  /* 33 */
															   sys_get_pids_wr,		/* 34 */
															   sys_get_conds_info_wr,  /* 35 */
															   sys_get_mutexes_info_wr,  /* 36 */
															   sys_get_fifos_info_wr /* 37 */
															};

/* Ejecuta la system call correspondiente al valor de rax */
uint64_t syscallDispatcher(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx) {
	if (rax < SYS_SIZE && rax >= 3)
		return (*syscalls[rax])(rbx,rcx,rdx);
	return 0;
}

/* SystemCall de Read para leer de entrada estándar*/
uint64_t sys_read(uint64_t fds, char * buffer, uint64_t bytes) {
	unsigned int i = 0;
	char c;
    if (fds == STDIN) {
		if (get_current_process() != get_foreground_process()) {
			block_foreground_process(get_current_process());
			yield_process();
		}
		while (i < bytes) {
			c = get_char();
			if (c != -1) {
				buffer[i++] = c;
			}
			else {
				_sti();
				block_read_process(get_current_process());
				yield_process();
			}
		}
    }
    else if (fds >= FILE_DESCRIPTORS)
    	i = fifo_read(fds_to_fifo(fds), buffer, bytes);
    return i;
}

/* SystemCall de Write para escribir a salida estándar */
uint64_t sys_write(uint64_t fds, const char * str, uint64_t length) {
	unsigned int n;

	if (fds == STDERR) {
		unsigned char r,g,b; // Backup
		current_char_color(&r,&g,&b);
		set_char_color(ERR_COLOR);
		put(str, length);
		set_char_color(r,g,b);
		n = length;
	}
	else if (fds == STDOUT) {
		put(str, length);
		n = length;
		return length;
	}
	else if (fds >= FILE_DESCRIPTORS) {
		n = fifo_write(fds_to_fifo(fds), str, length);
	}

	return n;
}

/* SystemCall de Time, retorna hora, minutos y segundos actuales */
uint64_t sys_time(uint64_t selection) {
    switch(selection) {
	case 0:
	    return seconds();
	case 1:
	    return minutes();
	case 2:
	    return hour();
    }
    return -1;
}

/* SystemCall de paint para pintar un pixel */
uint64_t sys_paint(uint64_t color, uint64_t x, uint64_t y) {
	char r = (color >> 16) & 0xFF;
	char g = (color >> 8) & 0xFF;
	char b = color & 0xFF;

	return fill(r,g,b,x,y); // 1 si fue un pixel válido, 0 sino.
}

/* SystemCall de wait, Pone en espera a la pantalla por la cantidad de milisegundos pasados por parámetro*/
uint64_t sys_wait(uint64_t milliseconds) {
	return sleep(milliseconds);
}

/* SystemCall que retorna la resolucion de la pantalla*/
uint64_t sys_screen_res(uint64_t selection) {
	switch (selection) {
		case 0: // screen width
			return scr_x_res();
		case 1: // screen height
			return scr_y_res();
	}
	return -1;
}

/* SystemCall que retorna la cantidad de filas o columnas de la pantalla */
uint64_t sys_text_space(uint64_t selection) {
	switch (selection) {
		case 0: // screen width
			return text_rows();
		case 1: // screen height
			return text_cols();
	}
	return -1;
}

/* SystemCall Malloc */
uint64_t sys_malloc(uint64_t bytes) {
	void * page = (void *) get_page(bytes);
	add_data_page(get_current_process(), page);
	return (uint64_t) page;
}

/* System call free*/
uint64_t sys_free(uint64_t address) {
	remove_data_page(get_current_process(), (void *) address);
	return store_page(address);
}

/* System call que retorna la dirección del módulo de datos */
uint64_t sys_data_address() {
	return DATA_ADDRESS;
}

/* System call para el seteo del color de letra */
uint64_t sys_set_char_color(uint64_t r, uint64_t g, uint64_t b) {
	return set_char_color(r,g,b);
}

/* SystemCall para el seteo de color de fondo */
uint64_t sys_set_bg_color(uint64_t r, uint64_t g, uint64_t b) {
	return set_bg_color(r,g,b);
}

uint64_t sys_exec(uint64_t ptr, uint64_t params, const char * name) {
	return exec_process(create_process(ptr, params, name));
}

uint64_t sys_end() {
	end_process();
	return 1;
}

uint64_t sys_yield() {
	yield_process();
	return 1;
}

uint64_t sys_mutex_op(uint64_t nameptr) {
	return mutex_open((char *)nameptr);
}

uint64_t sys_mutex_cl(uint64_t key) {
	return mutex_close(key);
}

uint64_t sys_mutex_lock(uint64_t key) {
	return mutex_lock(key);
}

uint64_t sys_mutex_unlock(uint64_t key) {
	return mutex_unlock(key);
}

uint64_t sys_fifo_op(uint64_t nameptr) {
	return fifo_to_fds(fifo_open((char *)nameptr));
}

uint64_t sys_fifo_cl(uint64_t fds) {
	return fifo_close(fds_to_fifo(fds));
}

uint64_t sys_set_foreground(uint64_t pid) {
	process * p = get_process_by_pid(pid);

	if (p == NULL)
		return 0;

	set_foreground_process(p);

	return 1;
}

uint64_t sys_kill(uint64_t pid) {
	int valid = 0;

	if (pid != INIT && pid != SHELL) {
		process * p = get_process_by_pid(pid);
		valid = kill_process(p);
	}

	return valid;
}

uint64_t sys_pid() {
	return pid_process(get_current_process());
}
uint64_t sys_ppid() {
	return ppid_process(get_current_process());
}

uint64_t sys_process_info(uint64_t pid, struct process_info_c * pi) {
	return get_process_info_by_pid(pi, pid);
}

uint64_t sys_cond_open(char * name) {
	return cond_open(name);
}

uint64_t sys_cond_wait(int cond_key, int lock_key) {
	return cond_wait(cond_key, lock_key);
}

uint64_t sys_cond_signal(int key) {
	return cond_signal(key);
}

uint64_t sys_cond_broadcast(int key) {
	return cond_broadcast(key);
}

uint64_t sys_cond_close(int key) {
	return cond_close(key);
}

uint64_t sys_get_pids(int * pid_array) {
	return get_current_pids(pid_array);
}

uint64_t sys_get_conds_info(cond_info info_array[]) {
	return get_conds_info(info_array);
}

uint64_t sys_get_mutexes_info(mutex_info info_array[]) {
	return get_mutexes_info(info_array);
}

uint64_t sys_get_fifos_info(fifo_info info_array[]) {
	return get_fifos_info(info_array);
}


/* WRAPPERS */
/* Se usan para system calls que no reciben exactamente 3 parametros enteros.
** En la wrapper se filtran los parametros innecesarios y se hacen los casteos
** que hagan falta. De esta forma se puede tener un vector de system calls sin
** que la implementacion de ellas se vea afectada.
*/

static uint64_t sys_write_wr(uint64_t fds, uint64_t str, uint64_t length) {
	return sys_write(fds, (const char *) str, length);
}

static uint64_t sys_read_wr(uint64_t fds, uint64_t buffer, uint64_t bytes) {
	return sys_read(fds, (char *) buffer, bytes);
}

static uint64_t sys_time_wr(uint64_t selection, uint64_t unused1, uint64_t unused2) {
	return sys_time(selection);
}

static uint64_t sys_wait_wr(uint64_t selection, uint64_t unused1, uint64_t unused2) {
	return sys_wait(selection);
}

static uint64_t sys_screen_res_wr(uint64_t selection, uint64_t unused1, uint64_t unused2) {
	return sys_screen_res(selection);
}

static uint64_t sys_text_space_wr(uint64_t selection, uint64_t unused1, uint64_t unused2) {
	return sys_text_space(selection);
}

static uint64_t sys_malloc_wr(uint64_t selection, uint64_t unused1, uint64_t unused2) {
	return sys_malloc(selection);
}

static uint64_t sys_free_wr(uint64_t selection, uint64_t unused2, uint64_t unused3) {
	return sys_free(selection);
}

static uint64_t sys_data_address_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3) {
	return sys_data_address();
}

static uint64_t sys_exec_wr(uint64_t ptr, uint64_t params, uint64_t name) {
	return sys_exec(ptr, params, (const char *) name);
}

static uint64_t sys_end_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3) {
	return sys_end();
}

static uint64_t sys_yield_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3) {
	return sys_yield();
}

static uint64_t sys_mutex_op_wr(uint64_t nameptr, uint64_t unused2, uint64_t unused3) {
	return sys_mutex_op(nameptr);
}

static uint64_t sys_mutex_cl_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_mutex_cl(key);
}

static uint64_t sys_mutex_lock_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_mutex_lock(key);
}

static uint64_t sys_mutex_unlock_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_mutex_unlock(key);
}

static uint64_t sys_set_foreground_wr(uint64_t pid, uint64_t unused2, uint64_t unused3) {
	return sys_set_foreground(pid);
}

static uint64_t sys_fifo_op_wr(uint64_t nameptr, uint64_t unused2, uint64_t unused3) {
	return sys_fifo_op(nameptr);
}

static uint64_t sys_fifo_cl_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_fifo_cl(key);
}

static uint64_t sys_kill_wr(uint64_t pid, uint64_t unused2, uint64_t unused3) {
	return sys_kill(pid);
}

static uint64_t sys_pid_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3) {
	return sys_pid();
}

static uint64_t sys_ppid_wr(uint64_t unused1, uint64_t unused2, uint64_t unused3) {
	return sys_ppid();
}

static uint64_t sys_process_info_wr (uint64_t pid, uint64_t pi, uint64_t unused3) {
	return sys_process_info(pid, (struct process_info_c *) pi);
}

static uint64_t sys_cond_open_wr(uint64_t name, uint64_t unused2, uint64_t unused3) {
	return sys_cond_open((char *) name);
}

static uint64_t sys_cond_wait_wr(uint64_t cond_key, uint64_t lock_key, uint64_t unused3) {
	return sys_cond_wait(cond_key, lock_key);
}

static uint64_t sys_cond_signal_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_cond_signal(key);
}

static uint64_t sys_cond_broadcast_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_cond_broadcast(key);
}

static uint64_t sys_cond_close_wr(uint64_t key, uint64_t unused2, uint64_t unused3) {
	return sys_cond_close(key);
}

static uint64_t sys_get_pids_wr(uint64_t pid_array, uint64_t unused2, uint64_t unused3) {
	return sys_get_pids((int *) pid_array);
}

static uint64_t sys_get_conds_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3) {
	return sys_get_conds_info((cond_info *) info_array);
}

static uint64_t sys_get_mutexes_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3) {
	return sys_get_mutexes_info((mutex_info *) info_array);
}

static uint64_t sys_get_fifos_info_wr(uint64_t info_array, uint64_t unused2, uint64_t unused3) {
	return sys_get_fifos_info((fifo_info *) info_array);
}
