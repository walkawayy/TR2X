#pragma once

#include "global/types.h"

#include <stdint.h>
#include <windows.h>

int32_t __cdecl GameInit(void);
int32_t __stdcall WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
    int32_t nShowCmd);
const char *__cdecl DecodeErrorMessage(int32_t error_code);
int32_t __cdecl RenderErrorBox(int32_t error_code);
void __cdecl ScreenshotPCX(void);
size_t __cdecl CompPCX(
    uint8_t *bitmap, int32_t width, int32_t height, RGB_888 *palette,
    uint8_t **pcx_data);
size_t __cdecl EncodeLinePCX(const uint8_t *src, int32_t width, uint8_t *dst);
size_t __cdecl EncodePutPCX(uint8_t value, uint8_t num, uint8_t *buffer);
void __cdecl Screenshot(LPDDS screen);
bool __cdecl DInputCreate(void);
void __cdecl DInputRelease(void);
void __cdecl WinInReadKeyboard(uint8_t *input_data);
int32_t __cdecl WinGameStart(void);
void __cdecl Shell_Shutdown(void);
int16_t __cdecl TitleSequence(void);
void __cdecl WinVidSetMinWindowSize(int32_t width, int32_t height);
void __cdecl WinVidSetMaxWindowSize(int32_t width, int32_t height);
void __cdecl WinVidClearMinWindowSize(void);
void __cdecl WinVidClearMaxWindowSize(void);
int32_t __cdecl CalculateWindowWidth(int32_t width, int32_t height);
int32_t __cdecl CalculateWindowHeight(int32_t width, int32_t height);
bool __cdecl WinVidGetMinMaxInfo(LPMINMAXINFO info);
HWND __cdecl WinVidFindGameWindow(void);
bool __cdecl WinVidSpinMessageLoop(bool need_wait);
void __cdecl WinVidShowGameWindow(int32_t cmd_show);
void __cdecl WinVidHideGameWindow(void);
void __cdecl WinVidSetGameWindowSize(int32_t width, int32_t height);
bool __cdecl ShowDDrawGameWindow(bool active);
bool __cdecl HideDDrawGameWindow(void);
HRESULT __cdecl DDrawSurfaceCreate(LPDDSDESC dsp, LPDDS *surface);
HRESULT __cdecl DDrawSurfaceRestoreLost(
    LPDDS surface1, LPDDS surface2, bool blank);
bool __cdecl WinVidClearBuffer(LPDDS surface, LPRECT rect, DWORD fill_color);
HRESULT __cdecl WinVidBufferLock(LPDDS surface, LPDDSDESC desc, DWORD flags);
HRESULT __cdecl WinVidBufferUnlock(LPDDS surface, LPDDSDESC desc);
bool __cdecl WinVidCopyBitmapToBuffer(LPDDS surface, const BYTE *bitmap);
DWORD __cdecl GetRenderBitDepth(uint32_t rgb_bit_count);
void __thiscall WinVidGetColorBitMasks(
    COLOR_BIT_MASKS *bm, LPDDPIXELFORMAT pixel_format);
void __cdecl BitMaskGetNumberOfBits(
    uint32_t bit_mask, uint32_t *bit_depth, uint32_t *bit_offset);
DWORD __cdecl CalculateCompatibleColor(
    const COLOR_BIT_MASKS *mask, int32_t red, int32_t green, int32_t blue,
    int32_t alpha);
bool __cdecl WinVidGetDisplayMode(DISPLAY_MODE *disp_mode);
bool __cdecl WinVidGoFullScreen(DISPLAY_MODE *disp_mode);
bool __cdecl WinVidGoWindowed(
    int32_t width, int32_t height, DISPLAY_MODE *disp_mode);
void __cdecl WinVidSetDisplayAdapter(DISPLAY_ADAPTER *disp_adapter);
void __cdecl Game_SetCutsceneTrack(int32_t track);
int32_t __cdecl Game_Cutscene_Start(int32_t level_num);
void __cdecl Misc_InitCinematicRooms(void);
int32_t __cdecl Game_Cutscene_Control(int32_t nframes);
int32_t __cdecl Room_FindByPos(int32_t x, int32_t y, int32_t z);
void __cdecl CutscenePlayer_Control(int16_t item_num);
void __cdecl Lara_Control_Cutscene(int16_t item_num);
void __cdecl CutscenePlayer1_Initialise(int16_t item_num);
void __cdecl CutscenePlayerGen_Initialise(int16_t item_num);
int32_t __cdecl Level_Initialise(int32_t level_num, int32_t level_type);
void __cdecl CreateScreenBuffers(void);
void __cdecl CreatePrimarySurface(void);
void __cdecl CreateBackBuffer(void);
void __cdecl CreateClipper(void);
void __cdecl CreateWindowPalette(void);
void __cdecl CreateZBuffer(void);
int32_t __cdecl GetZBufferDepth(void);
void __cdecl CreateRenderBuffer(void);
void __cdecl CreatePictureBuffer(void);
void __cdecl ClearBuffers(DWORD flags, DWORD fill_color);
void __cdecl UpdateFrame(bool need_run_message_loop, LPRECT rect);
void __cdecl RestoreLostBuffers(void);
void __cdecl WaitPrimaryBufferFlip(void);
bool __cdecl RenderInit(void);
void __cdecl RenderStart(bool is_reset);
void __cdecl RenderFinish(bool need_to_clear_textures);
bool __cdecl ApplySettings(const APP_SETTINGS *new_settings);
void __cdecl FmvBackToGame(void);
void __cdecl GameApplySettings(APP_SETTINGS *new_settings);
void __cdecl UpdateGameResolution(void);
void __cdecl D3DRelease(void);
void __cdecl Enumerate3DDevices(DISPLAY_ADAPTER *adapter);
