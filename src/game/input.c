#include "game/input.h"

#include "specific/s_input.h"

bool Input_Update(void)
{
    return S_Input_Update();
}
