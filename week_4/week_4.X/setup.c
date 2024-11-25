#define F_CPU 16000000UL // Define CPU frequency for delay
#define VOLTAGE 5        // Define the reference voltage
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <xc.h>
#include "setup.h"

void lcd_init()
{
    _delay_ms(200);
    
    PORTD = (0x30);
    PORTD |= 1 << PD3;
    _delay_us(10);
    PORTD &= ~(1 << PD3);
    _delay_us(250);
    
    PORTD = (0x30);
    PORTD |= 1 << PD3;
    _delay_us(10);
    PORTD &= ~(1 << PD3);
    _delay_us(250);
    
    PORTD = (0x30);
    PORTD |= 1 << PD3;
    _delay_us(10);
    PORTD &= ~(1 << PD3);
    _delay_us(250);

    PORTD = 0x20;
    PORTD |= (1 << PD3);  
    PORTD &= ~(1 << PD3); 
    _delay_us(250);
    
    lcd_command(0x28);
    
    lcd_command(0x0c);
    
    lcd_clear_display();
    
    lcd_command(0x06);
}

void lcd_clear_display() {
    lcd_command(0x01);      // Clear display command
    _delay_ms(2);           // Wait for LCD to process
}

void lcd_command(uint8_t cmd) {
    PORTD &= ~(1 << PD2);   // RS = 0 for command mode
    write_2_nibbles(cmd);
    _delay_us(50);          // Short delay for LCD
}

void lcd_data(char data) {
    PORTD |= (1 << PD2);    // RS = 1 for data mode
    write_2_nibbles(data);
    _delay_us(50);
}

void write_2_nibbles(uint8_t data) {
    uint8_t low_bits = PORTD & 0x0F;    // Mask PORTD lower bits

    // Send high nibble
    PORTD = (data & 0xF0) | low_bits;
    PORTD |= (1 << PD3);                // Enable pulse
    _delay_us(1);
    PORTD &= ~(1 << PD3);

    _delay_us(50);                      // Delay between nibbles

    // Send low nibble
    PORTD = ((data << 4) & 0xF0) | low_bits;
    PORTD |= (1 << PD3);                // Enable pulse
    _delay_us(1);
    PORTD &= ~(1 << PD3);
}
