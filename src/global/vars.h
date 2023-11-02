// This file is autogenerated. To update it, run tools/generate_funcs.

#pragma once

#include "global/types.h"
#include "inject_util.h"

// clang-format off
#define g_PerspectiveDistance (*(int32_t*)0x00464060)
#define g_PolyDrawRoutines (*((void(__cdecl  *(*)[9])(const int16_t *))0x00464068))
#define g_RhwFactor (*(float*)0x0046408C)
#define g_CD_TrackID (*(int16_t*)0x004640BC)
#define g_FlipEffect (*(int32_t*)0x004640C4)
#define g_EffectRoutines (*((void(__cdecl  *(*)[32])(struct ITEM_INFO *item))0x004641F8))
#define g_OptionMusicVolume (*(int16_t*)0x00465A60)
#define g_JumpPermitted (*(int32_t*)0x00465AD4) // = 1
#define g_LaraOldSlideAngle (*(int16_t*)0x00465AD8) // = 1
#define g_MidSort (*(int32_t*)0x0046C300) // = 0
#define g_ViewportAspectRatio (*(float*)0x0046C304) // = 0.0f
#define g_XGenY1 (*(int32_t*)0x0046C308)
#define g_XGenY2 (*(int32_t*)0x0046C30C)
#define g_GouraudTable (*(struct GOURAUD_ENTRY(*)[256])0x0046C310)
#define g_PhdWinTop (*(int32_t*)0x0046E310)
#define g_LsAdder (*(int32_t*)0x00470318)
#define g_FltWinBottom (*(float*)0x0047031C)
#define g_FltResZBuf (*(float*)0x00470320)
#define g_FltResZ (*(float*)0x00470324)
#define g_Output_InsertTransQuad (*(void(__cdecl **)(int32_t x, int32_t y, int32_t width, int32_t height, int32_t z))0x00470328)
#define g_PhdWinHeight (*(int32_t*)0x0047032C)
#define g_PhdWinCenterX (*(int32_t*)0x00470330)
#define g_PhdWinCenterY (*(int32_t*)0x00470334)
#define g_LsYaw (*(int16_t*)0x00470338)
#define g_Output_InsertTrans8 (*(void(__cdecl **)(struct PHD_VBUF *vbuf, int16_t shade))0x0047033C)
#define g_FltWinTop (*(float*)0x00470340)
#define g_SortBuffer (*(struct SORT_ITEM(*)[4000])0x00470348)
#define g_FltWinLeft (*(float*)0x00478048)
#define g_PhdWinMinY (*(int16_t*)0x0047804C)
#define g_PhdFarZ (*(int32_t*)0x00478058)
#define g_FltRhwOPersp (*(float*)0x0047805C)
#define g_PhdWinBottom (*(int32_t*)0x00478060)
#define g_PhdPersp (*(int32_t*)0x00478064)
#define g_PhdWinLeft (*(int32_t*)0x00478068)
#define g_Output_InsertFlatRect (*(void(__cdecl **)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t z, uint8_t color_idx))0x0047806C)
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
#define g_Output_InsertSprite (*(void(__cdecl **)(int32_t z, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t sprite_idx, int16_t shade))0x004B2A20)
#define g_FltNearZ (*(float*)0x004B2A24)
#define g_MatrixPtr (*(struct MATRIX **)0x004B2A28)
#define g_Output_DrawObjectGT3 (*(const int16_t *(__cdecl **)(const int16_t*, int32_t, enum SORT_TYPE))0x004B2A2C)
#define g_Output_DrawObjectGT4 (*(const int16_t *(__cdecl **)(const int16_t*, int32_t, enum SORT_TYPE))0x004B2A30)
#define g_RandomTable (*(int32_t(*)[32])0x004B2A38)
#define g_FltPersp (*(float*)0x004B2AB8)
#define g_W2VMatrix (*(struct MATRIX*)0x004B2AC0)
#define g_Info3DPtr (*(int16_t **)0x004B2AF0)
#define g_PhdWinWidth (*(int32_t*)0x004B2AF4)
#define g_Output_DrawLine (*(void(__cdecl **)(int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t))0x004B2AF8)
#define g_PhdTextureInfo (*(struct PHD_TEXTURE(*)[0x800])0x004B2B00)
#define g_PhdViewDistance (*(int32_t*)0x004BCB00)
#define g_LsPitch (*(int16_t*)0x004BCB04)
#define g_Output_DrawObjectG4 (*(const int16_t *(__cdecl **)(const int16_t*,int32_t, enum SORT_TYPE))0x004BCB08)
#define g_ShadesTable (*(int16_t(*)[32])0x004BCB10)
#define g_Output_DrawObjectG3 (*(const int16_t *(__cdecl **)(const int16_t*,int32_t, enum SORT_TYPE))0x004BCB50)
#define g_MatrixStack (*(struct MATRIX(*)[])0x004BCB58)
#define g_DepthQTable (*(struct DEPTHQ_ENTRY(*)[32])0x004BD2D8)
#define g_PhdScreenWidth (*(int32_t*)0x004BF3D8)
#define g_LsDivider (*(int32_t*)0x004BF3DC)
#define g_PhdVBuf (*(struct PHD_VBUF(*)[1500])0x004BF3E0)
#define g_XBuffer ((void*)0x004CAF60)
#define g_TexturePageBuffer8 (*(uint8_t *(*)[32])0x004D6AE0)
#define g_FltWinRight (*(float*)0x004D6B60)
#define g_LsVectorView (*(struct PHD_VECTOR*)0x004D6B68)
#define g_WibbleTable (*(float(*)[32])0x004D6B78)
#define g_PhdWinRight (*(int32_t*)0x004D6BF8)
#define g_SurfaceCount (*(int32_t*)0x004D6BFC)
#define g_Sort3DPtr (*(struct SORT_ITEM **)0x004D6C00)
#define g_WibbleOffset (*(int32_t*)0x004D6C0C)
#define g_IsWibbleEffect (*(int32_t*)0x004D6C10)
#define g_IsWaterEffect (*(int32_t*)0x004D6C14)
#define g_VBuffer (*(struct VERTEX_INFO(*)[20])0x004D6CD8)
#define g_GamePalette16 (*(PALETTEENTRY(*)[256])0x004D7380)
#define g_CineFrameCurrent (*(int32_t*)0x004D7780)
#define g_IsChunkyCamera (*(int32_t*)0x004D778C)
#define g_SoundTrackIds (*(int32_t(*)[128])0x004D7C80)
#define g_GameWindowHandle (*(HWND*)0x004D7F10)
#define g_IsGameToExit (*(uint8_t*)0x004D8378)
#define g_SampleFreqs (*(DWORD(*)[256])0x004D8570)
#define g_SoundAdapterList (*(struct SOUND_ADAPTER_LIST*)0x004D8970)
#define g_SampleBuffers (*(LPDIRECTSOUNDBUFFER(*)[256])0x004D8980)
#define g_IsSoundEnabled (*(uint8_t*)0x004D8D80)
#define g_DSound (*(LPDIRECTSOUND*)0x004D8D84)
#define g_ChannelSamples (*(int32_t(*)[32])0x004D8D88)
#define g_ChannelBuffers (*(LPDIRECTSOUNDBUFFER(*)[32])0x004D8E08)
#define g_CurrentSoundAdapter (*(struct SOUND_ADAPTER*)0x004D8E8C)
#define g_PrimarySoundAdapter (*(struct SOUND_ADAPTER_NODE **)0x004D8EAC)
#define g_RenderBufferSurface (*(LPDDS*)0x004D8EB0)
#define g_ThirdBufferSurface (*(LPDDS*)0x004D92B8)
#define g_PictureBufferSurface (*(LPDDS*)0x004D92BC)
#define g_ZBufferSurface (*(LPDDS*)0x004D92C0)
#define g_PrimaryBufferSurface (*(LPDDS*)0x004D92C8)
#define g_LevelComplete (*(int32_t*)0x004D9EC4)
#define g_HWR_VertexBuffer (*(D3DTLVERTEX(*)[0x2000])0x004D9ED8)
#define g_HWR_VertexPtr (*(D3DTLVERTEX **)0x00519F60)
#define g_Input (*(int32_t*)0x0051A208)
#define g_PhdWinRect (*(RECT*)0x0051B918)
#define g_SavedAppSettings (*(struct APP_SETTINGS*)0x0051BCC0)
#define g_ErrorMessage (*(char(*)[128])0x0051BD20)
#define g_MasterVolume (*(int32_t*)0x0051BDA8)
#define g_MciDeviceID (*(MCIDEVICEID*)0x0051BDAC)
#define g_CD_LoopTrack (*(int32_t*)0x0051BDB0)
#define g_SoundIsActive (*(int32_t*)0x0051E6C4)
#define g_Lara (*(struct LARA_INFO*)0x005206E0)
#define g_LaraItem (*(struct ITEM_INFO **)0x005207BC)
#define g_SoundFxCount (*(int32_t*)0x00521FDC)
#define g_Objects (*(struct OBJECT_INFO(*)[265])0x00522000)
#define g_Meshes (*(int16_t ***)0x005252B0)
#define g_IMMatrixStack (*(struct MATRIX(*)[256])0x005252C0)
#define g_IMFrac (*(int32_t*)0x005258F0)
#define g_Anims (*(struct ANIM_STRUCT **)0x005258F4)
#define g_Bones (*(int32_t **)0x00525BE8)
#define g_IMRate (*(int32_t*)0x00526184)
#define g_IMMatrixPtr (*(struct MATRIX **)0x00526188)
#define g_Rooms (*(struct ROOM_INFO **)0x0052618C)
#define g_FlipStatus (*(int32_t*)0x00526240)
#define g_TriggerIndex (*(int16_t **)0x00526288)
#define g_Items (*(struct ITEM_INFO(*)[])0x005262F0)
#define g_NumCineFrames (*(int16_t*)0x005262F6)
#define g_CineData (*(CINE_FRAME **)0x005262F8) // = NULL
#define g_CinePos (*(struct PHD_3DPOS*)0x00526300)
#define g_CineFrameIdx (*(int16_t*)0x00526314)
#define g_Camera (*(struct CAMERA_INFO*)0x00526320)
#define g_Boxes (*(struct BOX_INFO **)0x005263CC)
// clang-format on
