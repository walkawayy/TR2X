#include "game/option/option.h"
#include "game/ui/controls_dialog.h"
#include "global/vars.h"

#include <libtrx/utils.h>

static UI_WIDGET *m_ControlsDialog;
static UI_CONTROLS_CONTROLLER m_ControlsDialogController;

void __cdecl Option_Controls_DefaultConflict(void)
{
    for (int32_t i = 0; i < INPUT_ROLE_NUMBER_OF; i++) {
        g_ConflictLayout[i] = false;
        for (int32_t j = 0; j < INPUT_ROLE_NUMBER_OF; j++) {
            const uint16_t key1 = Input_GetAssignedKey(0, i);
            const uint16_t key2 = Input_GetAssignedKey(1, j);
            if (key1 == key2) {
                g_ConflictLayout[i] = true;
            }
        }
    }
}

void Option_Controls_Shutdown(void)
{
    m_ControlsDialog->free(m_ControlsDialog);
    m_ControlsDialog = NULL;
    Option_Controls_DefaultConflict();
}

void __cdecl Option_Controls(INVENTORY_ITEM *const item)
{
    if (m_ControlsDialog == NULL) {
        m_ControlsDialog =
            UI_ControlsDialog_Create(&m_ControlsDialogController);
        m_ControlsDialog->set_position(
            m_ControlsDialog,
            (640 - m_ControlsDialog->get_width(m_ControlsDialog)) / 2,
            (480 - m_ControlsDialog->get_height(m_ControlsDialog)) * 2 / 3);
    }

    m_ControlsDialog->control(m_ControlsDialog);
    if (m_ControlsDialogController.state == UI_CONTROLS_STATE_EXIT) {
        Option_Controls_Shutdown();
    } else {
        g_Input = 0;
        g_InputDB = 0;
    }
}
