#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <dinput.h>

static void Option_Controls_InitText(void);
static void Option_Controls_ShutdownText(void);

static void Option_Controls_Control_Navigate(void);
static void Option_Controls_Control_WaitToListen(void);
static void Option_Controls_Control_Listen(void);
static void Option_Controls_Control_EndListen(void);

typedef enum {
    STATE_NAVIGATE,
    STATE_WAIT_TO_LISTEN,
    STATE_LISTEN,
    STATE_END_LISTEN,
} STATE;

static int32_t m_Cursor = 0;
static int32_t m_State = STATE_NAVIGATE;

static void Option_Controls_InitText(void)
{
    g_ControlsText[0] = Text_Create(
        0, -50, 0,
        g_GF_PCStrings
            [g_LayoutPage == 0 ? GF_S_PC_DEFAULT_KEYS : GF_S_PC_USER_KEYS]);
    Text_CentreH(g_ControlsText[0], 1);
    Text_CentreV(g_ControlsText[0], 1);
    S_ShowControls();
    m_Cursor = -1;
    Text_AddBackground(g_ControlsText[0], 0, 0, 0, 0, 48, 0, 0, 0);
    Text_AddOutline(g_ControlsText[0], 1, 15, 0, 0);
}

static void Option_Controls_ShutdownText(void)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Remove(g_ControlsTextA[i]);
        g_ControlsTextA[i] = NULL;

        Text_Remove(g_ControlsTextB[i]);
        g_ControlsTextB[i] = NULL;
    }

    for (int32_t i = 0; i < 2; i++) {
        Text_Remove(g_ControlsText[i]);
        g_ControlsText[i] = NULL;
    }
}

static void Option_Controls_Control_Navigate(void)
{
    if (g_InputDB & (IN_LEFT | IN_RIGHT)) {
        if (m_Cursor == -1) {
            g_LayoutPage = 1 - g_LayoutPage;
            S_ChangeCtrlText();
            Option_Controls_FlashConflicts();
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
            if (m_Cursor < 7) {
                m_Cursor += 7;
            } else if (m_Cursor < 14) {
                m_Cursor -= 7;
            } else {
                m_Cursor = 7;
            }
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);
        }
    } else if (
        (g_InputDB & IN_DESELECT)
        || ((g_InputDB & IN_SELECT) && m_Cursor == -1)) {
        Text_Remove(g_ControlsText[0]);
        g_ControlsText[0] = NULL;
        Text_Remove(g_ControlsText[1]);
        g_ControlsText[1] = NULL;
        S_RemoveCtrlText();
        Option_Controls_DefaultConflict();
        return;
    }

    if (g_LayoutPage == 0) {
        g_Input = 0;
        g_InputDB = 0;
        return;
    }

    if (g_InputDB & IN_SELECT) {
        m_State = STATE_WAIT_TO_LISTEN;
        g_ControlsTextB[m_Cursor]->pos.z = 16;
        Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
        Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        g_ControlsTextA[m_Cursor]->pos.z = 0;
        Text_AddBackground(g_ControlsTextA[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
        Text_AddOutline(g_ControlsTextA[m_Cursor], 1, 15, 0, 0);
    } else if (g_InputDB & IN_FORWARD) {
        if (m_Cursor == -1) {
            Text_RemoveBackground(g_ControlsText[0]);
            Text_RemoveOutline(g_ControlsText[0]);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        }

        m_Cursor--;
        if (m_Cursor < -1) {
            m_Cursor = 13;
        }

        if (m_Cursor == -1) {
            Text_AddBackground(g_ControlsText[0], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsText[0], 1, 15, 0, 0);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);
        }
        g_Input = 0;
        g_InputDB = 0;
    } else if (g_InputDB & IN_BACK) {
        if (m_Cursor == -1) {
            Text_RemoveBackground(g_ControlsText[0]);
            Text_RemoveOutline(g_ControlsText[0]);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 16;
            Text_RemoveBackground(g_ControlsTextB[m_Cursor]);
            Text_RemoveOutline(g_ControlsTextB[m_Cursor]);
        }

        m_Cursor++;
        if (m_Cursor > 13) {
            m_Cursor = -1;
        }

        if (m_Cursor == -1) {
            Text_AddBackground(g_ControlsText[0], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsText[0], 1, 15, 0, 0);
        } else {
            g_ControlsTextB[m_Cursor]->pos.z = 0;
            Text_AddBackground(
                g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
            Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);
        }
    }

    g_Input = 0;
    g_InputDB = 0;
}

