#include <stdint.h>
#include "idtLoader.h"
#include "defs.h"
#include "interrupts.h"

DESCR_INT idt[255];	// IDT de 255 entradas
IDTR idtr;			// IDTR

void _lidt (IDTR *i);
void setup_IDT_entry (int index, uint8_t selector, uint64_t offset, uint8_t access);

void load_idt()
{

	setup_IDT_entry (0x20, 0x08, (uint64_t)&_irq00Handler, ACS_INT);
	setup_IDT_entry (0x21, 0x08, (uint64_t)&_irq01Handler, ACS_INT);
	setup_IDT_entry (0x80, 0x08, (uint64_t)&_syscallHandler, ACS_INT);
	setup_IDT_entry (0x70, 0x08, (uint64_t)&_yield_interrupt, ACS_INT);


	idtr.base = 0;
	idtr.base += (uint64_t) &idt;
	idtr.limit = (uint16_t) sizeof(idt)-1;
	
	_lidt(&idtr);	


	//Solo interrupcion de teclado y timer tick habilitadas 
	picMasterMask(0xFC); 
	picSlaveMask(0xFF);
        
	_sti();

	
}

void setup_IDT_entry (int index, uint8_t selector, uint64_t offset, uint8_t access) {
  idt[index].selector = selector;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = access;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}


