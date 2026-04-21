#ifndef CLI_H
#define CLI_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef CLI_Init(void);
HAL_StatusTypeDef CLI_Process(void);

extern uint8_t g_inPwr_readFlag;
extern uint8_t g_canPwr_readFlag;
extern uint8_t g_shunt_readFlag;

typedef void (*cli_cmd_fn_t)(int argc, char **argv);

typedef struct {
    const char *name;
    cli_cmd_fn_t fn;
    const char *help;
} cli_cmd_t;

#endif
