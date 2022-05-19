#ifndef IDT_H
#define IDT_H

struct idt_gate {
	unsigned short int low_offset;
	unsigned short int selector;
	unsigned char all0;
	unsigned char flags;
	unsigned short int high_offset;
} __attribute__ ((packed));

struct idtr_t {
	unsigned short limit;
	unsigned int base;
} __attribute__ ((packed));

struct idt_gate idt[256];
struct idtr_t idtr;

extern void load_idt(unsigned long *idt_ptr);
void set_idt_gate(int n, unsigned int handler);
void idt_install();

#endif