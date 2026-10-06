/* sdr07 - SLPM_654.95 0x002148E0-0x00214AD8: more sound driver request writers (see sdr01_nm.c): SdrPortStop (stop one SPU port),
 * SdrSetOutputMode (mono/stereo), SdrSetRev (reverb mode, depth and delay). The command word is built as
 * (value & 0xFFFFFF) | (opcode << 24); an int & 0xFFFFFF compiles to a dsll32 / dsrl32 pair here. */
#include "types.h"

typedef struct SNDQUE {
    int cmd;        /* 0x00 command word, < 0 when the slot is free */
    s8 vol;         /* 0x04 */
    s8 pan;         /* 0x05 */
    s16 pitch;      /* 0x06 */
    s8 x08;         /* 0x08 */
    u8 pad[3];
} SNDQUE;

extern SNDQUE sndque_tbl[];
extern volatile int sque_w_idx[];   /* unknown size on purpose: lui/addiu access, not gp; volatile: reloaded after the stores */

int SdrPortStop(int port)
{
    int idx = sque_w_idx[0];

    if (sndque_tbl[idx].cmd >= 0) {
        return -1;
    }
    sndque_tbl[idx].cmd = (((port & 0xFF) << 16) & 0xFFFFFF) | 0x4A000000;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}

int SdrSetOutputMode(int stereo)
{
    int idx = sque_w_idx[0];

    if (sndque_tbl[idx].cmd >= 0) {
        return -1;
    }
    sndque_tbl[idx].cmd = (((stereo != 0) << 16) & 0xFFFFFF) | 0x42000000;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}

int SdrSetRev(u32 mode, u32 depth, s16 delay, s8 a, s8 b)
{
    SNDQUE *q;

    if (mode > 2) {
        return -3;
    }
    if (depth >= 10) {
        return -2;
    }
    q = &sndque_tbl[sque_w_idx[0]];
    if (q->cmd >= 0) {
        return -1;
    }
    depth |= (mode + 1) << 6;
    q->cmd = (((depth << 16) | (delay & 0xFFFF)) & 0xFFFFFF) | 0x44000000;
    q->vol = a;
    q->pan = b;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}
