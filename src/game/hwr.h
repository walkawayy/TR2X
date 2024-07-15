#pragma once

#include <d3dtypes.h>
#include <stdbool.h>
#include <windows.h>

HRESULT HWR_DrawPrimitive(
    D3DPRIMITIVETYPE primitive_type, LPVOID vertices, DWORD vtx_count,
    bool is_no_clip);

void __cdecl HWR_InitState(void);
void __cdecl HWR_ResetTexSource(void);
void __cdecl HWR_ResetColorKey(void);
