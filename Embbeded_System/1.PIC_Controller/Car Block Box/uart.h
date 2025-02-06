/* 
 * File:   uart.h
 * Author: shan1234
 *
 * Created on 9 May, 2023, 2:03 PM
 */

#ifndef UART_H
#define	UART_H

#define FOSC                20000000

void init_uart(unsigned long baud);
unsigned char uart_getchar(void);
void uart_putchar(unsigned char data);
void uart_puts(const char *s);


#endif	/* UART_H */

