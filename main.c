#include <stdint.h>

#define UART_BASE ((volatile uint8_t*)0x90000000)

void uart_putchar(char c) {
    *UART_BASE = c;
}

// Make sure _start is the entry point
void _start() {
    while (1) {
        uart_putchar('S');
        uart_putchar('H');
        uart_putchar('A');
        uart_putchar('K');
        uart_putchar('T');
        uart_putchar('I');
        uart_putchar('\r');
        uart_putchar('\n');

        for (volatile int i = 0; i < 100000; i++); // simple delay
    }
}
