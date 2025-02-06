/*
 * File:   isr.c
 * Author: shan1234
  */

#include <xc.h>

unsigned int time_count_isr = 0;
void __interrupt() isr(void)
{    
    if (TMR0IF == 1)
    {
        /* TMR0 Register valur + 6 (offset count to get 250 ticks) + 2 Inst Cycle */
        TMR0 = TMR0 + 6 + 2;
        
        if (++time_count_isr == 20000)
        {
            time_count_isr = 0;
           
        }
        
        TMR0IF = 0;
    }
}