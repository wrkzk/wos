#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../libc/string.h"
#include "../drivers/display.h"
#include "../cpu/idt.h"
#include "../libc/mem.h"
#include "../cpu/timer.h"
#include "../drivers/ports.h"

uint32_t tick = 0;
size_t foreground;
size_t background;

/*
VGA_COLOR_BLACK = 0,
VGA_COLOR_BLUE = 1,
VGA_COLOR_GREEN = 2,
VGA_COLOR_CYAN = 3,
VGA_COLOR_RED = 4,
VGA_COLOR_MAGENTA = 5,
VGA_COLOR_BROWN = 6,
VGA_COLOR_LIGHT_GREY = 7,
VGA_COLOR_DARK_GREY = 8,
VGA_COLOR_LIGHT_BLUE = 9,
VGA_COLOR_LIGHT_GREEN = 10,
VGA_COLOR_LIGHT_CYAN = 11,
VGA_COLOR_LIGHT_RED = 12,
VGA_COLOR_LIGHT_MAGENTA = 13,
VGA_COLOR_LIGHT_BROWN = 14,
VGA_COLOR_WHITE = 15
*/

unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\', 'z', 'x', 'c', 'v', 'b',
    'n', 'm', ',', '.', '/',   0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0,
    0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char buffer[50];
size_t buffer_len = 0;
size_t cursor_x = 15;
size_t cursor_y = 9;
unsigned int current_loc = 0;
char *vidptr = (char*)0xb8000;
extern void keyboard_handler();

/**
 * It sets the IDT gate for the keyboard interrupt to the address of the keyboard handler
 */
void init_keyboard() {
	set_idt_gate(33, (unsigned long) keyboard_handler);
}

struct colorscheme {
	size_t bg;
	size_t fg;
};

struct colorscheme schemes[4];
size_t current_scheme;

/**
 * It handles the input from the user
 * 
 * @param input the input string
 */
void handle_input(char *input) {
	if (strcmp(input, "exit") == 0) {
		print_string("Bye!\n");
		terminal_set_cursor(3, cursor_y);
		__asm__ ("cli; hlt");

	} else if (strcmp(input, "uptime") == 0) {
		unsigned long total_seconds = tick / 50;
		unsigned long remainder;

		char d_ascii[256];
		char h_ascii[256];
		char m_ascii[256];
		char s_ascii[256];
				
		int d = total_seconds / 86400;
		remainder = total_seconds % 86400;

		int h = remainder / 3600;
		remainder = remainder % 3600;

		int m = remainder / 60;
		int s = remainder % 60;
				
		itoa(s, s_ascii, 10);
		itoa(m, m_ascii, 10);
		itoa(h, h_ascii, 10);
		itoa(d, d_ascii, 10);

		if (d != 0) {
			print_string(d_ascii);
			if (d == 1) print_string("day, ");
			else print_string("days, ");
		}

		if (h != 0) {
			print_string(h_ascii);
			print_string(" ");
			if (h == 1) print_string("hour, ");
			else print_string("hours, ");
		}

		if (m != 0) {
			print_string(m_ascii);
			print_string(" ");
			if (m == 1) print_string("minute, ");
			else print_string("minutes, ");
		}

		print_string(s_ascii);
		if (s == 1) print_string(" second");
		else print_string(" seconds");
		print_string("\n");
					
	} else if (strcmp(input, "clear") == 0) {
		clear_terminal();
		terminal_setcolor(vga_entry_color(foreground, background));
		cursor_y = -1;
		current_loc = -128;
	} else if (strcmp(input, "") == 0) {
		print_string("Enter a command\n");
	} else if (strcmp(input, "help") == 0) {
		print_string("\nhelp     -    show this help menu");
		print_string("\nexit     -    halt the kernel execution");
		print_string("\nclear    -    clear the terminal");
		print_string("\nscheme   -    cycle through the available color schemes");
		print_string("\nuptime   -    display the kernel uptime\n\n");
		cursor_y += 6;
	} else if (strcmp(input, "scheme") == 0) {
		current_scheme++;
		if (current_scheme == 4) {
			current_scheme = 0;
		}

		terminal_setscheme(schemes[current_scheme].fg, schemes[current_scheme].bg);
		background = schemes[current_scheme].bg;
		foreground = schemes[current_scheme].fg;

		print_string("Switched to colorscheme: ");
		if (current_scheme == 0) {
			print_string("classic\n");
		} else if (current_scheme == 1) {
			print_string("inverted\n");
		} else if (current_scheme == 2) {
			print_string("retro\n");
		} else if (current_scheme == 3) {
			print_string("hacker\n");
		}
	} else {
		print_string(input);
		print_string(": command not found\n");
	}
}

