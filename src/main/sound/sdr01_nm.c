/* sdr01_nm - SLPM_654.95 0x00214390-0x002145C8: SdrSeReq / SdrSeChg, queue a sound effect request / change in the 32-entry sound
 * driver request ring sndque_tbl (see sdr07.c for the other writers). The top byte of the command word holds the sign flags of
 * volume, pan and pitch (Chg also sets bit 3 and refuses an entry with only that bit). Near-match (not built): only the order in
 * which the flag bits are computed differs. */
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

int SdrSeReq(int snd, s8 vol, s8 pan, s16 pitch, s8 x)
{
    int idx = sque_w_idx[0];

    if (sndque_tbl[idx].cmd >= 0) {
        return -1;
    }
    {
    s8 f = ((vol >= 0) & 1) | (((pan >= 0) & 1) << 1) | ((pitch >= 0) << 2);
    sndque_tbl[idx].cmd = (f << 24) | (snd & 0xFFFFFF);
    }
    sndque_tbl[idx].vol = vol;
    sndque_tbl[idx].pan = pan;
    sndque_tbl[idx].pitch = pitch;
    sndque_tbl[idx].x08 = x;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}

int SdrSeChg(int snd, s8 vol, s8 pan, s16 pitch, s8 x)
{
    int idx = sque_w_idx[0];
    s8 f;

    if (sndque_tbl[idx].cmd >= 0) {
        return -1;
    }
    f = ((vol >= 0) & 1) | 8 | (((pan >= 0) & 1) << 1) | ((pitch >= 0) << 2);
    if (f == 8) {
        return 1;
    }
    sndque_tbl[idx].cmd = (f << 24) | (snd & 0xFFFFFF);
    sndque_tbl[idx].vol = vol;
    sndque_tbl[idx].pan = pan;
    sndque_tbl[idx].pitch = pitch;
    sndque_tbl[idx].x08 = x;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}
