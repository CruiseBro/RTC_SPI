/*
 * rs232.c
 *
 *  Created on: 10.12.2017
 *      Author: nicoo
 */

#include "rs232.h"
#include "board.h"

#define PINS_UART0  { PIO_PA9A_URXD0|PIO_PA10A_UTXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

uint8_t UART_ReadBuffer(Uart *uart, void *buffer, uint32_t size) {
	if((uart->UART_RCR == 0) && (uart->UART_RNCR == 0)) {
		uart->UART_RPR = (uint32_t) buffer;
		uart->UART_RCR = size;
		uart->UART_PTCR = UART_PTCR_RXTEN;

		return 1;
	}
	else if (uart->UART_RNCR == 0) {
		uart->UART_RNPR = (uint32_t) buffer;
		uart->UART_RNCR = size;

		return 1;
	}
	else{
		return 0;
	}
}

void RS232_Configure(uint32_t baudrate, uint32_t masterClock) {
	//Configure Pins
	const Pin pPins[] = {PINS_UART0, PIN_USART1_RXD, PIN_USART1_TXD};

	PIO_Configure(pPins, PIO_LISTSIZE(pPins));

	//Configure UART0

	PMC->PMC_PCER0 = 1 << ID_UART0;

	UART0->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

	UART0->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;

	UART0->UART_BRGR = (masterClock / baudrate) / 16;

	NVIC_EnableIRQ( UART0_IRQn ) ;

	UART0->UART_CR = UART_CR_RXEN | UART_CR_TXEN;

	//Configure USART1

	uint32_t mode = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK
					| US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT
					| US_MR_CHMODE_NORMAL;

	PMC_EnablePeripheral( ID_USART1 ) ;

	//NVIC_EnableIRQ( USART1_IRQn ) ;

	USART_Configure(USART1, mode, baudrate, masterClock);

    USART_SetTransmitterEnabled( USART1, 1 ) ;
    USART_SetReceiverEnabled( USART1, 0 ) ;

}

void RS232_send(void* buffer, uint32_t size) {
	USART_PutChar(USART1, ' ');
	USART_PutChar(USART1, '\2');
	USART_WriteBuffer(USART1, buffer, size);
	USART_PutChar(USART1, '\3');
	USART_PutChar(USART1, ' ');
}

void RS232_EnableIt() {
	UART0->UART_IER = UART_IER_RXBUFF;
	//USART1->US_IER = US_IER_RXBUFF;
}
