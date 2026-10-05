#ifndef REWARD_H
#define REWARD_H
/* Quest reward screen (f_reward*.c): shared work struct and externs. */
#include "f_game.h"
#include "plf.h"

/* reward screen work (0x3?????): fields by offset, meanings are guesses */
typedef struct REWARD_W {
    u8 x0;              /* 0x00 mode: 0 list, 1 item pick, 2 done */
    u8 x1;              /* 0x01 frame list index */
    u8 x2;              /* 0x02 sub step */
    u8 x3;              /* 0x03 */
    u8 x4;              /* 0x04 cursor (0-15) */
    u8 x5;              /* 0x05 second cursor (0-19) */
    s16 x6;             /* 0x06 time left */
    s16 x8;             /* 0x08 repeating keys */
    s8 xA;              /* 0x0A key repeat counter */
    s8 xB;              /* 0x0B */
    u8 xC;              /* 0x0C */
    u8 _padD[3];
} REWARD_W;
extern REWARD_W reward_w;
extern struct { u8 _pad00[0x10]; u8 x10; u8 x11; s16 x12; } PitMenu;
u16 reward_key_repeat();
extern u16 Psw[];
void se_req();
#endif
