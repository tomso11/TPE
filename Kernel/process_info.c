#include "process_info.h"
#include "strings.h"
#include "stdlib.h"
#include "stdio.h"


#define SEPARATOR ' '
#define FOREGROUND "foreground"
#define BACKGROUND "background"
#define STACK_ADDR "Stack Address: "
#define MEM_ADDR "Memory Address: "
#define PPID "PPID"


static char * state[] = {"RUNNING", "READY", "BLOCKED"};

static int strcpysep (char * to, const char * source, char separator);
static int has_mem_pages(const process_info * p);

int process_string(const process_info * p, char buffer[MAX_PROCESS_STRING]) {
	int i = 0;
	char * ground = p->is_foreground ? FOREGROUND : BACKGROUND;

	i += itoa(p->pid, buffer + i, 10);

	buffer[i++] = SEPARATOR;

	i += strcpysep(buffer + i, p->name, SEPARATOR); /* nombre */

	i += strcpysep(buffer + i, PPID, SEPARATOR);

	i += itoa(p->ppid, buffer + i, 10);

	buffer[i++] = SEPARATOR;

	i += strcpysep(buffer + i, ground, SEPARATOR); /* [background|foregound] */

	i += strcpysep(buffer + i, state[(int)p->st], SEPARATOR); /* [RUNNING|READY|BLOCKED]*/

	i += strcpysep(buffer + i, STACK_ADDR, SEPARATOR);

	i += itoa((uint64_t) p->stack_address, buffer + i, 16);

	buffer[i++] = SEPARATOR;

	if (has_mem_pages(p)) {
		int j;

		i += strcpysep(buffer + i, MEM_ADDR, SEPARATOR);

		for (j = 0; p->mem_pages[j] != NULL; j++) {
			i += itoa((uint64_t)p->mem_pages[j], buffer + i, 16);
			if (p->mem_pages[j+1] != NULL) {
				buffer[i++] = ',';
				buffer[i++] = ' ';
			}
		}
	}

	buffer[i] = '\0';

	return i;
}

static int strcpysep (char * to, const char * source, char separator) {
	int len = strcpy(to, source);
	to[len++] = separator;
	to[len] = '\0';

	return len;
}

static int has_mem_pages(const process_info * p) {
	return p->mem_pages[0] != NULL;
}


int get_process_info_by_pid(process_info * pi, uint64_t pid) {
	process * p = get_process_by_pid(pid);
	if (p == NULL)
		return 0;
	get_process_info(pi, p);
	return 1;
}

void get_process_info (process_info * pi, process * p) {
	assign_quantum();
	
	pi->is_foreground = get_foreground_process() == p;

	if (p == get_current_process())
		pi->st = RUNNING;
	else
		pi->st = is_blocked_process(p) ? BLOCKED : READY;

	pi->pid = pid_process(p);
	pi->ppid = ppid_process(p);

	get_name_process(pi->name, p);

	pi->stack_address = stack_page_process(p);
	data_pages_process(p, pi->mem_pages);
	
	unassign_quantum();
}