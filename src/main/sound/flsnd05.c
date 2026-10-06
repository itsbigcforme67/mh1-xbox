/* flsnd05 - SLPM_654.95 0x00215730-0x00215A9C: sound module start-up and sound pack loading on top of the Sdr* driver
 * calls: flSndModuleInit (loads the four IOP modules), flSndInitialize, flSndControll (per-frame driver update),
 * flSndPackLoadSub (copies the pack's sound table into tsb2[bank] and queues the sample data for the SPU),
 * flSndPackLoad (blocking), flSndPackLoadBG (background), flSndPackLoadStatus.
 * Pack header (guess from the loads): +0x08 sample data offset, +0x0C size, +0x10 offset of the second part, +0x20 table
 * offset, +0x24 table end; table entries are 0x10 bytes. */
#include "types.h"

extern char lit_190_003671F0[];
extern char lit_191_00367210[];
extern char lit_192_00367240[];
extern char lit_193_00367270[];
extern u8 tsb2[];

int flPS2IopModuleLoad(char *, int, int, int);
void SdrInit();
void SdrGetStateSend(int, int);
void SdrSendReq();
int SdrChkSendReq(int);
int SdrGetState(int, int);
void FlushCache(int);
void sceGsSyncV(int);
void *memset(void *, int, unsigned int);

void flSndModuleInit(void)
{
    flPS2IopModuleLoad(lit_190_003671F0, 0, 0, 0);
    flPS2IopModuleLoad(lit_191_00367210, 0, 0, 0);
    flPS2IopModuleLoad(lit_192_00367240, 0, 0, 0);
    flPS2IopModuleLoad(lit_193_00367270, 0, 0, 0);
}

void flSndInitialize(void)
{
    SdrInit();
    SdrGetStateSend(0x10, 0x800);
    SdrSendReq(0);
    memset(tsb2, 0, 0x800);
}

void flSndControll(void)
{
    SdrSendReq();
}

typedef struct SNDPACK {
    u8 x00[8];
    int x08;
    int x0C;
    int x10;
    int x14;
    int x18;
    int x1C;
    int x20;
    int x24;
} SNDPACK;

int SdrDmaLoadReq(u32, void *, int, void *, int, void *, int);

int flSndPackLoadSub(void *pack, u32 bank)
{
    s8 *p;
    SNDPACK *pk;
    s8 *src;
    s8 *t;
    s8 *base;
    int i;
    int r;

    if (bank >= 0x10) {
        return -1;
    }
    p = pack;
    pk = pack;
    src = p + pk->x20 + 0x10;
    base = (s8 *)tsb2 + (bank << 10);
    t = base;
    memset(base, 0, 0x400);
    i = 0;
    while (i < 0x80 && (u32)i < ((u32)(pk->x24 - 0x10) >> 4)) {
        i++;
        t[0] = src[0];
        t[1] = src[9];
        t[6] = src[8];
        t[4] = src[0xC];
        t[5] = src[0xD];
        t[2] = src[6] & 0x7F;
        t[3] = (src[6] & 0x80) != 0;
        t[7] = src[0xF];
        src += 0x10;
        t += 8;
    }
    r = SdrDmaLoadReq(bank, p + pk->x08, pk->x0C, p + pk->x10, pk->x14, p + pk->x18, pk->x1C);
    if (r < 0) {
        memset(base, 0, 0x400);
        return r;
    }
    FlushCache(0);
    SdrSendReq(1);
    return 0;
}

int flSndPackLoad(SNDPACK *pack, u32 bank)
{
    int r;

    r = flSndPackLoadSub(pack, bank);
    if (r < 0) {
        return r;
    }
    while (SdrChkSendReq(1) > 0) {
        sceGsSyncV(0);
    }
    while (SdrGetState(0x8000000C, 1) > 0) {
        sceGsSyncV(0);
    }
    return 0;
}

int flSndPackLoadBG(SNDPACK *pack, u32 bank)
{
    int r;

    r = flSndPackLoadSub(pack, bank);
    if (r < 0) {
        return r;
    }
    return 0;
}

int flSndPackLoadStatus(void)
{
    int r = SdrChkSendReq(1);
    if (r > 0) {
        return r;
    }
    return SdrGetState(0x8000000C, 1);
}
