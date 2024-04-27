#pragma once

#include <stdbool.h>
#include <stdint.h>

void Console_Init(void);
void Console_Shutdown(void);
void Console_Draw(void);

void Console_Open(void);
void Console_Close(void);
bool Console_IsOpened(void);

void Console_Confirm(void);

bool Console_HandleKeyDown(uint32_t key);
void Console_HandleChar(uint32_t char_);

void Console_Log(const char *fmt, ...);
void Console_ScrollLogs(void);
