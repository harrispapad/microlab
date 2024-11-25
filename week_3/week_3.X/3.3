#define F_CPU 16000000UL
#include "avr/io.h"
#include <util/delay.h>

int DC_VALUE[13] = {8, 28, 48, 68, 88, 108, 128, 148, 168, 188, 208, 228, 248};
int mode = 1;
int index;

void _setup_pwm(){    
    TCCR1A = (1<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM12) | (1<<CS10);
}

void _setup_adc(){
    ADMUX = 0b01000000;                                        // MUX = 0 -> POT1
    ADCSRA = 0b10000111;  
}

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));                                               //wait for the flag to clear
    return ADC;
}

int main() {
    _setup_pwm();
    _setup_adc();
    index = 6;  

    
    DDRB |= 0b11111111; 
    
    DDRD |= 0b00111001;

    OCR1A = DC_VALUE[index];  // Set initial duty cycle

    while(1) {
        if (!(PIND & (1 << PIND1))) {
            mode = 1;
            _delay_ms(100);
        }
        if (!(PIND & (1 << PIND2))) {   
            mode = 2;
            _delay_ms(100);
        }

        
        if (mode == 1) {
            if (!(PIND & (1 << PIND6))) { 
                if (index < 12) {
                    index++;
                    OCR1A = DC_VALUE[index];  
                }
                _delay_ms(100);  
            }
            if (!(PIND & (1 << PIND7))) {   
                if (index > 0) {
                    index--;
                    OCR1A = DC_VALUE[index]; 
                }
                _delay_ms(100); 
            }
        }
        
        if (mode == 2) {
            uint16_t adc_value = read_adc(); 
            index = (adc_value * 12) / 1023;            // since we have 10bits(1024) we need to scale for index
            OCR1A = DC_VALUE[index];  
        }

        _delay_ms(100); 
    }
}