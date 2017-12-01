#include <stdint.h>
#include "videoDriver.h"
#include "keyboardDriver.h"
#include "systemCalls.h"
#include "timer.h"

static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
	}
	return;
}

void int_20() {
	timer_handler();
}

void int_21() {
	store_scancode();
}
