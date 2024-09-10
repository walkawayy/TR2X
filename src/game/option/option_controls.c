#include "game/option/option.h"
#include "game/ui/controls_dialog.h"
#include "global/vars.h"

#include <libtrx/utils.h>

static UI_WIDGET *m_ControlsDialog;
static UI_CONTROLS_CONTROLLER m_ControlsDialogController;

void Option_Controls_Shutdown(void)
{
    if (m_ControlsDialog != NULL) {
        m_ControlsDialog->free(m_ControlsDialog);
        m_ControlsDialog = NULL;
    }
    Input_CheckConflictsWithDefaults();
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
