/* flsnd04 - SLPM_654.95 0x00215AA0-0x00216004: flSndRequest, flSndChange and flSndStatGet. Both walk a sound-effect chain in tsb2
 * (1 KB per bank = 128 entries of 8 bytes: [0] 0xFF ends the list, [1] pan override (0xFF none), [2] sound number
 * (7 bits), [3] bit 0 flag, [4] volume jitter, [5] pitch jitter, [6] base volume, [7] next entry (0xFF none)),
 * apply random jitter to volume and pitch, and hand each entry to the sound driver (SdrSeReq / SdrSeChg).
 * Parameter names are guesses. */
#include "types.h"

extern u8 tsb2[];
u16 ran_suu(int);
int SdrSeReq(int, s8, s8, s16, s8);
int SdrSeChg(int, s8, s8, s16, s8);

int flSndRequest(int bank, u32 no, int vol, int pan, int pitch, int x)
{
    u8 *e = tsb2 + (bank << 10) + no * 8;
    int r;
    int s;
    int v;

    for (;;) {
        if (no >= 0x80) {
            return -1;
        }
        if (e[0] == 0xFF) {
            return 0;
        }
        if (e[1] != 0xFF) {
            pan = e[1];
        }
        if (e[5] != 0) {
            s = ran_suu(1) & 1 ? 1 : -1;
            pitch = (pitch + ((s * (ran_suu(1) % e[5])) << 5)) & 0x3FFF;
        }
        s = ran_suu(1) & 1 ? 1 : -1;
        if (e[4] != 0) {
            v = e[6] + s * (ran_suu(1) % e[4]);
        } else {
            v = e[6];
        }
        if (v < 0) {
            v = 0;
        } else if (v > 0x7F) {
            v = 0x7F;
        }
        vol = (u32)((f32)v * ((f32)vol / 128.0f));
        r = SdrSeReq((((bank & 0x7F) << 16) | ((no & 0x7F) << 8)) | (((e[3] & 1) << 7) | (e[2] & 0x7F)), vol, pan, pitch, x);
        if (r < 0) {
            return r;
        }
        no = e[7];
        if (no != 0xFF) {
            e = tsb2 + (bank << 10) + no * 8;
            continue;
        }
        return 0;
    }
}

int flSndChange(int bank, u32 no, int vol, int pan, int pitch, int x)
{
    u8 *e = tsb2 + (bank << 10) + no * 8;
    int r;
    int s;
    int v;

    for (;;) {
        if (no >= 0x80) {
            return -1;
        }
        if (e[0] == 0xFF) {
            return 0;
        }
        if (e[1] != 0xFF) {
            pan = e[1];
        }
        if (e[5] != 0) {
            s = ran_suu(1) & 1 ? 1 : -1;
            pitch = (pitch + ((s * (ran_suu(1) % e[5])) << 5)) & 0x3FFF;
        }
        s = ran_suu(1) & 1 ? 1 : -1;
        if (e[4] != 0) {
            v = e[6] + s * (ran_suu(1) % e[4]);
        } else {
            v = e[6];
        }
        if (v < 0) {
            v = 0;
        } else if (v > 0x7F) {
            v = 0x7F;
        }
        vol = (u32)((f32)v * ((f32)vol / 128.0f));
        r = SdrSeChg((e[2] & 0x7F) | (((bank & 0x7F) << 16) | ((no & 0x7F) << 8)), vol, pan, pitch, x);
        if (r < 0) {
            return r;
        }
        no = e[7];
        if (no != 0xFF) {
            e = tsb2 + (bank << 10) + no * 8;
            continue;
        }
        return 0;
    }
}

int SdrGetState(int, int);

/* sound number of table entry (bank, no) packed with the bank and entry into one status query */
int flSndStatGet(int bank, int no)
{
    u32 v = *(tsb2 + 2 + (bank << 10) + no * 8);

    if (v == 0xFF) {
        v = 0;
    }
    return SdrGetState(4, (((bank & 0x7F) << 16) | ((no & 0x7F) << 8)) | (v & 0x7F));
}
