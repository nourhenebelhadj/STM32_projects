#ifndef __RTC_MANAGER_H
#define __RTC_MANAGER_H

#include "stm32h7xx_hal.h" // Adjust to your specific MCU HAL header (e.g., stm32l4xx_hal.h)
#include <stdint.h>        // For uint8_t, uint16_t types

#define RX_BUFFER_SIZE 20
#define RX_BUFFER_SIZE_CONNECT 17
#define COMMAND_STRING "Connect to STM32"
#define ACK_STRING "ACK: Connection Established\r\n"

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart3;
extern uint8_t rxData[RX_BUFFER_SIZE];


void Parse_And_Set_RTC(UART_HandleTypeDef *handler_uart, RTC_HandleTypeDef *rtc,
		uint8_t *rxData, RTC_TimeTypeDef *sTime_out,
		RTC_DateTypeDef *sDate_out);


void Received_Ack_Uart_Message(UART_HandleTypeDef *handler_uart,
		uint8_t *rxData);
#endif
