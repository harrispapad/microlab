#define F_CPU 16000000UL // Define CPU frequency for delay
#define VOLTAGE 5        // Define the reference voltage
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h> 


void setup(void);
void lcd_init(void);
void lcd_clear_display(void);
void lcd_command(uint8_t cmd);
void lcd_data(char data);
void write_2_nibbles(uint8_t data);

uint16_t read_adc() {
    ADMUX |= (1 << MUX0);  
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
}




int main(){
    setup();
    while(1) {
        lcd_clear_display();
        ADCSRA |= (1<<ADSC);
        double adc = read_adc();
        double Vin = (adc*VOLTAGE)/1024;
        char result[4];
        sprintf(result, "%f", Vin);
        for(int i = 0; i < 4; i++) {
            lcd_data(result[i]);
        }
        _delay_ms(1000);
    }
    
}


void setup() {           // Enable global interrupts
    DDRD = 0xFF;          // Set PORTD as output
    lcd_init();
    _delay_ms(100);       // Delay 100 mS
    lcd_clear_display();

    ADMUX = 0b01000001;   // ADC right-adjusted, select ADC1
    ADCSRA = 0b10001111;  // Enable ADC and start with interrupt

   
}

void lcd_init() {
    _delay_ms(200);
    PORTD = 0x30;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
    _delay_us(250);
    PORTD = 0x30;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
    _delay_us(250);
    PORTD = 0x30;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
    _delay_us(250);
    PORTD = 0x20;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
    _delay_us(250);
    
    lcd_command(0x28);
    lcd_command(0x0c);
    lcd_clear_display();
    lcd_command(0x06);
    
}

void lcd_clear_display() {
    lcd_command(0x01);
    _delay_ms(5);
}

void lcd_command(uint8_t cmd) {
    PORTD |= (0 << PD2);
    write_2_nibbles(cmd);
    _delay_us(250);    
}

void lcd_data(char data) {
    PORTD |= (0 << PD2);
    write_2_nibbles(data);
    _delay_us(250);
}

void write_2_nibbles(uint8_t data) {
    uint8_t x = PIND;
    x |= 0x0f;
    uint8_t y = data;
    y |= 0xf0;
    y += x;
    PORTD = y;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
    y = (data << 4) | (data >> 4);
    y |= 0xf0;
    y += x;
    PORTD = y;
    PORTD |= (1 << PD3);
    PORTD |= (0 << PD3);
}

