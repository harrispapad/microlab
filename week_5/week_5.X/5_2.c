#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#include "setup_TWI.h"

void setup()
{
	DDRD = 0xF0;
}

int main(void) {
	setup();
	twi_init();
	uint8_t input;
	PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
	PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);       // bit_0 input, rest output
	PCA9555_0_write(REG_OUTPUT_1, 0x0);              // check last line
	while(1)
	{
		input = PCA9555_0_read(REG_INPUT_1);
		input &= 0xF0;
		input = ~(input >> 4);
		PCA9555_0_write(REG_OUTPUT_0, input);
	}

}