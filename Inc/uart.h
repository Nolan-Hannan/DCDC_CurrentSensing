#ifndef UART_H
#define UART_H

#include "stm32f4xx_hal.h"
#include <stdint.h>


void UART_Init(UART_HandleTypeDef *huart);
void UART_StartReceiveIT(void);
void UART_SendString(const char *str);
void UART_SendData(uint8_t *data, uint16_t len);
void UART_SendLine(const char *str);

uint8_t UART_LineReady(void);
void UART_GetLine(char *dest, uint16_t max_len);
void UART_Process(void);
void UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif
