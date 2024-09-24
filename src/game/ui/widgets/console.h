#pragma once

#include "game/ui/widgets/base.h"

UI_WIDGET *UI_Console_Create(void);

void UI_Console_HandleOpen(UI_WIDGET *widget);
void UI_Console_HandleClose(UI_WIDGET *widget);
void UI_Console_HandleLog(UI_WIDGET *widget, const char *text);
void UI_Console_ScrollLogs(UI_WIDGET *widget);
