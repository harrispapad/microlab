#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>

#include <xc.h>
#include "setup_LCD_PEX.h"
#include "setup_TWI.h"

void lcd_init() {
    _delay_ms(200);       // Initial delay
    
    uint8_t cmd = 0x30;
    PCA9555_0_write(REG_OUTPUT_0, cmd);
    uint8_t portd = PCA9555_0_read(REG_OUTPUT_0);  // Function set: 8-bit mode
    portd |= (1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(10);
    portd = PCA9555_0_read(REG_OUTPUT_0);
    portd &= ~(1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(250);         // Wait for LCD to process
    
    PCA9555_0_write(REG_OUTPUT_0, cmd);
    portd = PCA9555_0_read(REG_OUTPUT_0);  // Function set: 8-bit mode
    portd |= (1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(10);
    portd = PCA9555_0_read(REG_OUTPUT_0);
    portd &= ~(1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(250);         // Wait for LCD to process

    PCA9555_0_write(REG_OUTPUT_0, cmd);
    portd = PCA9555_0_read(REG_OUTPUT_0);  // Function set: 8-bit mode
    portd |= (1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(10);
    portd = PCA9555_0_read(REG_OUTPUT_0);
    portd &= ~(1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(250);         // Wait for LCD to process
    
    cmd = 0x20;
    PCA9555_0_write(REG_OUTPUT_0, cmd);
    portd = PCA9555_0_read(REG_OUTPUT_0);  // Function set: 8-bit mode
    portd |= (1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(10);
    portd = PCA9555_0_read(REG_OUTPUT_0);
    portd &= ~(1 << PD3);
    PCA9555_0_write(REG_OUTPUT_0, portd);
    _delay_us(250);         // Wait for LCD to process

    lcd_command(0x28);      // Function set: 4-bit, 2 lines, 5x8 dots
    lcd_command(0x0C);      // Display on, cursor off
    lcd_clear_display();
    lcd_command(0x06);      // Entry mode set: Increment cursor
}

void lcd_clear_display() {
    lcd_command(0x01);      // Clear display command
    _delay_ms(5);           // Wait for LCD to process
}

void lcd_command(uint8_t cmd) {
    //PCA9555_0_write(REG_CONFIGURATION_0, 0xff);
    uint8_t portd = PCA9555_0_read(REG_OUTPUT_0);
    //PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //set as output
    portd &= 0b11111011;     //PORTD &= ~(1 << PD2)
     
    PCA9555_0_write(REG_OUTPUT_0, portd);   
    
    
    write_2_nibbles(cmd);
    _delay_us(250);          // Short delay for LCD
}

void lcd_data(char data) {
    //PORTD |= (1 << PD2)    // RS = 1 for data mode
    //PCA9555_0_write(REG_CONFIGURATION_0, 0xff);
    uint8_t portd = PCA9555_0_read(REG_OUTPUT_0);
    //PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
    portd |= 0b00000100;
    
    
    PCA9555_0_write(REG_OUTPUT_0, portd);   
    
    write_2_nibbles(data);
    _delay_us(250);
}

void write_2_nibbles(uint8_t data) {
    uint8_t portd = PCA9555_0_read(REG_OUTPUT_0); //read portd
    uint8_t low_bits = portd & 0x0F;    // Mask PORTD lower bits

    
    portd = (data & 0xF0) + low_bits;
    PCA9555_0_write(REG_OUTPUT_0, portd);  
    
    
    ///enable pulse
    portd = PCA9555_0_read(REG_OUTPUT_0); //read portd
    portd |= 0b00001000;                    // Enable pulse
    
    PCA9555_0_write(REG_OUTPUT_0, portd);  
    _delay_us(10);
    portd = PCA9555_0_read(REG_OUTPUT_0); //read portd
    portd &= 0b11110111;
    
    PCA9555_0_write(REG_OUTPUT_0, portd);
    
    _delay_us(50);                      // Delay between nibbles

    // Send low nibble

    portd = ((data << 4) & 0xF0) + low_bits;
    PCA9555_0_write(REG_OUTPUT_0, portd);
    
    
    portd = PCA9555_0_read(REG_OUTPUT_0); //read portd
    portd |= 0b00001000;                    // Enable pulse
    
    PCA9555_0_write(REG_OUTPUT_0, portd);  
    _delay_us(10);
    
    portd = PCA9555_0_read(REG_OUTPUT_0); //read portd
    portd &= 0b11110111;
    PCA9555_0_write(REG_OUTPUT_0, portd);
}