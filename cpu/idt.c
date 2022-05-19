#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "idt.h"

/**
 * It sets the nth entry in the IDT to point to the function at address handler
 * 
 * @param n The interrupt number.
 * @param handler The address of the function to be called when the interrupt occurs.
 */
void set_idt_gate(int n, unsigned int handler) {
	idt[n].low_offset = (unsigned short) ((handler) & 0xFFFF);
	idt[n].selector = 0x08;
	idt[n].all0 = 0;
	idt[n].flags = 0x8E;
	idt[n].high_offset = (unsigned short) ((handler >> 16) & 0xFFFF);
}

/**
 * It sets up the IDT pointer, and then loads the IDT
 */
void idt_install() {
    unsigned long idt_ptr[2];
	unsigned long idt_address = (unsigned long) idt;

	idt_ptr[0] = (sizeof (struct idt_gate) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address  >> 16;

	load_idt(idt_ptr);
}