#define F_CPU 1600000UL
#include "avr/io.h"
#include <util/delay.h>

int main(int argc, char** argv) {
    unsigned char duty;
    
    TCCR1A = (1<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM12) | (1<<CS11);
    
    DDRB |= 0b00111111;
    
    while(1) {
        for(duty = 0; duty < 255; duty++) {
            OCR1AL = duty;
            _delay_ms(100);
        }
        for(duty = 255; duty > 1; duty--) {
            OCR1AL = duty;
            _delay_ms(100);
        }
    }
}

