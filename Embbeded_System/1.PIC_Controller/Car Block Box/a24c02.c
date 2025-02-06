/*
 * File:   a24c02.c
 * Author: shan1234
 *
 */


#include <xc.h>
#include "i2c.h"
#include "a24c02.h"

unsigned char read_24c02(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(SLAVE_WRITE_EE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EE);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void write_24c02(unsigned char addr, unsigned char data) 
{
    i2c_start();
    i2c_write(SLAVE_WRITE_EE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
    
  //  __delay_ms(3);
}

void write_str_24c02(unsigned char addr,  char *data)
{
    while(*data != 0)
    {
        write_24c02(addr, *data);
        data++;
        addr++;
    }
}