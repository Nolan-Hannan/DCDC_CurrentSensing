#include "uart.h"
#include "main.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>

#define UART1_RX_LINE_SIZE 128

static UART_HandleTypeDef *uart1_handle = NULL;

static uint8_t rx1_byte; // one-byte interrupt reception
static volatile char rx1_line[UART1_RX_LINE_SIZE];     // current command line
static volatile uint16_t rx1_index = 0;
static volatile uint8_t line1_ready = 0;

HAL_StatusTypeDef UART_Init(UART_HandleTypeDef *huart) {
	if(huart == NULL) return HAL_ERROR;
	uart1_handle = huart;
	rx1_index = 0;
	line1_ready = 0;
	for(int i = 0; i < UART1_RX_LINE_SIZE; i++) rx1_line[i] = 0;

	return HAL_OK;
}

HAL_StatusTypeDef UART_StartReceiveIT(void) {
	HAL_StatusTypeDef status;
    if (uart1_handle == NULL)
        return HAL_ERROR;

    status = HAL_UART_Receive_IT(uart1_handle, &rx1_byte, 1);
    return status;
}

HAL_StatusTypeDef UART_SendString(const char *str) {
    return HAL_UART_Transmit(uart1_handle, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}


HAL_StatusTypeDef UART_SendLine(const char *str) {
	HAL_StatusTypeDef status;
	status = UART_SendString(str);
	if(status != HAL_OK) return status;
	status = UART_SendString("\r\n");
	return status;
}

uint8_t UART_LineReady(void) {
    return line1_ready;
}

void UART_GetLine(char *dest, uint16_t max_len) {
	if (dest == NULL || max_len == 0) return;

	__disable_irq();
	for(uint16_t i = 0; i < max_len - 1; i++) {
	    dest[i] = rx1_line[i];
	    if(rx1_line[i] == '\0') break;
	}
	dest[max_len - 1] = '\0';

	for(int i = 0; i < UART1_RX_LINE_SIZE; i++) rx1_line[i] = 0;
	line1_ready = 0;
	__enable_irq();
}

void UART_Process(void) {

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != uart1_handle) return;

    char c = (char)rx1_byte;

    // Choose to echo by default
//    HAL_UART_Transmit(uart1_handle, &rx1_byte, 1, 10);

    if (!line1_ready)
    {
        if (c == '\r' || c == '\n')
        {
            if (rx1_index > 0)
            {
                rx1_line[rx1_index] = '\0';
                line1_ready = 1;
                rx1_index = 0;
            }
        }
        else if (rx1_index < UART1_RX_LINE_SIZE - 1)
        {
            rx1_line[rx1_index++] = c;
        }
        else
        {
            // overflow behavior
            rx1_index = 0;
            for(int i = 0; i < UART1_RX_LINE_SIZE; i++) rx1_line[i] = 0;
        }
    }

    if(HAL_UART_Receive_IT(uart1_handle, &rx1_byte, 1) != HAL_OK) Error_Handler();
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart != uart1_handle)
        return;


    /* Clear hardware error flags */
	__HAL_UART_CLEAR_OREFLAG(huart);
	__HAL_UART_CLEAR_FEFLAG(huart);
	__HAL_UART_CLEAR_NEFLAG(huart);
	__HAL_UART_CLEAR_PEFLAG(huart);

	/* Reset HAL RX state */
	HAL_UART_AbortReceive(huart);

	/* Restart RX */
	for(int i = 0; i < 3; i++) {
		if(HAL_UART_Receive_IT(uart1_handle, &rx1_byte, 1) == HAL_OK) return;
	}

	Error_Handler();
}



