/*
 * rs232.h
 *
 *  Created on: 10.12.2017
 *      Author: nicoo
 */

#ifndef SOURCE_RS232_H_
#define SOURCE_RS232_H_

uint8_t UART_ReadBuffer(Uart *uart, void *buffer, uint32_t size);
void RS232_Configure(uint32_t baudrate, uint32_t masterClock);
void RS232_EnableIt();

#endif /* SOURCE_RS232_H_ */
