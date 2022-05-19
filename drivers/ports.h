#ifndef PORTS_H
#define PORTS_H

extern char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);

#endif