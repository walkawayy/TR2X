#pragma once

#include "game/ui/widgets/base.h"

UI_WIDGET *UI_Prompt_Create(int32_t width, int32_t height);

void UI_Prompt_SetFocus(UI_WIDGET *widget, bool is_focused);
void UI_Prompt_Clear(UI_WIDGET *widget);
