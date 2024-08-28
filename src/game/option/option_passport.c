#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

typedef enum {
    PASSPORT_MODE_BROWSE = 0,
    PASSPORT_MODE_LOAD_GAME = 1,
    PASSPORT_MODE_SELECT_LEVEL = 2,
} PASSPORT_MODE;

void Option_Passport_Shutdown(INVENTORY_ITEM *const item)
{
    Text_Remove(g_Inv_ItemText[IT_NAME]);
    g_Inv_ItemText[IT_NAME] = NULL;
    Text_Remove(g_Inv_LevelText);
    g_Inv_LevelText = NULL;
    Text_Remove(g_Inv_RingText);
    g_Inv_RingText = NULL;
    Text_Remove(g_Inv_TagText);
    g_Inv_TagText = NULL;
    Text_Remove(g_PasswordText1);
    g_PasswordText1 = NULL;

    Remove_Requester(&g_LoadGameRequester);
    Remove_Requester(&g_SaveGameRequester);

    g_PassportMode = PASSPORT_MODE_BROWSE;
}
