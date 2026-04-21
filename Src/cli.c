#include "cli.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

#define CLI_BUFFER_SIZE 128
#define CLI_MAX_ARGS 1


static char cli_buffer[CLI_BUFFER_SIZE];

static void cli_execute(char *cmd);

static void CMD_Ping(int argc, char **argv);
static void CMD_Help(int argc, char **argv);
static void CMD_PwrIn(int argc, char **argv);
static void CMD_CANPwr(int argc, char **argv);
static void CMD_PwrOut(int argc, char **argv);

static const cli_cmd_t commands[] = {
		{"ping", CMD_Ping, "Return pong"},
		{"help", CMD_Help, "Gives list of all functions"},
		{"pwrin", CMD_PwrIn, "Returns Current in and Estimated Power for 120V"},
		{"canpwr", CMD_CANPwr, "Returns Current along CAN line and est power for 12V"},
		{"pwrout", CMD_PwrOut, "Returns Current and Est. Power for each of 4 12V loads."}
};

#define NUM_COMMANDS ((sizeof(commands)) / (sizeof(commands[0])))

uint8_t g_inPwr_readFlag = 0;
uint8_t g_canPwr_readFlag = 0;
uint8_t g_shunt_readFlag = 0;


// Public CLI Functions

HAL_StatusTypeDef CLI_Init(void)
{
    return UART_SendString("> ");
}

HAL_StatusTypeDef CLI_Process(void) {
    if (UART_LineReady()) {
        UART_GetLine(cli_buffer, CLI_BUFFER_SIZE);
        cli_execute(cli_buffer);
        return UART_SendString("> ");  // only the prompt matters
    }
    return HAL_OK;
}

// Command Implementations

static void CMD_Ping(int argc, char **argv) {
	UART_SendLine("Pong");
}

static void CMD_Help(int argc, char **argv)
{
    UART_SendLine("Commands:");
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        UART_SendString(" ");
        UART_SendString(commands[i].name);
        UART_SendString(" - ");
        UART_SendLine(commands[i].help);
    }
}

static void CMD_PwrIn(int argc, char **argv){
	g_inPwr_readFlag = 1;
}

static void CMD_CANPwr(int argc, char **argv){
	g_canPwr_readFlag = 1;
}

static void CMD_PwrOut(int argc, char **argv){
	g_shunt_readFlag = 1;
}


// Command Execution

static int cli_tokenize(char *line, char **argv)
{
    int argc = 0;

    char *tok = strtok(line, " ");

    while (tok && argc < CLI_MAX_ARGS)
    {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }

    return argc;
}

static void cli_execute(char *line)
{
    char *argv[CLI_MAX_ARGS];
    int argc = cli_tokenize(line, argv);

    if (argc == 0)
        return;

    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (strcmp(argv[0], commands[i].name) == 0)
        {
            commands[i].fn(argc, argv);
            return;
        }
    }

    UART_SendLine("Unknown command");
}


