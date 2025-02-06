
/*
Name	    : Shankar s
Date	    : 28/04/2023
Project     : Car Black Box
Description : This project is used to display the current event of the car and also to store the event in the external eeprom
*/

#include <xc.h>
#include "i2c.h"
#include "ds1307.h"
#include "adc.h"
#include "timers.h"
#include "a24c02.h"
#include "uart.h"
#include "clcd.h"
#include "matrix_keypad.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

extern unsigned int time_count_isr;

char buff[3];
char time[9];  // "HH:MM:SS"
void display(int adc_reg_val_speed)
{ 
    int i;
    i = 1; // buff: "1 0 2 3"
    do
    {
        buff[i] = (adc_reg_val_speed % 10) + '0';//10%10 -> 0 + '0' 
        adc_reg_val_speed = adc_reg_val_speed / 10; //0
    } while (i--); // 0
    buff[2] = '\0';
    clcd_print(buff, LINE2(14));
}

static void display_time(unsigned char *clock_reg)
{
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';   
    time[2] = ':';
    // MM 
    time[3] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[4] = (clock_reg[1] & 0x0F) + '0';
 
    time[5] = ':';
    // SS
    time[6] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[7] = (clock_reg[2] & 0x0F) + '0';
    time[8] = '\0';
    
    clcd_print(time, LINE2(0)); // HH:MM:SS 
}

static void get_time(unsigned char *clock_reg)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
}

static void init_config(void) {
    init_clcd();
    init_matrix_keypad();
    init_i2c(100000); //100K
    init_ds1307();
    init_adc();
    init_uart(9600);
    init_timer0();
    /* Enable all the Global Interrupts */
    GIE = 1;   
}

int check_password()
{
    unsigned char org_password = read_24c02(0x04);    
    clcd_print("                ", LINE2(0));
    unsigned char temp_password = 0, key_state = ALL_RELEASED, count = 0;
    while(1)
    {
        key_state = read_matrix_keypad(STATE);
        
        if(count == 4)
        {
            break;
        }
        if(key_state == 5)
        {
            clcd_putch('*', LINE2(6+count));
            temp_password = (temp_password<<1) | 0;
            count++;
            continue;
        }
        if(key_state == 6)
        {
            clcd_putch('*', LINE2(6+count));
            temp_password = (temp_password<<1) | 1;
            count++;
            continue;
        }
        
        if(time_count_isr < 10000)
        {
            clcd_putch('_', LINE2(6+count));
        }
        else
        {
           clcd_putch(' ', LINE2(6+count)); 
        }
    }
    if(org_password == temp_password )
        return 1;
    return 0;
 }
 
 void write_external_eeprom(unsigned int event_idx,unsigned int addr, char *gear)
{
    
    write_24c02(addr, event_idx + '0' );
    addr++;
    write_24c02(addr, '_' ); 
    addr++;
    
    for(char i = 0; i < 8; i++)
    {
        write_24c02(addr, time[i] );
        addr++;
    }
    
    write_24c02(addr, ' ' );
    addr++;
    write_24c02(addr, gear[0] );
    addr++;
    write_24c02(addr, gear[1] );
    addr++;
    write_24c02(addr, ' ' );
    addr++;
    write_24c02(addr, buff[0] );
    addr++;
    write_24c02(addr, buff[1] ); 
}

