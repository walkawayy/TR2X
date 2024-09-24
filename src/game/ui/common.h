#pragma once

#include "game/ui/events.h"

void UI_Init(void);
void UI_Shutdown(void);

void UI_HandleKeyDown(uint32_t key);
void UI_HandleKeyUp(uint32_t key);
void UI_HandleChar(uint32_t char_);
