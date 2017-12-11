#define sam3s4
#include "board.h"
#include "lcd_dogm.h"
#include "spi.h"
#include "rtc_spi.h"
#include "rs232.h"

#define BUFFER_SIZE		49
#define LCD_LENGTH		48
#define MENU_STRING		"Configure Mode  B1: Time        B2: Date        "
#define CONFIG_STRING	"                B1.:+1    B1_:-1B2.:->    B2_:<-"

//State 0 == show time mode
//State 1 == show date mode
//State 2 == menu mode
//State 3 == time conf mode
//State 4 == date conf mode
static int8_t state = 0;
static int8_t menuidx = 0;
static int8_t groupMode = 0;
static char write_buffer[3];

static char string_out[9];
char readBuffer[BUFFER_SIZE];
char display[LCD_LENGTH+1] =			"                                                ";

void UART0_IrqHandler(void)
{
	if(readBuffer[48] == '\3'){
		memcpy(display, readBuffer, LCD_LENGTH);
		memcpy(display, string_out, 8);
		UART_ReadBuffer(UART0, readBuffer, 1);
	}
	else if(readBuffer[0] == '\2'){
		UART_ReadBuffer(UART0, readBuffer, BUFFER_SIZE);
	}
	else {
		UART_ReadBuffer(UART0, readBuffer, 1);
	}

}

//Converts an two-digit BCD to ascii-string
void bcd_to_string(char bcd, char* inascii) {
	//char bcdString[2];
	inascii[0] = (bcd >> 4)+48;
	inascii[1] = (bcd & 0x0F)+48;
}

void print_date(int row, int col, char* values){
	string_out[2] = '.';
	string_out[5] = '.';
	bcd_to_string(values[0], &string_out[0]);
	bcd_to_string(values[1], &string_out[3]);
	bcd_to_string(values[2], &string_out[6]);
	lcd_set_cursor(row,col);
	lcd_print_string(string_out);
}

void print_time(int row, int col, char* values){
	string_out[2] = ':';
	string_out[5] = ':';
	bcd_to_string(values[2], &string_out[0]);
	bcd_to_string(values[1], &string_out[3]);
	bcd_to_string(values[0], &string_out[6]);
	lcd_set_cursor(row,col);
	lcd_print_string(string_out);
}



void adjust_time(char* value, int8_t type){
	unsigned char limits[] = {36, 90, 90};
	if(*value >= limits[type]) {
		*value = 0;
	}
	else if((*value & 0b1111) >= 10){
		*value += 6;
	}
}

void adjust_date(char* value, int8_t type){
	unsigned char limits[] = {50, 19, 153};
	if(*value >= limits[type]) {
		*value = 0;
	}
	else if((*value & 0b1111) >= 10){
		*value += 6;
	}
}

static void button_handler1(const Pin* pPin)
{
	uint32_t curtime = 0;
	lcd_set_cursor(1,1);
	while(!PIO_Get(pPin) && curtime < 800000)
	{
		curtime++;
	}

	if(curtime >= 800000){
		//Button 1 long press
		switch(state) {
		case 0:
		case 1:
			//read other groups
			if(!groupMode){
				groupMode = 1;
			}
			else {
				groupMode = 0;
				lcd_clear();
			}
			break;
		case 2:
			//show_time_conf
			state = 3;
			break;
		case 3:
			if(menuidx%2 == 0){
				write_buffer[2-menuidx/2] -= 16;
			}
			else{
				write_buffer[2-menuidx/2]--;
			}
			adjust_time(&write_buffer[2-menuidx/2], menuidx/2);
			print_time(1,1, &write_buffer[0]);

			break;
		case 4:
			if(menuidx%2 == 0){
				write_buffer[menuidx/2] -= 16;
			}
			else{
				write_buffer[menuidx/2]--;
			}
			adjust_date(&write_buffer[menuidx/2], menuidx/2);
			print_date(1,1, &write_buffer[0]);
			break;
		}
	}
	else{
		//Button 1 short press
		switch(state) {
		case 0:
			//advanced feature
			break;
		case 1:
			//advanced feature
			break;
		case 2:
			//show_time_conf
			state = 3;
			break;
		case 3:
			if(menuidx%2 == 0){
				write_buffer[2-menuidx/2] += 16;
			}
			else{
				write_buffer[2-menuidx/2]++;
			}
			adjust_time(&write_buffer[2-menuidx/2], menuidx/2);
			print_time(1,1, &write_buffer[0]);
			break;
		case 4:
			if(menuidx%2 == 0){
				write_buffer[menuidx/2] += 16;

			}
			else{
				write_buffer[menuidx/2]++;
			}
			adjust_date(&write_buffer[menuidx/2], menuidx/2);
			print_date(1,1, &write_buffer[0]);
			break;
		}
	}
}

