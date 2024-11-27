#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>


#include "setup_LCD_PEX.h"
#include "setup_TWI.h"

void setup() {  
    twi_init();
    _delay_ms(100);
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
    lcd_init();
    _delay_ms(100);       // Delay 100 mS
    lcd_clear_display();
}

void lcd_set_cursor(int row, int col) {
    unsigned char pos = (row == 0) ? col : (col + 0x40);
    lcd_command(0x80 | pos);
}

int main(void) {
	setup();
	
    while(1) {
        //lcd_set_cursor(0, 0);
        
        //char msg[] ="FILOS";
        //for(int i = 0; i < 12; i++) {
        //    lcd_data(msg[i]);
        //}
        //lcd_set_cursor(1, 0);        
        char name[] = "GIORGOS STRIFTIS";
        for(int i = 0; i < 16; i++) {
            lcd_data(name[i]);
        }
        _delay_ms(1000);
        lcd_clear_display();
        
    }
}