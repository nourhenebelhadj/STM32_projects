#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include"data_rtc_processing.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_rtc.h"

void Parse_And_Set_RTC(UART_HandleTypeDef *handler_uart, RTC_HandleTypeDef *rtc,
		uint8_t *rxData, RTC_TimeTypeDef *sTime_out, RTC_DateTypeDef *sDate_out) {

	uint16_t year, month, date, hour, minute, second;
	RTC_TimeTypeDef sTimeRead = { 0 };
	RTC_DateTypeDef sDateRead = { 0 };

	if (rxData[RX_BUFFER_SIZE - 1] == '\n') {

		rxData[RX_BUFFER_SIZE - 1] = '\0';
	}

	printf("Received UART data '%s'\n", (char*) rxData);
	int count = sscanf((char*) rxData, "%hd-%hd-%hd %hd:%hd:%hd", &year, &month,
			&date, &hour, &minute, &second);

	//printf("Parsing result count: %d\n", count);
	printf("Parsed detailed date successfully: %hu-%hd-%hd %hd:%hd:%hd\n", year,
			month, date, hour, minute, second);

	if (count != 6) {

		printf("Data not parsed correctly");
	}

	// 1. Setup Time Structure
	sTime_out->Hours = (uint8_t) hour;
	sTime_out->Minutes = (uint8_t) minute;
	sTime_out->Seconds = (uint8_t) second;
	sTime_out->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime_out->StoreOperation = RTC_STOREOPERATION_RESET;

	sDate_out->Year = (uint8_t) (year - 2000);
	sDate_out->Month = (uint8_t) month;
	sDate_out->Date = (uint8_t) date;

	sDate_out->WeekDay = RTC_WEEKDAY_MONDAY;

	if (HAL_RTC_SetDate(rtc, sDate_out, RTC_FORMAT_BIN) != HAL_OK) {
		printf("issue in RTC_SetDate");

	}

	if (HAL_RTC_SetTime(rtc, sTime_out, RTC_FORMAT_BIN) != HAL_OK) {
		printf("issue in RTC_SetTime");

	}

	printf("**** Get RTC set values **** \n");
	HAL_RTC_GetTime(rtc, &sTimeRead, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(rtc, &sDateRead, RTC_FORMAT_BIN);

	printf("RTC Current Time: 20%02u-%02u-%02u %02u:%02u:%02u\n",
			(unsigned int) sDateRead.Year, (unsigned int) sDateRead.Month,
			(unsigned int) sDateRead.Date, (unsigned int) sTimeRead.Hours,
			(unsigned int) sTimeRead.Minutes, (unsigned int) sTimeRead.Seconds);

	memset(rxData, 0, RX_BUFFER_SIZE);
}

void Received_Ack_Uart_Message(UART_HandleTypeDef *handler_uart,
		uint8_t *rxData) {

	if (rxData[RX_BUFFER_SIZE_CONNECT - 1] == '\n') {

		rxData[RX_BUFFER_SIZE_CONNECT - 1] = '\0';

		if (strcmp((char*) rxData, COMMAND_STRING) == 0) {
			printf("well_received");

			HAL_UART_Transmit(handler_uart, (uint8_t*) ACK_STRING,
					strlen(ACK_STRING), 100);
		}

		memset(rxData, 0, RX_BUFFER_SIZE_CONNECT);
	}

}

