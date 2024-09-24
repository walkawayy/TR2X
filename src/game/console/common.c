#include "game/console/common.h"

#include "game/console/setup.h"
#include "game/input.h"
#include "game/text.h"
#include "game/ui/widgets/console.h"

#include <libtrx/game/console/common.h>

static bool m_IsOpened = false;
static UI_WIDGET *m_Console;

void Console_Init(void)
{
    m_Console = UI_Console_Create();
}

void Console_Shutdown(void)
{
    if (m_Console != NULL) {
        m_Console->free(m_Console);
        m_Console = NULL;
    }

    m_IsOpened = false;
}

void Console_Open(void)
{
    if (m_IsOpened) {
        UI_Console_HandleClose(m_Console);
    }
    m_IsOpened = true;
    UI_Console_HandleOpen(m_Console);
}

void Console_Close(void)
{
    UI_Console_HandleClose(m_Console);
    m_IsOpened = false;
}

bool Console_IsOpened(void)
{
    return m_IsOpened;
}

int32_t Console_GetMaxLineLength(void)
{
    return TEXT_MAX_STRING_SIZE - 1;
}

void Console_LogImpl(const char *const text)
{
    UI_Console_HandleLog(m_Console, text);
}

CONSOLE_COMMAND **Console_GetCommands(void)
{
    return g_ConsoleCommands;
}

void Console_Draw(void)
{
    UI_Console_ScrollLogs(m_Console);

#if 0
// TODO: draw screen quad
    int32_t sx = 0;
    int32_t sw = Viewport_GetWidth();
    int32_t sh = Screen_GetRenderScale(
        // not entirely accurate, but good enough
        TEXT_HEIGHT * m_PromptScale
            + MAX_LOG_LINES * TEXT_HEIGHT * m_LogScale,
        RSR_TEXT);
    int32_t sy = Viewport_GetHeight() - sh;

    RGBA_8888 top = { 0, 0, 0, 0 };
    RGBA_8888 bottom = { 0, 0, 0, 196 };

    Output_DrawScreenGradientQuad(sx, sy, sw, sh, top, top, bottom, bottom);
#endif
}
