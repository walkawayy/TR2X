#pragma once

#include "global/types.h"

BOOL __cdecl Level_LoadTexturePages(HANDLE handle);
BOOL __cdecl Level_LoadRooms(HANDLE handle);
BOOL __cdecl Level_LoadObjects(HANDLE handle);
BOOL __cdecl Level_LoadSprites(HANDLE handle);
BOOL __cdecl Level_LoadItems(HANDLE handle);
BOOL __cdecl Level_LoadDepthQ(HANDLE handle);
BOOL __cdecl Level_LoadPalettes(HANDLE handle);
BOOL __cdecl Level_LoadCameras(HANDLE handle);
BOOL __cdecl Level_LoadSoundEffects(HANDLE handle);
BOOL __cdecl Level_LoadBoxes(HANDLE handle);
BOOL __cdecl Level_LoadAnimatedTextures(HANDLE handle);
BOOL __cdecl Level_LoadCinematic(HANDLE handle);
BOOL __cdecl Level_LoadDemo(HANDLE handle);
void __cdecl Level_LoadDemoExternal(const char *level_name);
