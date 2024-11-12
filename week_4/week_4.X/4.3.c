#define F_CPU 16000000UL // Define CPU frequency for delay
#define VOLTAGE 5        // Define the reference voltage
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void setup(void);
void lcd_init(void);
void lcd_clear_display(void);
void lcd_command(uint8_t cmd);
void lcd_data(char data);
void write_2_nibbles(uint8_t data);

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));  
    return ADC; 
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

void setup() {      
    DDRB = 0xFF;
    DDRD = 0xFF;          // Set PORTD as output
    lcd_init();
    _delay_ms(100);       // Delay 100 mS
    lcd_clear_display();

    ADMUX = 0b01000010;   // ADC right-adjusted, select ADC1
    ADCSRA = 0b10000111;  // Enable ADC with a prescaler of 128
}

void lcd_init() {
    _delay_ms(200);       // Initial delay
    write_2_nibbles(0x30);  // Function set: 8-bit mode
    _delay_ms(5);         // Wait for LCD to process

    write_2_nibbles(0x30);  // Repeat function set
    _delay_us(200);

    write_2_nibbles(0x30);  // Repeat function set again
    _delay_us(200);

    write_2_nibbles(0x20);  // Switch to 4-bit mode
    _delay_us(200);

    lcd_command(0x28);      // Function set: 4-bit, 2 lines, 5x8 dots
    lcd_command(0x0C);      // Display on, cursor off
    lcd_clear_display();
    lcd_command(0x06);      // Entry mode set: Increment cursor
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