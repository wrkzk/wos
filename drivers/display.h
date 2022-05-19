#ifndef DISPLAY_H
#define DISPLAY_H



static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_setcolor(uint8_t color);
uint8_t vga_entry_color(size_t fg, size_t bg);
void terminal_setscheme(size_t fg, size_t bg);
void clear_terminal(void);
void terminal_putchar(char c);
void print_string(const char* data);
void terminal_set_cursor (int x, int y);

#endif