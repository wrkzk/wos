#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "display.h"
#include "../libc/string.h"
#include "ports.h"
#include "display.h"

enum vga_color {
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
	VGA_COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t foreground = 2;
size_t background = 0;


size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

/**
 * It takes a character and a color, and returns a 16-bit value that can be written to the VGA buffer
 * 
 * @param uc The character to print
 * @param color The color of the character.
 * 
 * @return A 16-bit value that is the character and the color.
 */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

/**
 * It takes a character, a color, and a position, and writes the character and color to the terminal
 * buffer at the given position
 * 
 * @param c The character to print
 * @param color The color of the character.
 * @param x The x coordinate of the character to be printed.
 * @param y The row of the terminal
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/**
 * It sets the color of the terminal
 * 
 * @param color The color to set the terminal to.
 */
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

/**
 * It takes two colors, and returns a byte that represents the two colors
 * 
 * @param fg foreground color
 * @param bg background color
 * 
 * @return The foreground color and the background color.
 */
uint8_t vga_entry_color(size_t fg, size_t bg) {
	return fg | bg << 4;
}

/**
 * It sets the color scheme of the terminal
 * 
 * @param fg foreground color
 * @param bg The background color
 */
void terminal_setscheme(size_t fg, size_t bg) {
	//background = bg;
	//foreground = fg;
	terminal_setcolor(vga_entry_color(fg, bg));

	char *vidptr = (char*)0xb8000;
	for (size_t pos = 1; pos < VGA_HEIGHT * VGA_WIDTH * 2; pos += 2) {
		vidptr[pos] = vga_entry_color(fg, bg);
	}
}

/**
 * It clears the terminal by setting the terminal row and column to 0, setting the terminal color to a
 * light grey background with a black foreground, setting the terminal buffer to the VGA buffer, and
 * then looping through the VGA buffer and setting each character to a space with the terminal color
 */
void clear_terminal(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, 3);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', vga_entry_color(foreground, background));
		}
	}
}

/**
 * If the character is a newline, increment the row and reset the column. Otherwise, put the character
 * at the current column and row, and increment the column. If the column is at the end of the row,
 * increment the row and reset the column. If the row is at the end of the screen, scroll the screen up
 * one row
 * 
 * @param c The character to print
 * 
 * @return the character that is being printed.
 */
void terminal_putchar(char c) {
	switch (c) {
	case '\n':
		terminal_row++;
		terminal_column = 0;
		return;
	default:
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	}
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
	if (terminal_row == VGA_HEIGHT) {
		terminal_row--;
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t next_index = (y + 1) * VGA_WIDTH + x;
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = terminal_buffer[next_index];
			}
		}
	}
}

/**
 * It takes a pointer to a string, and prints it to the screen
 * 
 * @param data The string to print
 */
void print_string(const char* data) {
	for (size_t i = 0; i < strlen(data); i++) {
		terminal_putchar(data[i]);
	}
}

/**
 * It takes the x and y coordinates of the cursor, multiplies the y coordinate by the width of the
 * screen, adds the x coordinate, and then adds one to the result. This is because the first character
 * in the VGA buffer is reserved for the cursor
 * 
 * @param x The x coordinate of the cursor.
 * @param y The row of the cursor.
 */
void terminal_set_cursor (int x, int y) {
	uint16_t pos = y * VGA_WIDTH + x + 1;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}