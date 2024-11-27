#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#include "setup_TWI.h"
#include "setup_KEYBOARD.h"

uint16_t pressed_keys;

void setup_keyboard() 
{
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);
	PCA9555_0_write(REG_OUTPUT_1, 0x0F);              // check last line
}

uint16_t scan_row(uint8_t i, uint16_t input)
{
    uint16_t temp = 0;
    switch(i){
        case 1: 
            PCA9555_0_write(REG_OUTPUT_1, 0b11111110);
            temp = ~(PCA9555_0_read(REG_INPUT_1));
            temp &= 0xF0;
            input = temp >> 4;
            break;
        case 2:
            PCA9555_0_write(REG_OUTPUT_1, 0b11111101);
            temp = ~(PCA9555_0_read(REG_INPUT_1));
            temp &= 0xF0;
            input |= temp;
            break;
        case 3:
            PCA9555_0_write(REG_OUTPUT_1, 0b11111011);
            temp = ~(PCA9555_0_read(REG_INPUT_1));
            temp &= 0xF0;
            temp = temp << 4;
            input |= temp;
            break;
        case 4:
            PCA9555_0_write(REG_OUTPUT_1, 0b11110111);
            temp = ~(PCA9555_0_read(REG_INPUT_1));
            temp &= 0xF0;
            temp = temp << 8;
            input |= temp;
            break;
    }   
  
    return input;
}

uint16_t scan_keypad()
{
    uint16_t input = 0;
    for(uint16_t i = 1; i < 5; i++){
        input = scan_row(i, input);
    }
    
    return input;
}

void scan_keypad_rising_edge()
{
    uint16_t pressed_keys_tempo;
    pressed_keys_tempo = scan_keypad();
    _delay_us(200);
    pressed_keys_tempo &= scan_keypad();
    if(pressed_keys_tempo != pressed_keys){
        pressed_keys ^= pressed_keys_tempo;
        pressed_keys &= pressed_keys_tempo;
    }
}

static int ascii[] = {42, 48, 35, 68, 55, 56, 57, 67, 52, 53, 54, 66, 49, 50, 51, 65};

int keypad_to_ascii() 
{
    uint16_t temp = pressed_keys;
    for(int i = 0; i < 16; i++) {
        if(temp == 1) return ascii[i];
        temp = temp >> 1;
    }
    return 0;
}
