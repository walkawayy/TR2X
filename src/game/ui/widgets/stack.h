#pragma once

#include "game/ui/widgets/base.h"

typedef enum {
    UI_STACK_LAYOUT_HORIZONTAL,
    UI_STACK_LAYOUT_VERTICAL,
} UI_STACK_LAYOUT;

UI_WIDGET *UI_Stack_Create(UI_STACK_LAYOUT layout);
void UI_Stack_AddChild(UI_WIDGET *self, UI_WIDGET *child);
void UI_Stack_DoLayout(UI_WIDGET *self);
