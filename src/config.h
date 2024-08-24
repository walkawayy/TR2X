#pragma once

#include <stdbool.h>

typedef struct {
    bool loaded;
} CONFIG;

extern CONFIG g_Config;

bool Config_Read(void);
bool Config_Write(void);
