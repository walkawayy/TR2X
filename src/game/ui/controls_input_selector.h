#pragma once

#include "game/ui/base.h"
#include "game/ui/controllers/controls.h"

UI_WIDGET *UI_ControlsInputSelector_Create(
    INPUT_ROLE input_role, UI_CONTROLS_CONTROLLER *controller);
