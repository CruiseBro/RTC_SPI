/*
 * rtc_spi.c
 *
 *  Created on: 05.11.2017
 *      Author: nicoo
 */
#define sam3s4
#include "board.h"
#include "rtc_spi.h"

//PIN used for UEXT-Interface configured as output for Chip Enable of the RTC
#define PIN_SPI_NPCS1_PA31  {PIO_PA31A_NPCS1, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

//declare Pins
static const Pin pSpiPins[] = {PINS_SPI, PIN_SPI_NPCS0_PA11};
static const Pin pCE = PIN_SPI_NPCS1_PA31;



rtc_init(){
	//PINs konfigurieren
	PIO_Configure(pSpiPins, PIO_LISTSIZE(pSpiPins));
	PIO_Configure(&pCE, 1);

	//Configure SPI using Master Mode
	SPI_Configure(SPI, ID_SPI, SPI_MR_MSTR|SPI_MR_MODFDIS|SPI_PCS(1));
	SPI_ConfigureNPCS( SPI, 1, SPI_DLYBCT( 100000, BOARD_MCK ) | SPI_DLYBS(100000, BOARD_MCK) | SPI_SCBR( 100000, BOARD_MCK) );
	SPI_Enable( SPI ) ;
}

//return formatted time as string
void rtc_read_time(char* buffer)
{
	PIO_Set(&pCE);

	SPI_Write(SPI, 1, 0b10010010);
	SPI_Read(SPI);

	SPI_Write(SPI, 1, 0b00000000);
	buffer[0] = SPI_Read(SPI)>>1; //seconds

	SPI_Write(SPI, 1, 0b00000000);
	buffer[1] = SPI_Read(SPI)>>1;//minutes

	SPI_Write(SPI, 1, 0b00000000);
	buffer[2] = SPI_Read(SPI)>>1;//hours

	PIO_Clear(&pCE);
}

void rtc_write_time(char hours_bcd, char minutes_bcd, char seconds_bcd){
	PIO_Set(&pCE);

	SPI_Write(SPI, 1, 0b00010010);
	SPI_Read(SPI);

	SPI_Write(SPI, 1, seconds_bcd);
	SPI_Read(SPI)>>1; //seconds

	SPI_Write(SPI, 1, minutes_bcd);
	SPI_Read(SPI)>>1;//minutes

	SPI_Write(SPI, 1, hours_bcd);
	SPI_Read(SPI)>>1;//hours

	PIO_Clear(&pCE);
}
//return formatted date as string
void rtc_read_date(char* buffer){
	PIO_Set(&pCE);

	SPI_Write(SPI, 1, 0b10010101);
	SPI_Read(SPI); //ignore this

	SPI_Write(SPI, 1, 0b00000000);
	buffer[2] = SPI_Read(SPI)>>1; //Days

	SPI_Write(SPI, 1, 0b00000000);
	SPI_Read(SPI); //ignore Weekdays this

	SPI_Write(SPI, 1, 0b00000000);
	buffer[1] = SPI_Read(SPI)>>1; //Months

	SPI_Write(SPI, 1, 0b00000000);
	buffer[0] = SPI_Read(SPI)>>1; //Year

	PIO_Clear(&pCE);

}
void rtc_write_date(char years_bcd, char months_bcd, char days_bcd){
	PIO_Set(&pCE);

	SPI_Write(SPI, 1, 0b00010101);
	SPI_Read(SPI);

	SPI_Write(SPI, 1, days_bcd);//days
	SPI_Read(SPI);

	SPI_Write(SPI, 1, 0b00000000);//weekdays ignored
	SPI_Read(SPI);

	SPI_Write(SPI, 1, months_bcd);//months
	SPI_Read(SPI);

	SPI_Write(SPI, 1, years_bcd);//years
	SPI_Read(SPI);

	PIO_Clear(&pCE);
}
