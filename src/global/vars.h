// This file is autogenerated. To update it, run tools/generate_funcs.

#pragma once

#include "global/types.h"
#include "inject_util.h"

// clang-format off
extern const char *g_TR2XVersion;
#define g_IID_IDirectDrawSurface3 (*(GUID*)0x00463150)
#define g_PerspectiveDistance (*(uint32_t*)0x00464060) // = 0x3000000
#define g_PolyDrawRoutines (*((void(__cdecl  *(*)[9])(const int16_t *))0x00464068))
#define g_RhwFactor (*(float*)0x0046408C) // = 335544320.0f
#define g_CineTrackID (*(int32_t*)0x004640B0) // = 1
#define g_CineTickRate (*(int32_t*)0x004640B8) // = 0x8000
#define g_CD_TrackID (*(int16_t*)0x004640BC) // = -1
#define g_FlipEffect (*(int32_t*)0x004640C4) // = -1
#define g_BoxLines (*(int32_t(*)[12][2])0x00464180)
#define g_AssaultBestTime (*(int32_t*)0x004641F0) // = -1
#define g_EffectRoutines (*((void(__cdecl  *(*)[32])(ITEM_INFO *item))0x004641F8))
#define g_GF_NumSecrets (*(int16_t*)0x004642E8) // = 3
#define g_GF_MusicTracks (*(int16_t(*)[16])0x004642F0)
#define g_CineTargetAngle (*(int16_t*)0x00464310) // = 0x4000
#define g_OverlayStatus (*(int32_t*)0x004644E0) // = 1
#define g_Inv_NFrames (*(int32_t*)0x004644F8)
#define g_Inv_MainObjectsCount (*(int16_t*)0x004654E0) // = 8
#define g_Inv_MainList (*(INVENTORY_ITEM *(*)[])0x00465518)
#define g_Inv_KeysList (*(INVENTORY_ITEM *(*)[])0x004655A8)
#define g_Inv_OptionObjectsCount (*(int16_t*)0x00465604) // = 4
#define g_Inv_OptionList (*(INVENTORY_ITEM *(*)[])0x00465608)
#define g_GymInvOpenEnabled (*(BOOL*)0x00465618) // = TRUE
#define g_Inv_Chosen (*(int16_t*)0x00465A50) // = -1
#define g_Inv_Mode (*(INVENTORY_MODE*)0x00465A54) // = INV_TITLE_MODE
#define g_OptionSoundVolume (*(int16_t*)0x00465A5C) // = 165
#define g_OptionMusicVolume (*(int16_t*)0x00465A60) // = 255
#define g_JumpPermitted (*(int32_t*)0x00465AD4) // = 1
#define g_LaraOldSlideAngle (*(int16_t*)0x00465AD8) // = 1
#define g_LaraControlRoutines (*((void(__cdecl  *(*)[71])(ITEM_INFO *item, COLL_INFO *coll))0x00465CD0))
#define g_ExtraControlRoutines (*((void(__cdecl  *(*)[11])(ITEM_INFO *item, COLL_INFO *coll))0x00465DF0))
#define g_LaraCollisionRoutines (*((void(__cdecl  *(*)[71])(ITEM_INFO *item, COLL_INFO *coll))0x00465E20))
#define g_TextSpacing (*(int8_t(*)[80])0x00466290)
#define g_TextASCIIMap (*(int8_t(*)[])0x004662E0)
#define g_BGND_PaletteIndex (*(int32_t*)0x00466400) // = -1
#define g_GameClassName (*(const char(*)[])0x00466448)
#define g_GameWindowName (*(const char(*)[])0x00466468)
#define g_GameSizer (*(double*)0x00466480) // = 1.0
#define g_GameSizerCopy (*(double*)0x00466488) // = 1.0
#define g_FadeValue (*(int32_t*)0x00466490) // = 0x100000
#define g_FadeLimit (*(int32_t*)0x00466494) // = 0x100000
#define g_FadeAdder (*(int32_t*)0x00466498) // = 0x8000
#define g_ErrorMessages (*(const char *(*)[43])0x004664E8)
#define g_PaletteIndex (*(int32_t*)0x00466BDC)
#define g_DumpX (*(int16_t*)0x00466BE4)
#define g_DumpY (*(int16_t*)0x00466BE6)
#define g_DumpWidth (*(int16_t*)0x00466BE8)
#define g_DumpHeight (*(int16_t*)0x00466BEA)
#define g_Layout (*(CONTROL_LAYOUT(*)[2])0x00466F70)
#define g_MidSort (*(int32_t*)0x0046C300) // = 0
#define g_ViewportAspectRatio (*(float*)0x0046C304) // = 0.0f
#define g_XGenY1 (*(int32_t*)0x0046C308)
#define g_XGenY2 (*(int32_t*)0x0046C30C)
#define g_GouraudTable (*(GOURAUD_ENTRY(*)[256])0x0046C310)
#define g_PhdWinTop (*(int32_t*)0x0046E310)
#define g_PhdSprites (*(PHD_SPRITE(*)[512])0x0046E318)
#define g_LsAdder (*(int32_t*)0x00470318)
#define g_FltWinBottom (*(float*)0x0047031C)
#define g_FltResZBuf (*(float*)0x00470320)
#define g_FltResZ (*(float*)0x00470324)
#define g_Output_InsertTransQuad (*(void(__cdecl **)(int32_t x, int32_t y, int32_t width, int32_t height, int32_t z))0x00470328)
#define g_PhdWinHeight (*(int32_t*)0x0047032C)
#define g_PhdWinCenterX (*(int32_t*)0x00470330)
#define g_PhdWinCenterY (*(int32_t*)0x00470334)
#define g_LsYaw (*(int16_t*)0x00470338)
#define g_Output_InsertTrans8 (*(void(__cdecl **)(const PHD_VBUF *vbuf, int16_t shade))0x0047033C)
#define g_FltWinTop (*(float*)0x00470340)
#define g_SortBuffer (*(SORT_ITEM(*)[4000])0x00470348)
#define g_FltWinLeft (*(float*)0x00478048)
#define g_PhdWinMinY (*(int16_t*)0x0047804C)
#define g_PhdFarZ (*(int32_t*)0x00478058)
#define g_FltRhwOPersp (*(float*)0x0047805C)
#define g_PhdWinBottom (*(int32_t*)0x00478060)
#define g_PhdPersp (*(int32_t*)0x00478064)
#define g_PhdWinLeft (*(int32_t*)0x00478068)
#define g_Output_InsertFlatRect (*(void(__cdecl **)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t z, uint8_t color_idx))0x0047806C)
#define g_Info3DBuffer (*(int16_t(*)[120000])0x00478070)
#define g_PhdWinMaxX (*(int32_t*)0x004B29F0)
#define g_PhdNearZ (*(int32_t*)0x004B29F4)
#define g_FltResZORhw (*(float*)0x004B29F8)
#define g_FltFarZ (*(float*)0x004B29FC)
#define g_FltWinCenterX (*(float*)0x004B2A00)
#define g_FltWinCenterY (*(float*)0x004B2A04)
#define g_PhdScreenHeight (*(int32_t*)0x004B2A08)
#define g_PrintSurfacePtr (*(uint8_t **)0x004B2A0C)
#define g_PhdWinMinX (*(int16_t*)0x004B2A10)
#define g_FltPerspONearZ (*(float*)0x004B2A14)
#define g_FltRhwONearZ (*(float*)0x004B2A18)
#define g_PhdWinMaxY (*(int32_t*)0x004B2A1C)
#define g_Output_InsertSprite (*(void(__cdecl **)(int32_t z, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t sprite_idx, int16_t shade))0x004B2A20)
#define g_FltNearZ (*(float*)0x004B2A24)
#define g_MatrixPtr (*(MATRIX **)0x004B2A28)
#define g_Output_DrawObjectGT3 (*(const int16_t *(__cdecl **)(const int16_t*, int32_t, SORT_TYPE))0x004B2A2C)
#define g_Output_DrawObjectGT4 (*(const int16_t *(__cdecl **)(const int16_t*, int32_t, SORT_TYPE))0x004B2A30)
#define g_RandomTable (*(int32_t(*)[32])0x004B2A38)
#define g_FltPersp (*(float*)0x004B2AB8)
#define g_W2VMatrix (*(MATRIX*)0x004B2AC0)
#define g_Info3DPtr (*(int16_t **)0x004B2AF0)
#define g_PhdWinWidth (*(int32_t*)0x004B2AF4)
#define g_Output_InsertLine (*(void(__cdecl **)(int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t))0x004B2AF8)
#define g_PhdTextureInfo (*(PHD_TEXTURE(*)[0x800])0x004B2B00)
#define g_PhdViewDistance (*(int32_t*)0x004BCB00)
#define g_LsPitch (*(int16_t*)0x004BCB04)
#define g_Output_DrawObjectG4 (*(const int16_t *(__cdecl **)(const int16_t*,int32_t, SORT_TYPE))0x004BCB08)
#define g_ShadesTable (*(int16_t(*)[32])0x004BCB10)
#define g_Output_DrawObjectG3 (*(const int16_t *(__cdecl **)(const int16_t*,int32_t, SORT_TYPE))0x004BCB50)
#define g_MatrixStack (*(MATRIX(*)[])0x004BCB58)
#define g_DepthQTable (*(DEPTHQ_ENTRY(*)[32])0x004BD2D8)
#define g_PhdScreenWidth (*(int32_t*)0x004BF3D8)
#define g_LsDivider (*(int32_t*)0x004BF3DC)
#define g_PhdVBuf (*(PHD_VBUF(*)[1500])0x004BF3E0)
#define g_XBuffer ((void *)0x004CAF60)
#define g_TexturePageBuffer8 (*(uint8_t *(*)[32])0x004D6AE0)
#define g_FltWinRight (*(float*)0x004D6B60)
#define g_LsVectorView (*(XYZ_32*)0x004D6B68)
#define g_WibbleTable (*(float(*)[32])0x004D6B78)
#define g_PhdWinRight (*(int32_t*)0x004D6BF8)
#define g_SurfaceCount (*(int32_t*)0x004D6BFC)
#define g_Sort3DPtr (*(SORT_ITEM **)0x004D6C00)
#define g_WibbleOffset (*(int32_t*)0x004D6C0C)
#define g_IsWibbleEffect (*(int32_t*)0x004D6C10)
#define g_IsWaterEffect (*(int32_t*)0x004D6C14)
#define g_VBuffer (*(VERTEX_INFO(*)[20])0x004D6CD8)
#define g_IsShadeEffect (*(int8_t*)0x004D6F78)
#define g_VBufferD3D (*(D3DTLVERTEX(*)[32])0x004D6F80)
#define g_GamePalette16 (*(PALETTEENTRY(*)[256])0x004D7380)
#define g_CineFrameCurrent (*(int32_t*)0x004D7780)
#define g_CineTickCount (*(int32_t*)0x004D7784)
#define g_OriginalRoom (*(int32_t*)0x004D7788)
#define g_IsChunkyCamera (*(int32_t*)0x004D778C)
#define g_HeightType (*(int32_t*)0x004D7790)
#define g_NoInputCounter (*(int32_t*)0x004D7794)
#define g_IsResetFlag (*(BOOL*)0x004D7798)
#define g_FlipTimer (*(int32_t*)0x004D779C)
#define g_LOSNumRooms (*(int32_t*)0x004D77A0) // = 0
#define g_StopInventory (*(BOOL*)0x004D77A4)
#define g_IsDemoLevelType (*(BOOL*)0x004D77AC)
#define g_IsDemoLoaded (*(BOOL*)0x004D77B0)
#define g_BoundStart (*(int32_t*)0x004D77C0)
#define g_BoundEnd (*(int32_t*)0x004D77C4)
#define g_IsAssaultTimerDisplay (*(int32_t*)0x004D77E0)
#define g_IsAssaultTimerActive (*(BOOL*)0x004D77E4)
#define g_IsMonkAngry (*(BOOL*)0x004D77E8)
#define g_GF_LaraStartAnim (*(int32_t*)0x004D77F0)
#define g_GF_NoFloor (*(int16_t*)0x004D77FC)
#define g_GF_RemoveWeapons (*(int16_t*)0x004D7800)
#define g_GF_RemoveAmmo (*(int16_t*)0x004D7804)
#define g_GF_StartGame (*(int8_t*)0x004D780C)
#define g_AmmoTextInfo (*(TEXTSTRING **)0x004D792C)
#define g_DisplayModeTextInfo (*(TEXTSTRING **)0x004D7930)
#define g_DisplayModeInfoTimer (*(DWORD*)0x004D7934)
#define g_Inv_MainCurrent (*(UINT16*)0x004D7938)
#define g_Inv_KeyObjectsCount (*(UINT16*)0x004D793C)
#define g_Inv_KeysCurrent (*(UINT16*)0x004D7940)
#define g_Inv_OptionCurrent (*(UINT16*)0x004D7944)
#define g_Inv_RingText (*(TEXTSTRING**)0x004D7954)
#define g_Inv_UpArrow1 (*(TEXTSTRING**)0x004D795C)
#define g_Inv_UpArrow2 (*(TEXTSTRING**)0x004D7960)
#define g_Inv_DownArrow1 (*(TEXTSTRING**)0x004D7964)
#define g_Inv_DownArrow2 (*(TEXTSTRING**)0x004D7968)
#define g_InputDB (*(uint32_t*)0x004D796C)
#define g_Inv_IsActive (*(uint16_t*)0x004D7978)
#define g_Inv_ExtraData (*(int32_t(*)[8])0x004D7980)
#define g_Inv_DemoMode (*(BOOL*)0x004D79A0)
#define g_Inv_IsOptionsDelay (*(BOOL*)0x004D79B4)
#define g_Inv_OptionsDelayCounter (*(int32_t*)0x004D79B8)
#define g_SoundOptionLine (*(uint16_t*)0x004D79BC)
#define g_StatsRequester (*(REQUEST_INFO*)0x004D79C0)
#define g_Assault (*(ASSAULT_STATS*)0x004D7BD8)
#define g_LevelItemCount (*(int32_t*)0x004D7C38)
#define g_HealthBarTimer (*(int32_t*)0x004D7C3C)
#define g_SlotsUsed (*(int32_t*)0x004D7C50)
#define g_SoundTrackIds (*(int32_t(*)[128])0x004D7C80)
#define g_BGND_PictureIsReady (*(bool*)0x004D7E88)
#define g_BGND_TexturePageIndexes (*(int32_t(*)[5])0x004D7E90)
#define g_BGND_PageHandles (*(HWR_TEXTURE_HANDLE(*)[5])0x004D7EA8)
#define g_D3DDev (*(LPDIRECT3DDEVICE2*)0x004D7EBC)
#define g_D3D (*(LPDIRECT3D2*)0x004D7EC0)
#define g_D3DView (*(LPDIRECT3DVIEWPORT2*)0x004D7EC4)
#define g_D3DMaterial (*(LPDIRECT3DMATERIAL2*)0x004D7EC8)
#define g_MinWindowClientHeight (*(int32_t*)0x004D7ED0)
#define g_DDrawInterface (*(LPDIRECTDRAW*)0x004D7ED4)
#define g_IsGameWindowChanging (*(bool*)0x004D7ED8)
#define g_MaxWindowHeight (*(int32_t*)0x004D7EDC)
#define g_DDraw (*(LPDIRECTDRAW3*)0x004D7EE0)
#define g_IsGameWindowCreated (*(bool*)0x004D7EE4)
#define g_IsGameWindowUpdating (*(bool*)0x004D7EE8)
#define g_IsDDrawGameWindowShow (*(bool*)0x004D7EEC)
#define g_MinWindowClientWidth (*(int32_t*)0x004D7EF0)
#define g_IsGameWindowShow (*(bool*)0x004D7EF4)
#define g_IsMinWindowSizeSet (*(bool*)0x004D7EF8)
#define g_MaxWindowClientWidth (*(int32_t*)0x004D7EFC)
#define g_GameWindowWidth (*(int32_t*)0x004D7F00)
#define g_IsMinMaxInfoSpecial (*(bool*)0x004D7F04)
#define g_IsGameFullScreen (*(bool*)0x004D7F08)
#define g_IsGameWindowMaximized (*(bool*)0x004D7F0C)
#define g_GameWindowHandle (*(HWND*)0x004D7F10)
#define g_GameWindowHeight (*(int32_t*)0x004D7F14)
#define g_PrimaryDisplayAdapter (*(DISPLAY_ADAPTER_NODE**)0x004D7F18)
#define g_CurrentDisplayAdapter (*(DISPLAY_ADAPTER*)0x004D7F20)
#define g_LockedBufferCount (*(uint32_t*)0x004D8338)
#define g_GameWindowPositionX (*(int32_t*)0x004D833C)
#define g_GameWindowPositionY (*(int32_t*)0x004D8340)
#define g_DisplayAdapterList (*(DISPLAY_ADAPTER_LIST*)0x004D8348)
#define g_MaxWindowClientHeight (*(int32_t*)0x004D8354)
#define g_IsMessageLoopClosed (*(bool*)0x004D8358)
#define g_MaxWindowWidth (*(int32_t*)0x004D835C)
#define g_IsMaxWindowSizeSet (*(bool*)0x004D8360)
#define g_AppResultCode (*(uint32_t*)0x004D8364)
#define g_FullScreenWidth (*(int32_t*)0x004D8368)
#define g_FullScreenHeight (*(int32_t*)0x004D836C)
#define g_FullScreenBPP (*(int32_t*)0x004D8370)
#define g_FullScreenVGA (*(int32_t*)0x004D8374)
#define g_IsGameToExit (*(uint8_t*)0x004D8378)
#define g_GameWindowY (*(int32_t*)0x004D837C)
#define g_GameWindowX (*(int32_t*)0x004D8380)
#define g_IsGameWindowMinimized (*(bool*)0x004D8384)
#define g_MinWindowWidth (*(int32_t*)0x004D8388)
#define g_MinWindowHeight (*(int32_t*)0x004D838C)
#define g_IsGameWindowActive (*(bool*)0x004D8390)
#define g_MessageLoopCounter (*(int32_t*)0x004D8394)
#define g_DInput (*(LPDIRECTINPUT*)0x004D855C)
#define IDID_SysKeyboard (*(LPDIRECTINPUTDEVICE*)0x004D8560)
#define g_ScreenSizer (*(int32_t*)0x004D8568)
#define g_IsVidSizeLock (*(int32_t*)0x004D856C)
#define g_SampleFreqs (*(DWORD(*)[256])0x004D8570)
#define g_SoundAdapterList (*(SOUND_ADAPTER_LIST*)0x004D8970)
#define g_SampleBuffers (*(LPDIRECTSOUNDBUFFER(*)[256])0x004D8980)
#define g_IsSoundEnabled (*(uint8_t*)0x004D8D80)
#define g_DSound (*(LPDIRECTSOUND*)0x004D8D84)
#define g_ChannelSamples (*(int32_t(*)[32])0x004D8D88)
#define g_ChannelBuffers (*(LPDIRECTSOUNDBUFFER(*)[32])0x004D8E08)
#define g_CurrentSoundAdapter (*(SOUND_ADAPTER*)0x004D8E8C)
#define g_PrimarySoundAdapter (*(SOUND_ADAPTER_NODE **)0x004D8EAC)
#define g_RenderBufferSurface (*(LPDDS*)0x004D8EB0)
#define g_DDrawClipper (*(LPDIRECTDRAWCLIPPER*)0x004D8EB4)
#define g_WinVid_Palette (*(PALETTEENTRY(*)[256])0x004D8EB8)
#define g_ThirdBufferSurface (*(LPDDS*)0x004D92B8)
#define g_PictureBufferSurface (*(LPDDS*)0x004D92BC)
#define g_ZBufferSurface (*(LPDDS*)0x004D92C0)
#define g_DDrawPalette (*(LPDIRECTDRAWPALETTE*)0x004D92C4)
#define g_PrimaryBufferSurface (*(LPDDS*)0x004D92C8)
#define g_ColorBitMasks (*(COLOR_BIT_MASKS*)0x004D92E8)
#define g_GameVid_BufRect (*(RECT*)0x004D9318)
#define g_GameVid_Rect (*(RECT*)0x004D9328)
#define g_GameVid_Width (*(int32_t*)0x004D9338)
#define g_GameVid_Height (*(int32_t*)0x004D933C)
#define g_GameVid_BPP (*(int32_t*)0x004D9340)
#define g_GameVid_BufWidth (*(int32_t*)0x004D9344)
#define g_GameVid_BufHeight (*(int32_t*)0x004D9348)
#define g_UVAdd (*(int32_t*)0x004D934C)
#define g_GameVid_IsVga (*(bool*)0x004D9350)
#define g_GameVid_IsWindowedVGA (*(int8_t*)0x004D9351)
#define g_GameVid_IsFullscreenVGA (*(bool*)0x004D9352)
#define g_IsWindowedVGA (*(bool*)0x004D9353)
#define g_Is16bitTextures (*(bool*)0x004D9354)
#define g_NeedToReloadTextures (*(bool*)0x004D9355)
#define g_BackBufferSurface (*(LPDDS*)0x004D9358)
#define g_FloorData (*(int16_t **)0x004D9D94)
#define g_IsFMVPlaying (*(int32_t*)0x004D9EAC)
#define g_CurrentLevel (*(int32_t*)0x004D9EC0)
#define g_LevelComplete (*(int32_t*)0x004D9EC4)
#define g_HWR_VertexBuffer (*(D3DTLVERTEX(*)[MAX_VERTICES])0x004D9ED8)
#define g_CurrentTexSource (*(D3DTEXTUREHANDLE*)0x00519ED8)
#define g_HWR_PageHandles (*(HWR_TEXTURE_HANDLE(*)[32])0x00519EE0)
#define g_HWR_VertexPtr (*(D3DTLVERTEX **)0x00519F60)
#define g_ZEnableState (*(bool*)0x00519F64)
#define g_AlphaBlendEnabler (*(D3DRENDERSTATETYPE*)0x00519F68)
#define g_ColorKeyState (*(bool*)0x00519F6C)
#define g_ZWriteEnableState (*(bool*)0x00519F70)
#define g_HWR_TexturePageIndexes (*(int32_t(*)[MAX_TEXTURE_PAGES])0x00519F78)
#define g_GameMemoryPtr (*(void **)0x0051A0CC)
#define g_ConflictLayout (*(BOOL(*)[14])0x0051A0D0)
#define g_DIKeys (*(uint8_t(*)[256])0x0051A108)
#define g_Input (*(int32_t*)0x0051A208)
#define g_IsVidModeLock (*(int8_t*)0x0051A20C)
#define g_JoyKeys (*(int32_t*)0x0051A210)
#define g_GameModule (*(HINSTANCE*)0x0051A238)
#define g_CmdLine (*(char **)0x0051A23C)
#define g_ScreenshotCounter (*(int32_t*)0x0051A240)
#define g_PhdWinRect (*(RECT*)0x0051B918)
#define g_HiRes (*(int32_t*)0x0051B928)
#define g_GamePalette8 (*(RGB_888(*)[256])0x0051B930)
#define g_WinVidNeedToResetBuffers (*(bool*)0x0051BC30)
#define g_SavedAppSettings (*(APP_SETTINGS*)0x0051BCC0)
#define g_ErrorMessage (*(char(*)[128])0x0051BD20)
#define g_IsTitleLoaded (*(BOOL*)0x0051BDA0)
#define g_MasterVolume (*(int32_t*)0x0051BDA8)
#define g_MciDeviceID (*(MCIDEVICEID*)0x0051BDAC)
#define g_CD_LoopTrack (*(int32_t*)0x0051BDB0)
#define g_TextureFormat (*(TEXTURE_FORMAT*)0x0051C1B8)
#define g_TexturesAlphaChannel (*(bool*)0x0051C20C)
#define g_TextstringTable (*(TEXTSTRING(*)[64])0x0051C820)
#define g_TextstringCount (*(int16_t*)0x0051D6A0) // = 0
#define g_TextstringBuffers (*(char(*)[64][64])0x0051D6C0)
#define g_SoundIsActive (*(int32_t*)0x0051E6C4)
#define g_SampleLUT (*(int16_t(*)[])0x0051E6E0)
#define g_SampleInfos (*(SAMPLE_INFO **)0x0051E9C4)
#define g_SaveGame (*(SAVEGAME_INFO*)0x0051E9E0)
#define g_BaddieSlots (*(CREATURE_INFO **)0x005206C0)
#define g_Lara (*(LARA_INFO*)0x005206E0)
#define g_LaraItem (*(ITEM_INFO **)0x005207BC)
#define g_Effects (*(FX_INFO **)0x005207C0)
#define g_NextEffectFree (*(int16_t*)0x005207C4)
#define g_NextItemFree (*(int16_t*)0x005207C6)
#define g_NextItemActive (*(int16_t*)0x005207C8)
#define g_NextEffectActive (*(int16_t*)0x005207CA)
#define g_PrevItemActive (*(int16_t*)0x005207CC)
#define g_Pickups (*(PICKUP_INFO(*)[12])0x00521CA0)
#define g_GF_CutsceneFileNames (*(char ***)0x00521DC4)
#define g_GameFlow (*(GAME_FLOW*)0x00521DE0)
#define g_GF_TitleFileNames (*(char ***)0x00521F48)
#define g_GF_LevelFileNames (*(char ***)0x00521F54)
#define g_SoundEffectCount (*(int32_t*)0x00521FDC)
#define g_SoundEffects (*(OBJECT_VECTOR **)0x00521FE0)
#define g_Objects (*(OBJECT_INFO(*)[265])0x00522000)
#define g_Meshes (*(int16_t ***)0x005252B0)
#define g_Outside (*(int32_t*)0x005252B4)
#define g_DrawRoomsCount (*(int32_t*)0x005252B8)
#define g_IMMatrixStack (*(MATRIX(*)[256])0x005252C0)
#define g_DoorVBuf (*(DOOR_VBUF(*)[4])0x005258C0)
#define g_IMFrac (*(int32_t*)0x005258F0)
#define g_Anims (*(ANIM_STRUCT **)0x005258F4)
#define g_BoundRooms (*(int32_t(*)[MAX_BOUND_ROOMS])0x00525900)
#define g_OutsideBottom (*(int32_t*)0x00525B00)
#define g_AnimRanges (*(ANIM_RANGE **)0x00525B04)
#define g_AnimCommands (*(int16_t **)0x00525B08)
#define g_DrawRoomsArray (*(int16_t(*)[100])0x00525B20)
#define g_Bones (*(int32_t **)0x00525BE8)
#define g_DynamicLightCount (*(int32_t*)0x00525BEC)
#define g_StaticObjects (*(STATIC_INFO(*)[MAX_STATIC_OBJECTS])0x00525C00)
#define g_OutsideLeft (*(int32_t*)0x00526178)
#define g_AnimChanges (*(ANIM_CHANGE **)0x0052617C)
#define g_RoomCount (*(int32_t*)0x00526180)
#define g_IMRate (*(int32_t*)0x00526184)
#define g_IMMatrixPtr (*(MATRIX **)0x00526188)
#define g_Rooms (*(ROOM_INFO **)0x0052618C)
#define g_CameraUnderwater (*(BOOL*)0x00526190)
#define g_OutsideRight (*(int32_t*)0x00526198)
#define g_OutsideTop (*(int32_t*)0x005261AC)
#define g_FlipStatus (*(int32_t*)0x00526240)
#define g_FlipMaps (*(int32_t(*)[MAX_FLIPMAPS])0x00526260)
#define g_TriggerIndex (*(int16_t **)0x00526288)
#define g_LOSRooms (*(int32_t(*)[20])0x005262A0)
#define g_Items (*(ITEM_INFO **)0x005262F0)
#define g_NumCineFrames (*(int16_t*)0x005262F6)
#define g_CineData (*(CINE_FRAME **)0x005262F8) // = NULL
#define g_CinePos (*(PHD_3DPOS*)0x00526300)
#define g_CineLevelID (*(int16_t*)0x00526312)
#define g_CineFrameIdx (*(int16_t*)0x00526314)
#define g_Camera (*(CAMERA_INFO*)0x00526320)
#define g_GroundZone (*(int16_t *(*)[][2])0x005263A0)
#define g_FlyZone (*(int16_t *(*)[2])0x005263C0)
#define g_Overlap (*(uint16_t **)0x005263C8)
#define g_Boxes (*(BOX_INFO **)0x005263CC)
#define g_BoxCount (*(int32_t*)0x005263D0)

// clang-format on
