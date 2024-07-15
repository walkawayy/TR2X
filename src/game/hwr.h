#pragma once

#include "global/types.h"

#include <d3dtypes.h>
#include <stdbool.h>
#include <windows.h>

HRESULT HWR_DrawPrimitive(
    D3DPRIMITIVETYPE primitive_type, LPVOID vertices, DWORD vtx_count,
    bool is_no_clip);

void __cdecl HWR_InitState(void);
void __cdecl HWR_ResetTexSource(void);
void __cdecl HWR_ResetColorKey(void);
void __cdecl HWR_ResetZBuffer(void);
void __cdecl HWR_TexSource(HWR_TEXTURE_HANDLE tex_source);
void __cdecl HWR_EnableColorKey(bool state);
void __cdecl HWR_EnableZBuffer(bool z_write_enable, bool z_enable);
void __cdecl HWR_BeginScene(void);
