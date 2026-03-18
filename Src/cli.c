#include "cli.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

#define CLI_BUFFER_SIZE 128

static char cli_buffer[CLI_BUFFER_SIZE];

static void cli_execute(char *cmd);

void CLI_Init(void)
{
    UART_SendString("> ");
}


