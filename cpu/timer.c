#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "timer.h"
#include "../cpu/idt.h"
#include "../drivers/ports.h"

/**
 * It sets up the timer interrupt handler, and then sets the timer to interrupt at a rate of `hz` times
 * per second
 * 
 * @param hz The frequency of the timer.
 */
void init_timer(uint32_t hz) {
	set_idt_gate(32, (unsigned long) timer_handler);
		
	uint32_t divisor = 1193180 / hz;
	uint8_t low = (uint8_t) (divisor & 0xFF);
	uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

	outb(0x43, 0x36);
	outb(0x40, low);
	outb(0x40, high);
}

/**
 * Increments the number of ticks each time the timer interrupts
 */
void timer_handler_main() {
	outb(0x20, 0x20);
	tick++;
}