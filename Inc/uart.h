#ifndef UART_H
#define UART_H

#include "stm32f4xx_hal.h"
#include <stdint.h>


HAL_StatusTypeDef UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef UART_StartReceiveIT(void);
HAL_StatusTypeDef UART_SendString(const char *str);
HAL_StatusTypeDef UART_SendLine(const char *str);

uint8_t UART_LineReady(void);
void UART_GetLine(char *dest, uint16_t max_len);
void UART_Process(void);
void UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif
