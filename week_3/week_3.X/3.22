#define F_CPU 16000000UL
#include "avr/io.h"
#include <util/delay.h>

int DC_VALUE[13] = {8, 28, 48, 68, 88, 108, 128, 148, 168, 188, 208, 228, 248}; 
uint16_t SAMPLES[16] = {0};  

void _setup_pwm() {    
    TCCR1A = (1<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM12) | (1<<CS10);
}

void _setup_adc() {
    ADMUX |= (1 << REFS0);  
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  
}

uint16_t read_adc() {
    ADMUX |= (1 << MUX0);  
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
}

void update_leds(uint16_t adc_value) {
    PORTD = 0x00;  
    if (adc_value <= 200) {
        PORTD |= (1 << PD0); 
    } else if (adc_value <= 400) {
        PORTD |= (1 << PD1);  
    } else if (adc_value <= 600) {
        PORTD |= (1 << PD2);  
    } else if (adc_value <= 800) {
        PORTD |= (1 << PD3);  
    } else {
        PORTD |= (1 << PD4);  
    }
}

int main() { 
    _setup_pwm();
    _setup_adc();
    int index = 6;  
    uint16_t sum = 0;
    int current_sample = 0;
    
    DDRB |= 0b11111111; 
    DDRD |= 0b00111111;  

    OCR1A = DC_VALUE[index];  
            
    while (1) {
        if (!(PIND & (1 << PIND6))) {  
            if (index < 12) {
                index++;
                OCR1A = DC_VALUE[index];
                _delay_ms(100);
            }
        }
        if (!(PIND & (1 << PIND7))) {  
            if (index > 0) {
                index--;
                OCR1A = DC_VALUE[index]; 
                _delay_ms(10);
            }
        }
        
        uint16_t adc_value = read_adc();  
        
        sum -= SAMPLES[current_sample]; 
        SAMPLES[current_sample] = adc_value;  
        sum += adc_value;  
        
        current_sample++;
        if (current_sample == 16) {
            current_sample = 0;
        }
        uint16_t average_adc_value = sum >> 4;  

        update_leds(average_adc_value);

        _delay_ms(100);  
    }
}
