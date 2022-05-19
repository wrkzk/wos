extern keyboard_handler_main
extern timer_handler_main

global inb
global outb
global keyboard_handler
global load_idt
global timer_handler

; Read data from a port
inb:
    mov edx, [esp + 4]
    in al, dx
    ret

; Write data to a port
outb:
    mov al, [esp + 8]
    mov edx, [esp + 4]
    out dx, al
    ret

; Load the IDT (Interrupt Descriptor Table)
load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    sti
    ret

; Call the keyboard handler
keyboard_handler:
    call keyboard_handler_main
    iretd

; Call the timer handler
timer_handler:
    call timer_handler_main
    iretd