static void Option_Controls_Control_WaitToListen(void)
{
    if (g_InputDB & IN_SELECT) {
        g_Input = 0;
        g_InputDB = 0;
        return;
    }

    m_State = STATE_LISTEN;
    g_Input = 0;
    g_InputDB = 0;
}

static void Option_Controls_Control_Listen(void)
{
    int32_t pressed = 0;

    if (g_JoyKeys) {
        for (int32_t i = 0; i < 32; i++) {
            if (g_JoyKeys & (1 << i)) {
                pressed = i;
                break;
            }
        }
        if (!pressed) {
            return;
        }
        pressed += 0x100;
    } else {
        for (int32_t i = 0; i < 256; i++) {
            if (g_DIKeys[i] & 0x80) {
                pressed = i;
                break;
            }
        }
        if (!pressed) {
            return;
        }
    }

    if (!pressed
        // clang-format off
        || g_KeyNames[pressed] == NULL
        || pressed == DIK_RETURN
        || pressed == DIK_LEFT
        || pressed == DIK_RIGHT
        || pressed == DIK_UP
        || pressed == DIK_DOWN
        // clang-format on
    ) {
        g_Input = 0;
        g_InputDB = 0;
        return;
    }

    if (pressed != DIK_ESCAPE) {
        g_Layout[g_LayoutPage].key[m_Cursor] = pressed;
        Text_ChangeText(g_ControlsTextA[m_Cursor], g_KeyNames[pressed]);
    }

    g_ControlsTextA[m_Cursor]->pos.z = 16;
    Text_RemoveBackground(g_ControlsTextA[m_Cursor]);
    Text_RemoveOutline(g_ControlsTextA[m_Cursor]);

    g_ControlsTextB[m_Cursor]->pos.z = 0;
    Text_AddBackground(g_ControlsTextB[m_Cursor], 0, 0, 0, 0, 0, 0, 0, 0);
    Text_AddOutline(g_ControlsTextB[m_Cursor], 1, 15, 0, 0);

    m_State = STATE_END_LISTEN;
    Option_Controls_FlashConflicts();

    g_Input = 0;
    g_InputDB = 0;
}

static void Option_Controls_Control_EndListen(void)
{
    if (g_Layout[g_LayoutPage].key[m_Cursor] & 0x100) {
        if (!(g_JoyKeys & (1 << g_Layout[g_LayoutPage].key[m_Cursor]))) {
            m_State = STATE_NAVIGATE;
        }
    } else if ((g_DIKeys[g_Layout[g_LayoutPage].key[m_Cursor]] & 0x80u) == 0) {
        m_State = STATE_NAVIGATE;
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LCONTROL) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RCONTROL;
        }
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LSHIFT) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RSHIFT;
        }
        if (g_Layout[g_LayoutPage].key[m_Cursor] == DIK_LMENU) {
            g_Layout[g_LayoutPage].key[m_Cursor] = DIK_RMENU;
        }
        Option_Controls_FlashConflicts();
    }
    g_Input = 0;
    g_InputDB = 0;
}

void __cdecl Option_Controls_FlashConflicts(void)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Flash(g_ControlsTextA[i], 0, 0);
        for (int32_t j = 0; j < 14; j++) {
            const uint16_t key1 = g_Layout[g_LayoutPage].key[i];
            const uint16_t key2 = g_Layout[g_LayoutPage].key[j];
            if (i != j && key1 == key2) {
                Text_Flash(g_ControlsTextA[i], 1, 20);
            }
        }
    }
}

void __cdecl Option_Controls_DefaultConflict(void)
{
    for (int32_t i = 0; i < 14; i++) {
        g_ConflictLayout[i] = false;
        for (int32_t j = 0; j < 14; j++) {
            uint16_t key1 = g_Layout[0].key[i];
            uint16_t key2 = g_Layout[1].key[j];
            if (key1 == key2) {
                g_ConflictLayout[i] = true;
            }
        }
    }
}

void Option_Controls_Shutdown(INVENTORY_ITEM *const item)
{
    Option_Controls_ShutdownText();
}

void __cdecl Option_Controls(INVENTORY_ITEM *const item)
{
    if (g_ControlsText[0] == NULL) {
        Option_Controls_InitText();
    }

    switch (m_State) {
    case STATE_NAVIGATE:
        Option_Controls_Control_Navigate();
        break;

    case STATE_WAIT_TO_LISTEN:
        Option_Controls_Control_WaitToListen();
        break;

    case STATE_LISTEN:
        Option_Controls_Control_Listen();
        break;

    case STATE_END_LISTEN:
        Option_Controls_Control_EndListen();
        break;

    default:
        g_Input = 0;
        g_InputDB = 0;
    }
}
