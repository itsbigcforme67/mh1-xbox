#ifndef GAME_H
#define GAME_H
/* game_w (0x3F33F0, 0x224 bytes): global game/session state.
 * Offsets from matched code: master (Pl_master_ck, pl_sw_set). */
#include "types.h"

typedef struct GAME_W {
    u8 _pad000[0xD1];
    u8 master;          /* 0x0D1 player number of the session master */
    u8 _pad0D2[0x224 - 0xD2];
} GAME_W;

extern GAME_W game_w;

#endif
