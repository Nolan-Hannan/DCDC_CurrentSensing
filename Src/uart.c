#include "uart.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define UART1_RX_LINE_SIZE 128

static UART_HandleTypeDef *uart1_handle = NULL;

static uint8_t rx1_byte;                     // one-byte interrupt reception
static char rx1_line[UART1_RX_LINE_SIZE];     // current command line
static volatile uint16_t rx1_index = 0;
static volatile uint8_t line1_ready = 0;

void UART_Init(UART_HandleTypeDef *huart) {
	uart1_handle = huart;
	rx1_index = 0;
	line1_ready = 0;
	memset(rx1_line, 0, sizeof(rx1_line));

    UART_StartReceiveIT();
}

void UART_StartReceiveIT(void) {
    if (uart1_handle == NULL)
        return;

    HAL_UART_Receive_IT(uart1_handle, &rx1_byte, 1);
}

void UART_SendString(const char *str) {
    HAL_UART_Transmit(uart1_handle, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}


void UART_SendData(uint8_t *data, uint16_t len) {

}

void UART_SendLine(const char *str) {
	 UART_SendString(str);
	 UART_SendString("\r\n");
}

uint8_t UART_LineReady(void) {
    return line1_ready;
}

void UART_GetLine(char *dest, uint16_t max_len) {
	if (dest == NULL || max_len == 0) return;

	strncpy(dest, rx1_line, max_len - 1);
	dest[max_len - 1] = '\0';

	memset(rx1_line, 0, sizeof(rx1_line));
	line1_ready = 0;
}

void UART_Process(void) {

}

void UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != uart1_handle) return;

    char c = (char)rx1_byte;

    // Choose to echo by default
    HAL_UART_Transmit(uart1_handle, &rx1_byte, 1, 10);

    if (!line1_ready)
    {
        if (c == '\r' || c == '\n')
        {
            if (rx1_index > 0)
            {
                rx1_line[rx1_index] = '\0';
                line1_ready = 1;
                rx1_index = 0;
                UART_SendString("\r\n");
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
            UART_SendLine("\r\nERR: line too long");
        }
    }

    HAL_UART_Receive_IT(uart1_handle, &rx1_byte, 1);
}

