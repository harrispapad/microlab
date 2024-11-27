#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>
#include <string.h>
#include <xc.h>

#include "setup_LCD_PEX.h"
#include "setup_TWI.h"
#include "setup_OWI.h"
#include "setup_USART.h"
#include "setup_KEYBOARD.h"

static uint8_t temp_l, temp_h;
char output[4];
extern uint16_t pressed_keys;

int16_t measure_temp();
uint16_t read_adc();

typedef enum {
    NURSE_CALL,         // 0
    OK,                 // 1
    CHECK_TEMP,         // 2
    CHECK_PRESSURE      // 3
} STATUS_t;

void setup() {  
    twi_init();
    _delay_ms(50);  // Delay 500 ms for better stability
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // Set as output
    lcd_init();
    _delay_ms(50);
    lcd_clear_display();
    _delay_us(100);
    usart_init(103);
    ADMUX = 0b01000010;   // ADC right-adjusted, select ADC2
    ADCSRA = 0b10000111;  // Enable ADC with a prescaler of 128
    setup_keyboard();
    _delay_ms(50);  // Delay 500 ms for better stability

}

void comms()
{
    char message1[] = "ESP:connect";
    char message2[] = "ESP:url:\"http://192.168.1.250:5000/data\"";
    char c;
    for(int i = 0; i < 11; i++) {
        usart_transmit(message1[i]);
        _delay_us(10);
    }
    usart_transmit('\n');
        
    lcd_data('1');
    lcd_data('.');
    do { 
        c = usart_receive();
            if(c != '\n')
                lcd_data(c);
        }
        while(c != '\n');
        
        lcd_set_cursor(1, 0);

        for(int i = 0; i < 40; i++) {
            usart_transmit(message2[i]);
            _delay_us(10);
        }
        usart_transmit('\n');
        
        lcd_data('2');
        lcd_data('.');
        do { 
            c = usart_receive();
            if(c != '\n')
                lcd_data(c);
        } while(c != '\n');        

}

static   int num = 0;

int main()
{
    setup();
    

    uint16_t adc;
    STATUS_t status;
    int temperature;
    int del = 0;

    char pressure[4];
    
    
    while(1){
        del = 0;
        lcd_set_cursor(0, 0);
        status = OK;  
        adc = read_adc();
        adc = (adc * 20) / 1024;
        
        comms();
        
        temperature = measure_temp();
        _delay_us(10);
        while(del < 500) {
            scan_keypad_rising_edge();
            if (pressed_keys != 0) {
                num = keypad_to_ascii();
                if( num == 48) 
                    status = NURSE_CALL;
                else if(num == 35)
                    status = OK;
            }
            _delay_us(100);
            del++;
        }
        if((adc < 4 || adc > 12) && status != NURSE_CALL)
            status = CHECK_PRESSURE;
        else if ((temperature < 34 || temperature > 37) && status != NURSE_CALL)
            status = CHECK_TEMP;
        
        lcd_clear_display();
        lcd_data('T');
        lcd_data(':');
        for (int i = 0; i < 4; i++)
            lcd_data(output[i]);
        lcd_data(' ');
        lcd_data('P');
        lcd_data(':');
        sprintf(pressure, "%.1f", (double)adc);
        for (int i = 0; i < 4; i++)
            lcd_data(pressure[i]);
        
        lcd_set_cursor(1, 0);
        char message[16];
        switch(status) {
            case NURSE_CALL:
                sprintf(message, "NURSE CALL");
                break;
            case CHECK_TEMP:
                sprintf(message, "CHECK TEMP");
                break;
            case CHECK_PRESSURE:
                sprintf(message, "CHECK PRESSURE");
                break;
            case OK:
                sprintf(message, "OK");
                break;
        }            
        for (int i = 0; i < strlen(message); i++)
            lcd_data(message[i]);
        
        _delay_ms(3000);
        
        lcd_clear_display();
                    
    }
}

int16_t measure_temp()
{
    int16_t temperature = 0;
    if(one_wire_reset()) {
        one_wire_transmit_byte(0xCC);
        one_wire_transmit_byte(0x44);
        while(!one_wire_receive_bit()) {
            //busy waiting
        }
        if(one_wire_reset()) {
            one_wire_transmit_byte(0xCC);
            one_wire_transmit_byte(0xBE);
            temp_l = one_wire_receive_byte();
            temp_h = one_wire_receive_byte();
        }
        else {
            temp_l = 0x00;
            temp_h = 0x80;
        }
    }
    else {
        temp_l = 0x00;
        temp_h = 0x80;
    }   
       
    temperature = (temp_h & 0b00000111) << 8;
    temperature |= temp_l;
    double result = ((double)temperature * 0.0625) + 12.0;
        
    sprintf(output, "%.1f", result);
    
    return result;
}

uint16_t read_adc() 
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
}