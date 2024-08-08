#pragma once

#include "global/types.h"

BOOL __cdecl Level_LoadTexturePages(HANDLE handle);
BOOL __cdecl Level_LoadRooms(HANDLE handle);
BOOL __cdecl Level_LoadObjects(HANDLE handle);
BOOL __cdecl Level_LoadSprites(HANDLE handle);
BOOL __cdecl Level_LoadItems(HANDLE handle);
BOOL __cdecl Level_LoadDepthQ(HANDLE handle);
BOOL __cdecl Level_LoadPalettes(HANDLE handle);
