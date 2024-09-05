#pragma once

#include "global/types.h"

void __cdecl Requester_Init(REQUEST_INFO *req);
void __cdecl Requester_Shutdown(REQUEST_INFO *req);
void __cdecl Requester_Item_CenterAlign(REQUEST_INFO *req, TEXTSTRING *text);
void __cdecl Requester_Item_LeftAlign(REQUEST_INFO *req, TEXTSTRING *text);
