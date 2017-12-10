/*
 * rtc_spi.h
 *
 *  Created on: 05.11.2017
 *      Author: nicoo
 */

#ifndef SOURCE_RTC_SPI_H_
#define SOURCE_RTC_SPI_H_

void rtc_init();
void rtc_read_time(char* buffer);
void rtc_write_time(char hours_bcd, char minutes_bcd, char seconds_bcd);
void rtc_read_date(char* buffer);
void rtc_write_date(char years_bcd, char months_bcd, char days_bcd);

#endif /* SOURCE_RTC_SPI_H_ */
