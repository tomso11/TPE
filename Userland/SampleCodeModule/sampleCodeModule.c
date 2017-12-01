#include <stdint.h>
#include "stdlib.h"
#include "fractals.h"
#include "syscalls.h"

extern char bss;
extern char endOfBinary;

int shell();

int main() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);

	load_fractals(data_address());
    set_fractals_resolution(screen_Xresolution(), screen_Yresolution()); // fractales en resolucion bien manija

	shell();

	return 0;
}
