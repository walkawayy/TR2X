#pragma once

#include "game/ui/base.h"
#include "game/ui/controllers/controls.h"

UI_WIDGET *UI_ControlsColumn_Create(
    int32_t column, UI_CONTROLS_CONTROLLER *controller);
