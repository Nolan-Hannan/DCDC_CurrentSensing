#ifndef CLI_H
#define CLI_H

#include <stdint.h>

void CLI_Init(void);
void CLI_Process(void);

typedef void (*cli_cmd_fn_t)(int argc, char **argv);

typedef struct {
    const char *name;
    cli_cmd_fn_t fn;
    const char *help;
} cli_cmd_t;

#endif
