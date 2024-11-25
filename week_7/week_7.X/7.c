#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>
#include <xc.h>

#include "setup_LCD_PEX.h"
#include "setup_OWI.h"
#include "setup_TWI.h"

static uint8_t temp_l, temp_h;

void setup() {  
    twi_init();
    _delay_ms(500);  // Delay 500 ms for better stability
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // Set as output
    lcd_init();
    _delay_ms(50);
    lcd_clear_display();
}

int main()
{
    setup();
    int sign = 0;
    int16_t temperature = 0;
    while(1){
        lcd_clear_display();
        if(one_wire_reset()) {
            one_wire_transmit_byte(0xCC);
            one_wire_transmit_byte(0x44);
            while(!one_wire_receive_bit()) {
                //busy waiting
            }
            if(one_wire_reset()) {
                one_wire_transmit_byte(0xCC);
                one_wire_transmit_byte(0xBE);
                temp_l = one_wire_receive_byte();
                temp_h = one_wire_receive_byte();
            }
            else {
                temp_l = 0x00;
                temp_h = 0x80;
            }
        }
        else {
            temp_l = 0x00;
            temp_h = 0x80;
        }   
        
        if(temp_l == 0x00 && temp_h == 0x80) {
            char msg[] = "NO Device";
            for(int i = 0; i < 9; i++){
                lcd_data(msg[i]);
            }
        }
        else{
            if(temp_h & 0b11111000) sign = 1;
            else sign = 0;
        
            temperature = (temp_h & 0b00000111) << 8;
            temperature |= temp_l;
            double result = (double)temperature * 0.0625;
        
            if(sign) lcd_data('-');
            else lcd_data('+');
        
            char output[6];
            sprintf(output, "%.4f", result);
            for(int i = 0; i < 6; i++) {
                lcd_data(output[i]);
            }
       
        }
        _delay_ms(1000);
    }
}
