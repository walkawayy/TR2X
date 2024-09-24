#include "game/ui/common.h"

#include <libtrx/vector.h>

void UI_Init(void)
{
    UI_Events_Init();
}

void UI_Shutdown(void)
{
    UI_Events_Shutdown();
}

void UI_HandleKeyDown(const uint32_t key)
{
    const UI_EVENT event = {
        .name = "key_down",
        .sender = NULL,
        .data = (void *)(uintptr_t)key,
    };
    UI_Events_Fire(&event);
}

void UI_HandleKeyUp(const uint32_t key)
{
    const UI_EVENT event = {
        .name = "key_up",
        .sender = NULL,
        .data = (void *)(uintptr_t)key,
    };
    UI_Events_Fire(&event);
}

void UI_HandleChar(const uint32_t char_)
{
    const UI_EVENT event = {
        .name = "char",
        .sender = NULL,
        .data = (void *)(uintptr_t)char_,
    };
    UI_Events_Fire(&event);
}
