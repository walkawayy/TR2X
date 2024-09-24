#pragma once

#include "game/ui/widgets/base.h"

#define UI_LABEL_AUTO_SIZE (-1)

UI_WIDGET *UI_Label_Create(const char *text, int32_t width, int32_t height);

void UI_Label_ChangeText(UI_WIDGET *widget, const char *text);
const char *UI_Label_GetText(UI_WIDGET *widget);

void UI_Label_AddFrame(UI_WIDGET *widget);
void UI_Label_RemoveFrame(UI_WIDGET *widget);
void UI_Label_Flash(UI_WIDGET *widget, bool enable, int32_t rate);
void UI_Label_SetScale(UI_WIDGET *widget, float scale);
int32_t UI_Label_MeasureTextWidth(UI_WIDGET *widget);
