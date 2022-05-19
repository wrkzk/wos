#ifndef TIMER_H
#define TIMER_H

uint32_t tick;

void init_timer(uint32_t hz);
void timer_handler_main();
void print_formatted_uptime();
extern void timer_handler();

#endif