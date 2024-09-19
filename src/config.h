#pragma once

#include <libtrx/config/config_option.h>

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
void Config_Sanitize(void);
void Config_ApplyChanges(void);
const CONFIG_OPTION *Config_GetOptionMap(void);
