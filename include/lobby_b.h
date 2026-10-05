/* Lobby client/UI overlay views (agent B): typed overlays for the byte-offset globals of the lobby
   (lb_sys, pNet, ...). Casting the address of the symbol to a struct gives the original's `symbol+offset`
   addressing (lui/lw %lo(lb_sys + 0x6)), and MWCC allocates registers like the original, which it does not
   for F(T, p, off) macros. Add only. */
#ifndef LOBBY_B_H
#define LOBBY_B_H
#define lb_sys lb_sys_a_unused
#include "lobby_a.h"
#undef lb_sys

typedef struct LBSYS_B {          /* lb_sys, 0x90 bytes (same object as LBSYS) */
    u8 _p00; s8 x01; s8 x02; s8 x03; s8 x04; s8 x05; s8 x06; s8 x07; s8 x08; u8 _p09; s8 x0A;
    u8 _p0B[0x28 - 0xB]; s8 x28;
    u8 _p29[0x64 - 0x29]; u16 x64; u16 x66; s32 x68; s32 x6C;
    u8 x70; s8 x71; s8 x72; u8 x73; s16 x74; u16 x76; u8 x78; u8 _p79[3]; s32 x7C; s32 x80; s8 x84; u8 _p85;
    s8 x86; u8 x87; u8 _p88[5]; u8 x8D; u8 x8E; u8 _p8F;
} LBSYS_B;
extern LBSYS_B lb_sys;
#define LBS (&lb_sys)
#endif
