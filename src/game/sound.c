#include "game/sound.h"

#include "game/math.h"
#include "game/random.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_audio_sample.h"

#include <libtrx/log.h>
#include <libtrx/utils.h>

typedef enum {
    SOUND_MODE_NORMAL = 0,
    SOUND_MODE_WAIT = 1,
    SOUND_MODE_RESTART = 2,
    SOUND_MODE_LOOPED = 3,
    SOUND_MODE_MASK = 3,
} SOUND_MODE;

typedef enum {
    // clang-format off
    SF_NO_PAN        = 1 << 12, // = 0x1000 = 4096
    SF_PITCH_WIBBLE  = 1 << 13, // = 0x2000 = 8192
    SF_VOLUME_WIBBLE = 1 << 14, // = 0x4000 = 16384
    // clang-format on
} SAMPLE_FLAG;

#define SOUND_DEFAULT_PITCH 0x10000
#define SOUND_MAX_SLOTS 32
#define SOUND_MAX_VOLUME_CHANGE 0x2000
#define SOUND_MAX_PITCH_CHANGE 6000

#define SOUND_RANGE 10
#define SOUND_RADIUS (SOUND_RANGE * WALL_L) // = 0x2800 = 10240
#define SOUND_RADIUS_SQRD SQUARE(SOUND_RADIUS) // = 0x6400000

#define SOUND_MAXVOL_RANGE 1
#define SOUND_MAXVOL_RADIUS (SOUND_MAXVOL_RANGE * WALL_L) // = 0x400 = 1024
#define SOUND_MAXVOL_RADIUS_SQRD SQUARE(SOUND_MAXVOL_RADIUS) // = 0x100000

void __cdecl Sound_Init(void)
{
    Sound_SetMasterVolume(32);

    for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
        SOUND_SLOT *const slot = &g_SoundSlots[i];
        slot->sample_num = -1;
    }

    g_SoundIsActive = true;
}

void __cdecl Sound_Shutdown(void)
{
    if (!g_SoundIsActive) {
        return;
    }

    S_Audio_Sample_OutCloseAllTracks();
    for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
        SOUND_SLOT *const slot = &g_SoundSlots[i];
        slot->sample_num = -1;
    }
}

void __cdecl Sound_SetMasterVolume(int32_t volume)
{
    g_MasterVolume = volume;
}

void __cdecl Sound_UpdateEffects(void)
{
    for (int32_t i = 0; i < g_SoundEffectCount; i++) {
        OBJECT_VECTOR *sound = &g_SoundEffects[i];
        if ((g_FlipStatus && (sound->flags & SF_FLIP))
            || (!g_FlipStatus && (sound->flags & SF_UNFLIP))) {
            Sound_Effect(
                sound->data, (XYZ_32 *)sound,
                0); // TODO: use proper pointer for this
        }
    }

    if (g_FlipEffect != -1) {
        g_EffectRoutines[g_FlipEffect](NULL);
    }

    Sound_EndScene();
}

