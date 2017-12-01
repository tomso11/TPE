#include "memoryAllocator.h"
#include "init.h"
#include "dirs.h"
#include "processManager.h"
#include "videoDriver.h"
#include "process.h"
#include "systemCalls.h"
#include "shell.h"

static void * const sampleCodeModuleAddress = (void *) CODE_ADDRESS;

void _hlt();
void _sti();

void init() {
	initialize_memory_allocator_mutex();
	initialize_stack_memory_allocator_mutex();
	initialize_process_mutex();
	
	_sti();
	//sys_exec((uint64_t)sampleCodeModuleAddress, 0, "shell");
	sys_exec((uint64_t) shell, 0, "shell");
	set_foreground_process (get_process_by_pid(1));
	while (1) {
		_hlt();
	}
}