void main(void) {
    init_config();
    unsigned char key_state,key_level;
    unsigned char clock_reg[3]; // HH MM SS
                               // 0   1  2 
    
    char *gear[7] = {"G1","G2","G3","G4","G5","GR","GN"};
    unsigned char org_password = 0x00;
    write_24c02(0x04, org_password);  
    unsigned int addr[10] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0};
    unsigned int event_count = 0,event_idx = 0;
    int gear_idx = -1,gear_flag = 0,menu_flag = 0,menu_select = 0 ,password_flag = 0 ;  
   
    while (1) {
        clcd_print("Time      G   SP", LINE1(0));
        key_state = read_matrix_keypad(STATE);
        key_level = read_matrix_keypad(LEVEL);  // 0 1 2 3 4 5 6 7 8 9 '*' '#'  ALL_RELEASED
        
        unsigned  int adc_reg_val_speed;
        adc_reg_val_speed = (read_adc(CHANNEL0)  / (float)1024) * 100; // pot1 
        display(adc_reg_val_speed);
        
        
        get_time(clock_reg); // HH MM SS   
        display_time(clock_reg); // HH:MM:SS --> 13:14:15
   
        if(key_state == 4)
        {
            menu_flag = !menu_flag;
            clcd_print("                ", LINE1(0));
            clcd_print("                ", LINE2(0));
            key_state = ALL_RELEASED;
            password_flag = 0;
            menu_select = 0;
        }
        
        if(menu_flag == 0) 
        {
            
            if(gear_flag == 0)
            {
                clcd_print("ON", LINE2(10));

            }

            if(key_state == 1)
            {
                clcd_print("C ", LINE2(10));
                gear_flag = 1;
                gear_idx = -1;
                
                write_external_eeprom(event_idx, addr[event_idx], "C_");
                event_count++;
                if(++event_idx == 10)
                {
                    event_idx = 0;
                }
            }


            if(key_state == 2)
            {
                gear_flag = 1;
                if(++gear_idx == 7)
                {
                    gear_idx = 0;
                }
                clcd_print(gear[gear_idx], LINE2(10));
                
                write_external_eeprom(event_idx, addr[event_idx],gear[gear_idx] );
                event_count++;
                if(++event_idx == 10)
                {
                    event_idx = 0;
                }
            }

            if(key_state == 3)
            {
                gear_flag = 1;
                if(--gear_idx <= -1)
                {
                    gear_idx = 6;
                }
                clcd_print(gear[gear_idx], LINE2(10));
                
                write_external_eeprom(event_idx, addr[event_idx],gear[gear_idx] );
                event_count++;
                if(++event_idx == 10)
                {
                    event_idx = 0;
                }
            }
        }
        
        
        else
        {
            clcd_print(" Enter Password ", LINE1(0));
            if(password_flag == 0 && check_password() != 1)   
            {
                clcd_print(" Wrong Password ", LINE1(0));
                clcd_print("                ", LINE2(0));
                for(unsigned int  i = 0; i < 500; i++ )
                    for(unsigned int  j = 0; j < 500; j++ );
                menu_flag = 0;
                continue;
            }
                password_flag = 1;
                gear_flag = 0;
            
                unsigned int time_count_increment = 0;
                while(1)
                {
                    key_state = read_matrix_keypad(STATE);
                    key_level = read_matrix_keypad(LEVEL);

                    if(key_state == 4)
                    {
                        menu_flag = !menu_flag;
                        clcd_print("                ", LINE1(0));
                        clcd_print("                ", LINE2(0));
                        menu_select = ALL_RELEASED;
                        break;
                    }

                    if(key_level == 5) 
                    {
                       time_count_increment++; 
                        if(time_count_increment > 10)
                        {
                            time_count_increment = 0;
                            break;
                        }
                    }
                    else if(time_count_increment != 0)
                    {
                        if(++menu_select == 5)
                            menu_select = 0;
                        time_count_increment = 0;

                    }
                    if(key_state == 6)
                    {
                        if(--menu_select == -1)
                                menu_select = 4;
                    }

                    switch(menu_select)
                    {
                        case 0:
                        {
                            if(time_count_isr >= 10000)
                            {
                                clcd_print("   MENU         ", LINE1(0));
                                clcd_print("   View log     ", LINE2(0));
                            }
                            else
                            {
                                clcd_print("   MENU         ", LINE1(0));
                                clcd_print("-> View log     ", LINE2(0));   
                            }

                            break;
                        }
                        case 1:
                        {
                            if(time_count_isr >= 10000)
                            {
                                clcd_print("   Set time     ", LINE1(0));
                                clcd_print("   Change passwd", LINE2(0));
                            }
                            else
                            {
                                clcd_print("-> Set time     ", LINE1(0));
                                clcd_print("   Change passwd", LINE2(0));
                            }

                            break;

                        }
                        case 2:
                        {
                            if(time_count_isr >= 10000)
                            {
                                clcd_print("   Set time     ", LINE1(0));
                                clcd_print("   Change passwd", LINE2(0));
                            }
                            {
                                clcd_print("   Set time     ", LINE1(0));
                                clcd_print("-> Change passwd", LINE2(0));

                            }

                            break;
                        }

                        case 3:
                        {
                            if(time_count_isr >= 10000)
                            {
                                clcd_print("   Clear log    ", LINE1(0));
                                clcd_print("   download log ", LINE2(0));
                            }
                            else
                            {
                                clcd_print("-> Clear log    ", LINE1(0));
                                clcd_print("   download log ", LINE2(0));

                            }

                            break;

                        }
                        case 4:
                        {
                            if(time_count_isr >= 10000)
                            {
                                clcd_print("   Clear log    ", LINE1(0));
                                clcd_print("   download log ", LINE2(0));
                            }
                            else
                            {
                                clcd_print("   Clear log    ", LINE1(0));
                                clcd_print("-> download log ", LINE2(0));

                            }
                            break;

                        }

                    }

                }

                if(menu_select == 0)
                {
                    if(event_count == 0)
                    {
                        clcd_print("     Empty Log   ", LINE2(0));
                        for(unsigned int  i = 0; i < 500; i++ )
                            for(unsigned int  j = 0; j < 500; j++ );
                            
                        continue;
                        
                    }
                    unsigned int time_count_decrement = 0,log_select = 0;
                    while(1)
                    {
                        if(event_count > 10)
                        {
                            event_count = 10;
                        }
                        key_state = read_matrix_keypad(STATE);
                        key_level = read_matrix_keypad(LEVEL);

                        if(key_state == 4)
                        {
                            menu_flag = !menu_flag;
                            clcd_print("                ", LINE1(0));
                            clcd_print("                ", LINE2(0));
                            break;
                        }

                        if(key_level == 6) 
                        {
                           time_count_decrement++; 
                            if(time_count_decrement > 10)
                            {
                                time_count_decrement = 0;
                                break;
                            }
                        }
                        else if(time_count_decrement != 0)
                        {
                            if(--log_select == -1)
                                log_select = event_count;
                            time_count_decrement = 0;

                        }
                        if(key_state == 5)
                        {
                            if(++log_select == event_count)
                                    log_select = 0;
                        }

                        unsigned char log_display[17];
                        for(char i = 0; i < 16; i++)
                        {
                            log_display[i] = read_24c02(addr[log_select] + i);
                        }
                        log_display[16] = '\0';
                        clcd_print("  Time     G  SP", LINE1(0));
                        clcd_print(log_display, LINE2(0));
                   }
                }

                else if(menu_select == 1)
                {
                    clcd_print("Time      G   SP", LINE1(0));
                    clcd_print("                ", LINE2(0));

                    clcd_print(gear[gear_idx], LINE2(10));

                    display(adc_reg_val_speed);

                    int field_select = 2,time_count_increment = 0;
                    while(1)
                    {
                        key_state = read_matrix_keypad(STATE);
                        key_level = read_matrix_keypad(LEVEL);

                        if(key_state == 5)
                        {
                            if(--field_select == -1)
                                    field_select = 2;
                        }



                        if(key_level == 6) 
                        {
                           time_count_increment++; 
                            if(time_count_increment > 100)
                            {
                                menu_flag = 0;
                                break;
                            }
                        }
                        else if(time_count_increment != 0)
                        {
                            if(++(clock_reg[field_select]) == 100)
                            {
                                clock_reg[field_select] = 0;
                            }

                            time_count_increment = 0;

                            write_ds1307(HOUR_ADDR,clock_reg[0]);
                            write_ds1307(MIN_ADDR,clock_reg[1]);
                            write_ds1307(SEC_ADDR,clock_reg[2]);

                        }

                        if(time_count_isr < 10000)
                        {
                            switch(field_select)
                            {
                                case 0:
                                {
                                   clcd_print("__",LINE2(0));
                                   break;
                                }

                                case 1:
                                {
                                   clcd_print("__",LINE2(3));
                                   break;
                                }

                                case 2:
                                {
                                   clcd_print("__",LINE2(6));
                                   break;
                                }
                            }
                        }

                        else
                        {
                            display_time(clock_reg);
                        }

                    }

                }
                

                else if(menu_select == 2)
                {
                    clcd_print("Entr prev Passwd", LINE1(0));
                    if(check_password() == 1)
                    {
                       clcd_print("Enter new Passwd", LINE1(0)); 
                       clcd_print("                ", LINE2(0));
    
                        unsigned char temp_password = 0, key_state = ALL_RELEASED, count = 0;

                        while(1)
                        {
                            key_state = read_matrix_keypad(STATE);

                            if(count == 4)
                            {
                                break;
                            }
                            if(key_state == 5)
                            {
                                clcd_putch('*', LINE2(6+count));
                                temp_password = (temp_password<<1) | 1;
                                count++;
                                continue;
                            }
                            if(key_state == 6)
                            {
                                clcd_putch('*', LINE2(6+count));
                                temp_password = (temp_password<<1) | 0;
                                count++;
                                continue;
                            }

                            if(time_count_isr < 10000)
                            {
                                clcd_putch('_', LINE2(6+count));
                            }
                            else
                            {
                               clcd_putch(' ', LINE2(6+count)); 
                            }
                        }
                        write_24c02(0x04, temp_password);
                        
                        clcd_print("  New Password  ", LINE1(0));
                        clcd_print("   Successful   ", LINE2(0));
                        for(unsigned int  i = 0; i < 500; i++ )
                            for(unsigned int  j = 0; j < 500; j++ );
                    }
                    else
                    {
                        clcd_print(" Wrong Password ", LINE1(0));
                        clcd_print("                ", LINE2(0));
                        for(unsigned int  i = 0; i < 500; i++ )
                            for(unsigned int  j = 0; j < 500; j++ );
                    }
                }
                else if(menu_select == 3)
                {
                    event_count = 0;
                    event_idx = 0;
                    clcd_print("  log cleared   ", LINE1(0));
                    clcd_print("  successfully  ", LINE2(0));
                        for(unsigned int  i = 0; i < 500; i++ )
                            for(unsigned int  j = 0; j < 500; j++ );
                }
                else
                {
                    uart_puts("     Time     G  SP\n\r");
                    unsigned char log_display[19];
                    for(unsigned char j = 0; j < event_count; j++)
                    {
                        for(char i = 0; i < 16; i++)
                        {
                            log_display[i] = read_24c02(addr[j] + i);
                        }
                        log_display[16] = '\n';
                        log_display[17] = '\r';
                        log_display[18] = '\0';   
                        uart_puts(log_display);
                    }
                }
        }
    }
    return;
}