static void button_handler2(const Pin* pPin)
{
	uint32_t curtime = 0;
	lcd_set_cursor(1,1);
	while(!PIO_Get(pPin) && curtime < 800000)
	{
		curtime++;
	}

	if(curtime >= 800000){
		//Button 2 long press
		switch(state) {
		case 0:
			state = 2;
			break;
		case 1:
			state = 2;
			break;
		case 2:
			//show date config
			state = 4;
			break;
		case 3:
			//confirm and idx++, if idx > 5 -> state = 0 and idx = 0
			menuidx--;
			if(menuidx < 0) {
				state = 0;
			}

			break;
		case 4:
			//confirm and idx++, if idx > 5 -> state = 0 and idx = 0
			menuidx--;
			if(menuidx < 0) {
				state = 0;
			}

			break;
		}
	}
	else{
		//Button 2 short press
		switch(state) {
		case 0:
			state = 1;
			break;
		case 1:
			state = 0;
			break;
		case 2:
			//show date config
			state = 4;
			break;
		case 3:
			//confirm and idx++, if idx > 5 -> state = 0 and idx = 0
			menuidx++;
			if(menuidx > 5) {
				state = 0;
				rtc_write_time(write_buffer[2], write_buffer[1], write_buffer[0]);
			}
			adjust_time(&write_buffer[menuidx/2], menuidx/2);
			print_time(1,1, &write_buffer[0]);
			break;
		case 4:
			//confirm and idx++, if idx > 5 -> state = 0 and idx = 0
			menuidx++;
			if(menuidx > 5) {
				state = 0;
				rtc_write_date(write_buffer[0], write_buffer[1], write_buffer[2]);
			}
			adjust_date(&write_buffer[menuidx/2], menuidx/2);
			print_date(1,1, &write_buffer[0]);
			break;
		}
	}
}

void showConfig(){
	write_buffer[0] = 0;
	write_buffer[1] = 0;
	write_buffer[2] = 0;
	menuidx = 0;
	lcd_set_cursor(1,1);
	lcd_print_string(CONFIG_STRING);
}

void ITM_SendString(const char *s) {
	while(*s!='\0') {
		ITM_SendChar(*s++);
	}
}

int main(void)
{
	char time[3];
	char date[3];
	unsigned char buffer[2];

	uint16_t blinkCounter = 0;
	float blinkProportion = 0.80; //in Percent

	WDT_Disable(WDT); // Watchdog Timer

	lcd_init();

	/* Button Interrupt */
	const Pin PB1 = PIN_PUSHBUTTON_1;
	const Pin PB2 = PIN_PUSHBUTTON_2;
	PIO_InitializeInterrupts(0);

	PIO_Configure(&PB1, 1);
	PIO_SetDebounceFilter(&PB1, 20);
	PIO_ConfigureIt(&PB1, button_handler1);
	PIO_EnableIt(&PB1);

	PIO_Configure(&PB2, 1);
	PIO_SetDebounceFilter(&PB2, 20);
	PIO_ConfigureIt(&PB2, button_handler2);
	PIO_EnableIt(&PB2);

	string_out[8] = '\0';
	display[48] = '\0';
	rtc_init();

	RS232_Configure(115200, BOARD_MCK);

	UART_ReadBuffer(UART0, readBuffer, 1);

	RS232_EnableIt();

	while(1){
		if(state == 0){
			rtc_read_time(time);
			if(groupMode) {
				lcd_set_cursor(1,1);
				lcd_print_string(display);
			}
			print_time(1,1, time);
		}
		else if(state == 1){
			rtc_read_date(date);
			if(groupMode) {
				lcd_set_cursor(1,1);
				lcd_print_string(display);
			}
			print_date(1,1, date);
		}
		else if(state == 2) {
			lcd_set_cursor(1,1);
			lcd_print_string(MENU_STRING);
			while(state==2);
			lcd_clear();
		}
		else if(state == 3) {
			showConfig();
			lcd_set_cursor(1,1);
			lcd_print_string("00:00:00");
			while(state == 3){}
			lcd_clear();
		}
		else if(state == 4) {
			showConfig();
			lcd_set_cursor(1,1);
			lcd_print_string("00.00.00");
			while(state == 4);
			lcd_clear();
		}

		memcpy(display, string_out, 8);
		RS232_send(display, LCD_LENGTH);
		wait_ms(100);
	}
}
