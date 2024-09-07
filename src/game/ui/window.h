#pragma once

#include "game/ui/base.h"

UI_WIDGET *UI_Window_Create(
    UI_WIDGET *root, int32_t border_top, int32_t border_right,
    int32_t border_bottom, int32_t border_left);