/**
 * We read the keyboard input, and if it's a newline character, we read the buffer
 * to see if it matches one of the commands, otherwise we add it to the buffer
 */
void keyboard_handler_main(void) {
	current_loc = (cursor_y * 80 * 2) + (cursor_x * 2) + 2;

	char keycode;
	unsigned char status = inb(0x64);

	outb(0x20, 0x20);

	if (status & 0x01) {
		keycode = inb(0x60);

		if (keycode < 0) {
			return;
		} else if (keycode == 0x1C) {
			print_string("\n");

			cursor_x = 15;
			cursor_y++;
			current_loc += (80 * 2) - (buffer_len * 2);

			handle_input(buffer);

			memset(buffer, 0, buffer_len);
			buffer_len = 0;

			cursor_y++;

			print_string("warren@kernel~$ ");

			current_loc += (80 * 2) - (buffer_len * 2);

			terminal_set_cursor(cursor_x, cursor_y);

			return;
		} else if (keycode == 0x0E) {
			if (cursor_x <= 15) {
				return;
			}

			current_loc -= 2;
			cursor_x--;
			
			vidptr[current_loc] = keyboard_map[(unsigned char) 0x39];

			terminal_set_cursor(cursor_x, cursor_y);



			buffer_len -= 1;
			buffer[buffer_len] = "\0";

			return;
		}

		buffer[buffer_len++] = keyboard_map[(unsigned char) keycode];

		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = vga_entry_color(foreground, background);
		terminal_set_cursor(cursor_x + 1, cursor_y);
		cursor_x++;
	}
}

/**
 * It sets up the PICs
 */
void irq_install() {
	
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	idt_install();
}

/**
 * It initializes the terminal, installs the IRQs, initializes the timer, initializes the keyboard, and
 * then waits for the user to type something
 */
void kmain(void) {
	clear_terminal();
	terminal_setcolor(vga_entry_color(foreground, background));

	print_string("Welcome to Warren's Operating System (WOS)\n\n");
	print_string("Initializing display...    ");
	print_string("done\n");

	print_string("Setting up IRQs...         ");
	irq_install();
	print_string("done\n");

	print_string("Initializing timer...      ");
	init_timer(50);
	print_string("done\n");

	print_string("Initializing keyboard...   ");
	init_keyboard();
	print_string("done\n\n");

	print_string("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~");

	print_string("\nType 'help' for a list of commands.");
	print_string("\nwarren@kernel~$ ");

	terminal_set_cursor(cursor_x, cursor_y);


	schemes[0].bg = 0;
	schemes[0].fg = 7;

	schemes[1].bg = 7;
	schemes[1].fg = 0;

	schemes[2].bg = 11;
	schemes[2].fg = 12;

	schemes[3].bg = 0;
	schemes[3].fg = 2;

	current_scheme = 0;
	terminal_setscheme(schemes[current_scheme].fg, schemes[current_scheme].bg);
	foreground = schemes[current_scheme].fg;
	background = schemes[current_scheme].bg;
	
	for(;;) {
		asm("hlt");
	}
}