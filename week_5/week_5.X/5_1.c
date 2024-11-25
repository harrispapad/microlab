#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <xc.h>

#include "setup_TWI.h"

void setup()
{
    DDRB = 0b11110000;                   //PORTB input
    PORTB = 0b00001111;               // enable pull-up resistors
    DDRD = 0xFF;
    PORTD = 0xFF;
}

int main(void) {
    setup();
    uint8_t input;
    uint8_t A, B, C, D, F0, F1;
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    while(1)
    {
        input = ~(PINB);
        input &= 0b00001111;
        A = (input & 0b00000001);
        B = (input & 0b00000010) >> 1;
        C = (input & 0b00000100) >> 2;
        D = (input & 0b00001000) >> 3;
        F0 = (~((~A&B&C) | ((~B)&D))) & 1;
        F1 = ((A|B|C) & (B&(~D))) << 1;
        _delay_ms(1);
        F0 |= F1;
        
        PCA9555_0_write(REG_OUTPUT_0, F0);
    }

}