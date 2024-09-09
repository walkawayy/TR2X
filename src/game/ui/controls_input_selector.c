#include "game/ui/controls_input_selector.h"

#include "game/ui/label.h"
#include "game/ui/stack.h"

#include <libtrx/memory.h>

typedef struct {
    UI_WIDGET_VTABLE vtable;
    INPUT_ROLE input_role;
    UI_WIDGET *label;
    UI_WIDGET *choice;
    UI_WIDGET *container;
    UI_CONTROLS_CONTROLLER *controller;
} UI_CONTROLS_INPUT_SELECTOR;

static void UI_ControlsInputSelector_UpdateText(
    UI_CONTROLS_INPUT_SELECTOR *self);
static int32_t UI_ControlsInputSelector_GetWidth(
    const UI_CONTROLS_INPUT_SELECTOR *self);
static int32_t UI_ControlsInputSelector_GetHeight(
    const UI_CONTROLS_INPUT_SELECTOR *self);
static void UI_ControlsInputSelector_SetPosition(
    UI_CONTROLS_INPUT_SELECTOR *self, int32_t x, int32_t y);
static void UI_ControlsInputSelector_Control(UI_CONTROLS_INPUT_SELECTOR *self);
static void UI_ControlsInputSelector_Free(UI_CONTROLS_INPUT_SELECTOR *self);

static void UI_ControlsInputSelector_UpdateText(
    UI_CONTROLS_INPUT_SELECTOR *const self)
{
    const uint16_t key =
        Input_GetAssignedKey(self->controller->active_layout, self->input_role);
    if (Input_GetKeyName(key) == NULL) {
        UI_Label_ChangeText(self->choice, "BAD");
    } else {
        UI_Label_ChangeText(self->choice, Input_GetKeyName(key));
    }
    UI_Label_ChangeText(self->label, Input_GetRoleName(self->input_role));
}

static int32_t UI_ControlsInputSelector_GetWidth(
    const UI_CONTROLS_INPUT_SELECTOR *const self)
{
    return self->container->get_width(self->container);
}

static int32_t UI_ControlsInputSelector_GetHeight(
    const UI_CONTROLS_INPUT_SELECTOR *const self)
{
    return self->container->get_height(self->container);
}

static void UI_ControlsInputSelector_SetPosition(
    UI_CONTROLS_INPUT_SELECTOR *const self, const int32_t x, const int32_t y)
{
    return self->container->set_position(self->container, x, y);
}

static void UI_ControlsInputSelector_Control(
    UI_CONTROLS_INPUT_SELECTOR *const self)
{
    // Sync outlines
    UI_Label_RemoveFrame(self->label);
    UI_Label_RemoveFrame(self->choice);
    if (self->controller->active_role == self->input_role) {
        if (self->controller->state == UI_CONTROLS_STATE_NAVIGATE_INPUTS
            || self->controller->state
                == UI_CONTROLS_STATE_NAVIGATE_INPUTS_DEBOUNCE) {
            UI_Label_AddFrame(self->label);
        } else if (
            self->controller->state == UI_CONTROLS_STATE_LISTEN
            || self->controller->state == UI_CONTROLS_STATE_LISTEN_DEBOUNCE) {
            UI_Label_AddFrame(self->choice);
        }
    }

    UI_ControlsInputSelector_UpdateText(self);

    // Flash conflicts
    UI_Label_Flash(self->choice, false, 0);
    const uint16_t key1 =
        Input_GetAssignedKey(self->controller->active_layout, self->input_role);
    for (INPUT_ROLE role = 0; role < INPUT_ROLE_NUMBER_OF; role++) {
        const uint16_t key2 =
            Input_GetAssignedKey(self->controller->active_layout, role);
        if (role != self->input_role && key1 == key2) {
            UI_Label_Flash(self->choice, true, 20);
        }
    }
}

static void UI_ControlsInputSelector_Free(
    UI_CONTROLS_INPUT_SELECTOR *const self)
{
    self->label->free(self->label);
    self->choice->free(self->choice);
    self->container->free(self->container);
    Memory_Free(self);
}

UI_WIDGET *UI_ControlsInputSelector_Create(
    const INPUT_ROLE input_role, UI_CONTROLS_CONTROLLER *const controller)
{
    UI_CONTROLS_INPUT_SELECTOR *const self =
        Memory_Alloc(sizeof(UI_CONTROLS_INPUT_SELECTOR));
    self->vtable = (UI_WIDGET_VTABLE) {
        .control = (UI_WIDGET_CONTROL)UI_ControlsInputSelector_Control,
        .get_width = (UI_WIDGET_GET_WIDTH)UI_ControlsInputSelector_GetWidth,
        .get_height = (UI_WIDGET_GET_HEIGHT)UI_ControlsInputSelector_GetHeight,
        .set_position =
            (UI_WIDGET_SET_POSITION)UI_ControlsInputSelector_SetPosition,
        .free = (UI_WIDGET_FREE)UI_ControlsInputSelector_Free,
    };

    self->controller = controller;
    self->input_role = input_role;

    self->label = UI_Label_Create("", 140, 15);
    self->choice = UI_Label_Create("", 70, 15);
    self->container = UI_Stack_Create(UI_STACK_LAYOUT_HORIZONTAL);
    UI_Stack_AddChild(self->container, self->choice);
    UI_Stack_AddChild(self->container, self->label);

    // update the text on init
    UI_ControlsInputSelector_UpdateText(self);

    return (UI_WIDGET *)self;
}
