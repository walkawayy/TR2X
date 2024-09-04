#pragma once

#include "global/types.h"

#include <libtrx/enum_str.h>

#include <stdint.h>

extern ENUM_STRING_MAP ENUM_STRING_MAP(GAME_OBJECT_ID)[];

void Object_SetupNames(void);

const char *Object_GetName(GAME_OBJECT_ID obj_id);
void Object_SetName(GAME_OBJECT_ID obj_id, const char *name);

// Return a list of object ids that match given string.
// out_match_count may be NULL.
// The result must be freed by the caller.
GAME_OBJECT_ID *Object_IdsFromName(const char *name, int32_t *out_match_count);
