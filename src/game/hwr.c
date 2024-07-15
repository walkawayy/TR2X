#include "game/hwr.h"

#include "global/funcs.h"
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

void __cdecl HWR_InitState(void)
{
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_TEXTUREPERSPECTIVE,
        g_SavedAppSettings.perspective_correct ? TRUE : FALSE);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_DITHERENABLE,
        g_SavedAppSettings.dither ? TRUE : FALSE);

    g_AlphaBlendEnabler = g_CurrentDisplayAdapter.shade_restricted
        ? D3DRENDERSTATE_STIPPLEDALPHA
        : D3DRENDERSTATE_ALPHABLENDENABLE;

    const DWORD texture_filter = g_SavedAppSettings.bilinear_filtering
        ? D3DFILTER_LINEAR
        : D3DFILTER_NEAREST;
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_TEXTUREMAG, texture_filter);
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_TEXTUREMIN, texture_filter);

    const DWORD blend_mode =
        (g_CurrentDisplayAdapter.hw_device_desc.dpcTriCaps.dwTextureBlendCaps
         & D3DPTBLENDCAPS_MODULATEALPHA)
        ? D3DTBLEND_MODULATEALPHA
        : D3DTBLEND_MODULATE;
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_TEXTUREMAPBLEND, blend_mode);

    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);

    HWR_ResetTexSource();
    HWR_ResetColorKey();
    HWR_ResetZBuffer();
}

void __cdecl HWR_ResetTexSource(void)
{
    g_CurrentTexSource = 0;
    g_D3DDev->lpVtbl->SetRenderState(g_D3DDev, D3DRENDERSTATE_TEXTUREHANDLE, 0);
    g_D3DDev->lpVtbl->SetRenderState(g_D3DDev, D3DRENDERSTATE_FLUSHBATCH, 0);
}

void __cdecl HWR_ResetColorKey(void)
{
    g_ColorKeyState = FALSE;
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev,
        g_TexturesAlphaChannel ? D3DRENDERSTATE_ALPHABLENDENABLE
                               : D3DRENDERSTATE_COLORKEYENABLE,
        FALSE);
}

void __cdecl HWR_ResetZBuffer(void)
{
    g_ZEnableState = FALSE;
    g_ZWriteEnableState = FALSE;
    if (g_ZBufferSurface != NULL) {
        g_D3DDev->lpVtbl->SetRenderState(
            g_D3DDev, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
        g_D3DDev->lpVtbl->SetRenderState(
            g_D3DDev, D3DRENDERSTATE_ZENABLE,
            g_SavedAppSettings.zbuffer ? TRUE : FALSE);
    } else {
        g_D3DDev->lpVtbl->SetRenderState(
            g_D3DDev, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        g_D3DDev->lpVtbl->SetRenderState(
            g_D3DDev, D3DRENDERSTATE_ZENABLE, FALSE);
    }
    g_D3DDev->lpVtbl->SetRenderState(
        g_D3DDev, D3DRENDERSTATE_ZWRITEENABLE, FALSE);
}

void __cdecl HWR_TexSource(HWR_TEXTURE_HANDLE tex_source)
{
    if (g_CurrentTexSource != tex_source) {
        g_D3DDev->lpVtbl->SetRenderState(
            g_D3DDev, D3DRENDERSTATE_TEXTUREHANDLE, tex_source);
        g_CurrentTexSource = tex_source;
    }
}
