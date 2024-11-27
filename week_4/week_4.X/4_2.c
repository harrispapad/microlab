#define F_CPU 16000000UL // Define CPU frequency for delay
#define VOLTAGE 5        // Define the reference voltage
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <xc.h>
#include "setup.h"

void setup() {           
    DDRD = 0xFF;          // Set PORTD as output
    lcd_init();
    _delay_ms(100);       // Delay 100 mS
    lcd_clear_display();

    ADMUX = 0b01000001;   // ADC right-adjusted, select ADC1
    ADCSRA = 0b10000111;  // Enable ADC with a prescaler of 128
}


uint16_t read_adc() {
    ADMUX |= (1 << MUX0);  
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
}

int main() {
    setup();
    while(1) {
        lcd_clear_display();
        double adc = read_adc();
        double Vin = (adc * VOLTAGE) / 1024;
        char result[5];
        sprintf(result, "%.2f", Vin);
        for(int i = 0; i < 4; i++) {
            lcd_data(result[i]);
        }
        _delay_ms(100);
    }
}