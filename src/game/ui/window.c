#include "game/ui/window.h"

#include "game/text.h"

#include <libtrx/memory.h>

typedef struct {
    UI_WIDGET_VTABLE vtable;
    TEXTSTRING *text;
    UI_WIDGET *root;
    struct {
        int32_t left;
        int32_t right;
        int32_t top;
        int32_t bottom;
    } border;
} UI_WINDOW;

static int32_t UI_Window_GetWidth(const UI_WINDOW *self);
static int32_t UI_Window_GetHeight(const UI_WINDOW *self);
static void UI_Window_SetPosition(UI_WINDOW *self, int32_t x, int32_t y);
static void UI_Window_Control(UI_WINDOW *self);
static void UI_Window_Free(UI_WINDOW *self);

static int32_t UI_Window_GetWidth(const UI_WINDOW *const self)
{
    return self->root->get_width(self->root) + self->border.left
        + self->border.right;
}

static int32_t UI_Window_GetHeight(const UI_WINDOW *const self)
{
    return self->root->get_height(self->root) + self->border.top
        + self->border.bottom;
}

static void UI_Window_SetPosition(
    UI_WINDOW *const self, const int32_t x, const int32_t y)
{
    self->root->set_position(
        self->root, x + self->border.left, y + self->border.top);

    Text_SetPos(self->text, x, y);

    const int32_t w = UI_Window_GetWidth(self);
    const int32_t h = UI_Window_GetHeight(self);
    Text_AddBackground(self->text, w, h, w / 2, 0, 0, INV_COLOR_BLACK, NULL, 0);
    Text_AddOutline(self->text, true, INV_COLOR_BLUE, NULL, 0);
}

static void UI_Window_Control(UI_WINDOW *const self)
{
    if (self->root->control != NULL) {
        self->root->control(self->root);
    }
}

static void UI_Window_Free(UI_WINDOW *const self)
{
    Text_Remove(self->text);
    Memory_Free(self);
}

UI_WIDGET *UI_Window_Create(
    UI_WIDGET *const root, const int32_t border_top, const int32_t border_right,
    const int32_t border_bottom, const int32_t border_left)
{
    UI_WINDOW *const self = Memory_Alloc(sizeof(UI_WINDOW));
    self->vtable = (UI_WIDGET_VTABLE) {
        .control = (UI_WIDGET_CONTROL)UI_Window_Control,
        .get_width = (UI_WIDGET_GET_WIDTH)UI_Window_GetWidth,
        .get_height = (UI_WIDGET_GET_HEIGHT)UI_Window_GetHeight,
        .set_position = (UI_WIDGET_SET_POSITION)UI_Window_SetPosition,
        .free = (UI_WIDGET_FREE)UI_Window_Free,
    };

    self->root = root;
    self->text = Text_Create(0, 0, 0, "");
    self->text->pos.z = 32;
    self->border.top = border_top;
    self->border.right = border_right;
    self->border.bottom = border_bottom;
    self->border.left = border_left;
    return (UI_WIDGET *)self;
}
