#define sam3s4
#include "board.h"
#include "lcd_dogm.h"

void wait_ms(int time_ms);
void wait_us(int time_us);

void lcd_print_nibble(char nibble)
{
	PIOA->PIO_CODR = (1 << LCD_DATA_7_PIN);
	PIOA->PIO_CODR = (1 << LCD_DATA_6_PIN);
	PIOA->PIO_CODR = (1 << LCD_DATA_5_PIN);
	PIOA->PIO_CODR = (1 << LCD_DATA_4_PIN);
	PIOA->PIO_SODR = (1 << LCD_E_PIN);
	PIOA->PIO_SODR = (((nibble >> 3) & (0x01)) << LCD_DATA_7_PIN);
	PIOA->PIO_SODR = (((nibble >> 2) & (0x01)) << LCD_DATA_6_PIN);
	PIOA->PIO_SODR = (((nibble >> 1) & (0x01)) << LCD_DATA_5_PIN);
	PIOA->PIO_SODR = (((nibble) & (0x01)) << LCD_DATA_4_PIN);
	wait_us(100);
	PIOA->PIO_CODR = (1 << LCD_E_PIN);
	wait_us(1);
	PIOA->PIO_SODR = (1 << LCD_E_PIN);
	wait_us(100);
	return;
}

void lcd_print_char(char character)
{
	switch (character) {
	case 'ä':
		character = 0x84;
		break;
	case 'Ä':
		character = 0x8E;
		break;
	case 'ö':
		character = 0x94;
		break;
	case 'Ö':
		character = 0x99;
		break;
	case 'ü':
		character = 0x81;
		break;
	case 'Ü':
		character = 0x9A;
		break;
	default:
		break;
	}
	lcd_print_nibble(character >> 4);
	wait_us(10);
	lcd_print_nibble(character & 0x0F);
	return;
}

void lcd_clear(){
	lcd_set_cursor(1,1);
	lcd_print_string("                ");
	lcd_set_cursor(2,1);
	lcd_print_string("                ");
	lcd_set_cursor(3,1);
	lcd_print_string("                ");
}

void lcd_print_bcd(char character)
{

	lcd_print_char((character >> 4)+48);
	lcd_print_char((character & 0x0F)+48);

	return;
}

void lcd_print_string(char *zeichenkette)
{
	__disable_irq();

	PIOA->PIO_SODR = (1 << LCD_RS_PIN);
	PIOA->PIO_CODR = (1 << LCD_RW_PIN);

	while (*zeichenkette != '\0') {
		lcd_print_char(*zeichenkette);
		zeichenkette++;
	}

	__enable_irq();
	return;
}

void lcd_set_cursor(int zeile, int spalte)
{
	PIOA->PIO_CODR = (1 << LCD_RS_PIN);
	PIOA->PIO_CODR = (1 << LCD_RW_PIN);
	lcd_print_char((((zeile - 1) << 4) | ((spalte - 1) & 0xF)) | 0x80);
	PIOA->PIO_SODR = (1 << LCD_RS_PIN);
	return;
}

void lcd_init()
{
	PIOA->PIO_OER = (1 << LCD_LIGHT_PIN);
	PIOA->PIO_OER = (1 << LCD_DATA_7_PIN);
	PIOA->PIO_OER = (1 << LCD_DATA_6_PIN);
	PIOA->PIO_OER = (1 << LCD_DATA_5_PIN);
	PIOA->PIO_OER = (1 << LCD_DATA_4_PIN);
	PIOA->PIO_OER = (1 << LCD_E_PIN);
	PIOA->PIO_OER = (1 << LCD_RS_PIN);
	PIOA->PIO_OER = (1 << LCD_RW_PIN);

	PIOA->PIO_PER = (1 << LCD_LIGHT_PIN);
	PIOA->PIO_PER = (1 << LCD_DATA_7_PIN);
	PIOA->PIO_PER = (1 << LCD_DATA_6_PIN);
	PIOA->PIO_PER = (1 << LCD_DATA_5_PIN);
	PIOA->PIO_PER = (1 << LCD_DATA_4_PIN);
	PIOA->PIO_PER = (1 << LCD_E_PIN);
	PIOA->PIO_PER = (1 << LCD_RS_PIN);
	PIOA->PIO_PER = (1 << LCD_RW_PIN);

	PIOA->PIO_PUDR = (1 << LCD_LIGHT_PIN);
	PIOA->PIO_PUDR = (1 << LCD_DATA_7_PIN);
	PIOA->PIO_PUDR = (1 << LCD_DATA_6_PIN);
	PIOA->PIO_PUDR = (1 << LCD_DATA_5_PIN);
	PIOA->PIO_PUDR = (1 << LCD_DATA_4_PIN);
	PIOA->PIO_PUDR = (1 << LCD_E_PIN);
	PIOA->PIO_PUDR = (1 << LCD_RS_PIN);
	PIOA->PIO_PUDR = (1 << LCD_RW_PIN);

	PIOA->PIO_SODR = (1 << LCD_LIGHT_PIN);
	PIOA->PIO_SODR = (1 << LCD_RS_PIN);
	PIOA->PIO_SODR = (1 << LCD_RW_PIN);
	PIOA->PIO_SODR = (1 << LCD_E_PIN);
	wait_ms(50);
	PIOA->PIO_CODR = (1 << LCD_RS_PIN);
	PIOA->PIO_CODR = (1 << LCD_RW_PIN);
	wait_ms(50);
	lcd_print_nibble(0x03);	// Init Function Set
	wait_ms(10);
	lcd_print_nibble(0x03);	// Init Function Set
	wait_us(150);
	lcd_print_nibble(0x03);	// Init Function Set
	wait_us(50);
	lcd_print_nibble(0x02);	// Init Function Set
	wait_us(50);
	lcd_print_char(0x2D);	// Function Set: 4 Bit, 3 Lines, Double Height, Instruction Table 1
	wait_us(50);
	lcd_print_char(0x17);	// Bias Set: 1/5 Bias, Internal Oscillator Frequency set
	wait_us(50);
	lcd_print_char(0x70);	// Contrast set, C3 - C0 = 0
	wait_us(50);
	lcd_print_char(0x5D);	// Icon display on, booster on, contrast C5, C4 = 01
	wait_us(50);
	lcd_print_char(0x6E);	// Follower Control on
	wait_us(50);
	lcd_print_char(0x0C);	// Display on, Cursor off
	wait_ms(50);
	lcd_print_char(0x01);	// Display Clear
	wait_ms(5);
	lcd_print_char(0x06);	// Entry Mode set: increment by one, no shift
	wait_ms(50);
	lcd_print_string("Development ToolEmbedded Systems");
	PIOA->PIO_CODR = (1 << LCD_RS_PIN);
	PIOA->PIO_CODR = (1 << LCD_RW_PIN);
	wait_ms(1500);
	lcd_print_char(0x29);	// Function Set: 4bit, 1 Zeile, single height, instruction table 1
	wait_us(50);
	lcd_print_char(0x01);	// Clear Display
	wait_ms(2);
	return;
}

/* Sehr ungenaue Schätzwerte! Nicht nachmachen!
 * Präzise Wartezeiten/Zeitmessungen
 * sind z.B. mit dem SysTick möglich.
 */

void wait_ms(int time_ms)
{
	for (int i = 0; i < time_ms * 2500; i++)
		asm("nop");
	return;
}

void wait_us(int time_us)
{
	for (int i = 0; i < time_us * 3; i++)
		asm("nop");
	return;
}
