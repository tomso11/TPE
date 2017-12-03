#include "strings.h"
#include "lib.h"
#include "stdio.h"
#include "executer.h"

#define MAX_ARGS 32

#define PAGE 0x1000

#define NULL ((void*)0)

typedef int (*entry_point) (int, char **);

static void executer(void ** params);
static void build_arguments(const char * arg, char * arg_strings, char * arguments[]);

int execpn(void * function_ptr, const char * name) {
	return execp(function_ptr, "", name);
}

int execp(void * function_ptr, const char * arg, const char * name) {
	void * memory = sys_malloc(PAGE);
	char ** arguments = memory;
	char * arg_strings = memory + MAX_ARGS * sizeof(char *);
	int pid;

	arguments[0] = (void *) function_ptr;

	build_arguments(arg, arg_strings, arguments + 1);

	pid = sys_exec(executer, (uint64_t) memory, name);

	sys_yield();

	sys_free(memory);

	return pid;
}

static void executer(void ** params) {
	void ** memory = sys_malloc(PAGE);
	entry_point function;
	char ** argv;
	int argc;
	/* int ret_value; */

	memcpy(memory, params, PAGE);

	function = memory[0];
	argv = (char **) memory + 1;
	argc = 0;

	while (argv[argc] != NULL)
		argc++;

	/* Si bien no se ultiliza el valor de retorno, se deja explicito como seria su uso */
	/* ret_value = */ (*function)(argc, argv);

	sys_free(memory);
	sys_set_foreground(sys_ppid());
	sys_end();
}

static void build_arguments(const char * arg, char * arg_strings, char * arguments[]) {
	int i = 0;
	int j;
	char * ptr;

	if (arg[0] == '\0') {
		arguments[0] = NULL;
		return;
	}

	ptr = arg_strings;

	for (j = 0; arg[j] != '\0'; j++) {
		if (arg[j] == ' ') {
			arg_strings[j] = '\0';
			arguments[i++] = ptr;
			ptr = arg_strings + j + 1;
		}
		else
			arg_strings[j] = arg[j];
	}

	arg_strings[j] = '\0';
	arguments[i++] = ptr;
	arguments[i] = NULL;
}
