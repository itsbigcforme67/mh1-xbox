/* sdr03 - SLPM_654.95 0x00214860-0x002148D8: SdrAllStop, queues the "stop everything" command (0x40000000) in the sound driver
 * request ring (see sdr01_nm.c for the other queue writers). */
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

int SdrAllStop(void)
{
    int idx = sque_w_idx[0];

    if (sndque_tbl[idx].cmd >= 0) {
        return -1;
    }
    sndque_tbl[idx].cmd = 0x40000000;
    sque_w_idx[0] = (sque_w_idx[0] + 1) % 32;
    return 0;
}
