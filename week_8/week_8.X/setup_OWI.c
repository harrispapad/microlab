#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>

uint8_t one_wire_reset()
{
    DDRD |= (1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(480);
    
    DDRD &= ~(1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(100);
    
    uint8_t input = PIND;

    _delay_us(380);
    
    if(input & 0b00010000) return 0;
    else return 1;
}

uint8_t one_wire_receive_bit()
{
    uint8_t result;
    DDRD |= (1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(2);
    
    DDRD &= ~(1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(10);
    
    uint8_t input = PIND;
    if(input & 0b00010000) result = 1;
    else result = 0;
    _delay_us(49);
    
    return result;
}

void one_wire_transmit_bit(uint8_t output)
{
    DDRD |= (1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(2);

    if(output) PORTD |= (1 << PD4);
    else    PORTD &= ~(1 << PD4);
    _delay_us(58);
    
    DDRD &= ~(1 << PD4);
    PORTD &= ~(1 << PD4);
    _delay_us(1);
}

uint8_t one_wire_receive_byte()
{
    uint8_t input;
    uint8_t temp = 0;
    for(int i = 0; i < 8; i++) {
        input = one_wire_receive_bit();
        temp >>= 1;
        if(input == 0) temp |= input;
        else temp |= 0x80;
    }
    return temp;
}

void one_wire_transmit_byte(uint8_t output)
{
    uint8_t temp = output;
    for(int i = 0; i < 8; i++) {
        output = 0;
        if(temp & 1) output = 1;
        one_wire_transmit_bit(output);
        temp >>= 1;
    }
}