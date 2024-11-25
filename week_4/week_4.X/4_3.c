#define F_CPU 16000000UL // Define CPU frequency for delay
#define VOLTAGE 5        // Define the reference voltage
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "setup.h"

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
}


void setup() {      
    DDRB = 0xFF;
    DDRD = 0xFF;          // Set PORTD as output
    lcd_init();
    _delay_ms(100);       // Delay 100 mS
    lcd_clear_display();

    ADMUX = 0b01000010;   // ADC right-adjusted, select ADC1
    ADCSRA = 0b10000111;  // Enable ADC with a prescaler of 128
}


int main() {
    int danger = 0;
    setup();
    while(1) {
        lcd_clear_display();
        double adc = read_adc();
        double ppm = (adc * 500) / 1024;
        if(ppm >= 70) danger = 1;
        else danger = 0;
        if(danger == 0){
            PORTB = 1;
            char message[5] = "CLEAR";
            for (int i = 0; i < 5; i++)
                lcd_data(message[i]);
            _delay_ms(100);
        }
        else {
            PORTB = 0;
            char message[12] = "GAS DETECTED";
            for(int i = 0; i < 12; i++){
                lcd_data(message[i]);
            }
            _delay_ms(50);
            if(ppm <= 83) PORTB = 1;
            else if(ppm <= 166) PORTB = 3;
            else if(ppm <= 249) PORTB = 7;
            else if(ppm <= 332) PORTB = 15;
            else if(ppm <= 415) PORTB = 31;
            else PORTB = 63; 
            _delay_ms(50);
        }
    }
}
