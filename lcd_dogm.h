#ifndef LCD_DOGM_H_
#define LCD_DOGM_H_

#ifdef __cplusplus
extern "C" {
#endif

enum LCD_PINS
{
	LCD_LIGHT_PIN = 0,
	LCD_DATA_7_PIN = 5,
	LCD_DATA_6_PIN = 6,
	LCD_DATA_5_PIN = 7,
	LCD_DATA_4_PIN = 8,
#ifdef OLD_LCD_LAYOUT
	LCD_E_PIN = 13,
	LCD_RW_PIN = 14,
	LCD_RS_PIN = 23
#else
	LCD_E_PIN = 23,
	LCD_RW_PIN = 25,
	LCD_RS_PIN = 26
#endif
};

void lcd_init();
void lcd_set_cursor(int row, int col);
void lcd_print_char(char character);
void lcd_print_string(char *str);
void lcd_print_bcd(char character);
void lcd_clear();
#ifdef __cplusplus
}
#endif

#endif /* LCD_DOGM_H_ */
