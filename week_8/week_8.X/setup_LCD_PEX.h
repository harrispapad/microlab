void lcd_init(void);
void lcd_clear_display(void);
void lcd_command(uint8_t cmd);
void lcd_data(char data);
void write_2_nibbles(uint8_t data);
void lcd_set_cursor(int row, int col);
