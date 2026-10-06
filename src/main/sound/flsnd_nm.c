/* flsnd01 - 0x002160D0-0x002162B8: flSndPackLoadSub2(pack, dst, slot): load the sound pack of one SPU bank
 * (slot < 16). Builds the 8-byte-per-entry table tsb2[slot] (0x80 entries, from 16-byte records of the pack),
 * merges the pack's HD headers into dst with HdMerge, errors out when the merged size is 0xBFFE1 or more,
 * then starts the DMA with SdrDmaLoadReq. Returns -1 for a bad slot, the DMA error or 0. */
#include "types.h"

extern u8 tsb2[];
extern int hdpack[];
extern char lit_536_00367298[];

void *memset(void *, int, int);
int HdMerge(int, int *, void *);
int SdrDmaLoadReq(int, void *, int, int, int, u8 *, int);
void FlushCache(int);
void SdrSendReq(int);
void system_error(char *, int, int, int);

typedef struct SNDPK {
    u8 _pad00[0x18];
    int x18;
    int x1C;
    int x20;
    int x24;
} SNDPK;

int flSndPackLoadSub2(u8 *pk, void *dst, u32 slot) {
    SNDPK *h = (SNDPK *)pk;
    u8 *out0;
    u8 *out;
    s8 *src;
    int i;
    int r;

    if (slot >= 0x10) {
        return -1;
    }
    out0 = tsb2 + (slot << 10);
    out = out0;
    src = (s8 *)(pk + h->x20 + 0x10);
    memset(out0, 0, 0x400);
    for (i = 0; i < 0x80 && (u32)i < ((u32) * (int *)(pk + 0x24) - 0x10) >> 4; i++) {
        out[0] = src[0];
        out[1] = src[9];
        out[2] = src[6] & 0x7F;
        out[3] = (src[6] & 0x80) != 0;
        out[6] = src[8];
        out[4] = src[0xC];
        out[5] = src[0xD];
        out[7] = src[0xF];
        src += 0x10;
        out += 8;
    }
    r = HdMerge(hdpack[0], &hdpack[1], dst);
    if (hdpack[6] >= 0xBFFE1) {
        system_error(lit_536_00367298, (s16)r, (s16)hdpack[6], *(s16 *)&h->x1C);
    }
    r = SdrDmaLoadReq(slot, dst, r, hdpack[8], hdpack[6], pk + h->x18, h->x1C);
    if (r < 0) {
        memset(out0, 0, 0x400);
        return r;
    }
    FlushCache(0);
    SdrSendReq(1);
    return 0;
}
