#include "game/hwr.h"

#include "global/vars.h"

#include <d3d.h>

HRESULT HWR_DrawPrimitive(
    D3DPRIMITIVETYPE primitive_type, LPVOID vertices, DWORD vtx_count,
    bool is_no_clip)
{
    return g_D3DDev->lpVtbl->DrawPrimitive(
        g_D3DDev, primitive_type, D3DVT_TLVERTEX, vertices, vtx_count,
        is_no_clip ? D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP : 0);
}