void __cdecl Sound_Effect(
    const int32_t sample_id, const XYZ_32 *const pos, const uint32_t flags)
{
    if (!g_SoundIsActive) {
        return;
    }

    if (flags != SPM_ALWAYS
        && ((flags & SPM_UNDERWATER)
            != (g_Rooms[g_Camera.pos.room_num].flags & RF_UNDERWATER))) {
        return;
    }

    const int32_t sample_num = g_SampleLUT[sample_id];
    if (sample_num == -1) {
        g_SampleLUT[sample_id] = -2;
        return;
    }
    if (sample_num == -2) {
        return;
    }

    SAMPLE_INFO *const s = &g_SampleInfos[sample_num];
    if (s->randomness && (Random_GetDraw() > s->randomness)) {
        return;
    }

    uint32_t distance = 0;
    int32_t pan = 0;
    if (pos) {
        const int32_t dx = pos->x - g_Camera.mic_pos.x;
        const int32_t dy = pos->y - g_Camera.mic_pos.y;
        const int32_t dz = pos->z - g_Camera.mic_pos.z;
        if (ABS(dx) > SOUND_RADIUS || ABS(dy) > SOUND_RADIUS
            || ABS(dz) > SOUND_RADIUS) {
            return;
        }
        distance = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
        if (distance > SOUND_RADIUS_SQRD) {
            return;
        } else if (distance < SOUND_MAXVOL_RADIUS_SQRD) {
            distance = 0;
        } else {
            distance = Math_Sqrt(distance) - SOUND_MAXVOL_RADIUS;
        }
        if (!(s->flags & SF_NO_PAN)) {
            pan = (int16_t)Math_Atan(dz, dx) - g_Camera.actual_angle;
        }
    }

    int32_t volume = s->volume;
    if (s->flags & SF_VOLUME_WIBBLE) {
        volume -= Random_GetDraw() * SOUND_MAX_VOLUME_CHANGE >> 15;
    }
    const int32_t attenuation =
        SQUARE(distance) / (SOUND_RADIUS_SQRD / 0x10000);
    volume = (volume * (0x10000 - attenuation)) / 0x10000;

    if (volume <= 0) {
        return;
    }

    int32_t pitch = (flags & SPM_PITCH) != 0 ? (flags >> 8) & 0xFFFFFF
                                             : SOUND_DEFAULT_PITCH;
    if (s->flags & SF_PITCH_WIBBLE) {
        pitch = pitch + ((Random_GetDraw() * SOUND_MAX_PITCH_CHANGE) >> 14)
            - SOUND_MAX_PITCH_CHANGE;
    }

    if (s->number < 0) {
        return;
    }

    const int32_t num_samples = (s->flags >> 2) & 0xF;
    const int32_t track_id = num_samples == 1
        ? s->number
        : s->number + (int32_t)((num_samples * Random_GetDraw()) / 0x8000);

    switch (s->flags & SOUND_MODE_MASK) {
    case SOUND_MODE_NORMAL:
        break;

    case SOUND_MODE_WAIT:
        for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
            SOUND_SLOT *slot = &g_SoundSlots[i];
            if (slot->sample_num == sample_num) {
                if (S_Audio_Sample_OutIsTrackPlaying(i)) {
                    return;
                }
                slot->sample_num = -1;
            }
        }
        break;

    case SOUND_MODE_RESTART:
        for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
            SOUND_SLOT *slot = &g_SoundSlots[i];
            if (slot->sample_num == sample_num) {
                S_Audio_Sample_OutCloseTrack(i);
                g_SoundSlots[i].sample_num = -1;
                break;
            }
        }
        break;

    case SOUND_MODE_LOOPED:
        for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
            SOUND_SLOT *slot = &g_SoundSlots[i];
            if (slot->sample_num == sample_num) {
                if (volume > g_SoundSlots[i].volume) {
                    g_SoundSlots[i].volume = volume;
                    g_SoundSlots[i].pan = pan;
                    g_SoundSlots[i].pitch = pitch;
                }
                return;
            }
        }
        break;
    }

    int32_t handle = (s->flags & SOUND_MODE_MASK) == SOUND_MODE_LOOPED
        ? S_Audio_Sample_OutPlayLooped(track_id, volume, pitch, pan)
        : S_Audio_Sample_OutPlay(track_id, volume, pitch, pan);

    if (handle == -1) {
        int32_t min_volume = 0x8000;
        int32_t min_slot = -1;
        for (int32_t i = 1; i < SOUND_MAX_SLOTS; i++) {
            SOUND_SLOT *slot = &g_SoundSlots[i];
            if (slot->sample_num >= 0 && slot->volume < min_volume) {
                min_volume = slot->volume;
                min_slot = i;
            }
        }

        if (min_slot >= 0 && volume >= min_volume) {
            S_Audio_Sample_OutCloseTrack(min_slot);
            g_SoundSlots[min_slot].sample_num = -1;
            handle = (s->flags & SOUND_MODE_MASK) == SOUND_MODE_LOOPED
                ? S_Audio_Sample_OutPlayLooped(track_id, volume, pitch, pan)
                : S_Audio_Sample_OutPlay(track_id, volume, pitch, pan);
        }
    }

    if (handle < 0) {
        s->number = -1;
    } else {
        g_SoundSlots[handle].volume = volume;
        g_SoundSlots[handle].pan = pan;
        g_SoundSlots[handle].pitch = pitch;
        g_SoundSlots[handle].sample_num = sample_num;
    }
}

void __cdecl Sound_StopEffect(const int32_t sample_id)
{
    if (!g_SoundIsActive) {
        return;
    }

    const int32_t sample_num = g_SampleLUT[sample_id];
    const int32_t num_samples = (g_SampleInfos[sample_num].flags >> 2) & 0xF;

    for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
        SOUND_SLOT *slot = &g_SoundSlots[i];
        if (slot->sample_num >= sample_num
            && slot->sample_num < sample_num + num_samples) {
            S_Audio_Sample_OutCloseTrack(i);
            slot->sample_num = -1;
        }
    }
}

void __cdecl Sound_EndScene(void)
{
    if (!g_SoundIsActive) {
        return;
    }

    for (int32_t i = 0; i < SOUND_MAX_SLOTS; i++) {
        SOUND_SLOT *const slot = &g_SoundSlots[i];
        SAMPLE_INFO *const s = &g_SampleInfos[slot->sample_num];
        if (slot->sample_num < 0) {
            continue;
        }

        if ((s->flags & SOUND_MODE_MASK) == SOUND_MODE_LOOPED) {
            if (slot->volume == 0) {
                S_Audio_Sample_OutCloseTrack(i);
                slot->sample_num = -1;
            } else {
                S_Audio_Sample_OutSetPanAndVolume(i, slot->pan, slot->volume);
                S_Audio_Sample_OutSetPitch(i, slot->pitch);
                slot->volume = 0;
            }
        } else if (!S_Audio_Sample_OutIsTrackPlaying(i)) {
            slot->sample_num = -1;
        }
    }
}
