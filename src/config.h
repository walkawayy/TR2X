#pragma once

#include <stdbool.h>

typedef struct {
    bool loaded;

    struct {
        bool fix_m16_accuracy;
    } gameplay;
} CONFIG;

extern CONFIG g_Config;

bool Config_Read(void);
bool Config_Write(void);
