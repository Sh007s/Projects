/* 
 * File:   a24c02.h
 * Author: shan1234
 */ 

#ifndef A24C02_H
#define	A24C02_H


#define SLAVE_READ_EE          0xA1
#define SLAVE_WRITE_EE         0xA0

unsigned char read_24c02(unsigned char addr);
void write_24c02(unsigned char addr, unsigned char data); 

void write_str_24c02(unsigned char addr,  char *data);

#endif	/* A24C02_H */

