#pragma once

#include "game/ui/widgets/base.h"

UI_WIDGET *UI_Label_Create(const char *text, int32_t width, int32_t height);

void UI_Label_ChangeText(UI_WIDGET *widget, const char *text);

void UI_Label_AddFrame(UI_WIDGET *widget);
void UI_Label_RemoveFrame(UI_WIDGET *widget);
void UI_Label_Flash(UI_WIDGET *widget, bool enable, int32_t rate);
