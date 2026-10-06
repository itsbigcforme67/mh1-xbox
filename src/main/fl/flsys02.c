/* fl library system start up: flInitialize, system_work_init, system_hard_init, flFlip, flPS2VramFullClear. SLPM_654.95 0x0018BD90-0x0018C304.
 * (flPS2VSyncCallback, 0x0018C110, uses the `ei` instruction and is kept as original bytes via config/c_rawfuncs.txt.) */
#include "types.h"

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flFMS[];
extern int flPs2VIF1Control[];
extern int flCTNum;
extern int flClayNum;
extern int flLoadCount;
extern int flPTNum;
extern int flSystemRenderOperation;
extern int flSystemRenderState;
extern int flFrame;
extern int (*plmalloc)();
extern void (*plfree)();
extern char lit_240_0035BFF0[], lit_241_0035C020[], lit_242_0035C040[], lit_243_0035C070[], lit_244_0035C090[], lit_245_0035C0A0[];

int flAllocMemory();
void free();
int malloc();
int flAllocMemoryS();
void flMemset();
void flPS2VramInit();
void fmsInitialize();
void mflInit();
void flPS2SystemTmpBuffInit();
void flPS2InitRenderBuff();
void flPS2SwapDBuff();
void flPADInitialize();
int sceGsSyncV();
void sceSifInitRpc();
void sceSifInitIopHeap();
void sceCdInit();
void sceCdMmode();
int sceSifRebootIop();
int sceSifSyncIop();
void flPS2IopModuleLoadStart();
void ps2HddPowerOffSet();
void sceFsReset();
void flPS2IopModuleLoad();
void flPS2PADModuleInit();
void ps2McModuleInit();
void flSndModuleInit();
void flAdxModuleInit();
void sceGsResetPath();
void sceDmaReset();
int sceDmaGetChan();
void sceVpu0Reset();
void flPS2DmaInitControl();
void flPS2DmaInterrupt();
void ps2McInit();
void flPS2DmaWait();
void flmwFlip();
void flPS2DmaSend();
void flPS2SystemTmpBuffFlush();
void flPS2DrawPreparation();
int flPS2GetSystemMemoryHandle();
int flPS2GetSystemBuffAdrs();
void func_0016AE70();
void sceGsSetDefLoadImage();
void FlushCache();
void sceGsExecLoadImage();
void sceGsSyncPath();
void flPS2ReleaseSystemMemory();
int system_work_init(void);
int system_hard_init(void);
void flPS2VramFullClear(void);

int flInitialize(void) {
    int t;

    if (system_work_init() == 0) {
        return 0;
    }
    if (system_hard_init() == 0) {
        return 0;
    }
    flPS2SystemTmpBuffInit();
    PS2S(int, 0x54) = 0;
    PS2S(int, 0x58) = 0;
    PS2S(int, 0x5C) = 0;
    flPS2VramFullClear();
    flPS2InitRenderBuff(4, 4, 1, 0, 1);
    flPS2SwapDBuff(0, 1);
    flPADInitialize();
    do {
        t = sceGsSyncV(0);
        PS2S(int, 0x4C) = t;
    } while (t != 0);
    *(int *)0x10000810 = 0x80;
    *(int *)0x10000800 = 0;
    return 1;
}

int system_work_init(void) {
    int p;
    int t;

    flMemset(flPs2State, 0, 0x470);
    flPS2VramInit();
    p = malloc(0x1100000);
    if (p == 0) {
        return 0;
    }
    fmsInitialize(flFMS, p, 0x1100000, 0x40);
    PS2S(int, 0x3FC) = 0xE00000;
    t = flAllocMemoryS(PS2S(int, 0x3FC));
    PS2S(int, 0x3F8) = t;
    mflInit(t, PS2S(int, 0x3FC), 0x40);
    PS2S(int, 0x3F0) = 0;
    plmalloc = flAllocMemory;
    flCTNum = 0;
    flPTNum = 0;
    plfree = free;
    flClayNum = 0;
    flLoadCount = 0x64;
    PS2S(int, 0x3EC) = -1;
    flSystemRenderState = 0x202;
    flSystemRenderOperation = 0x138B032;
    return 1;
}

/* original bytes: build/raw/system_hard_init.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm int system_hard_init(void)
{
#include "system_hard_init.inc"
}
#else
int system_hard_init(void) {
    int sp;
    int i;
    int *p;

    sceSifInitRpc(0);
    sceSifInitIopHeap();
    sceCdInit(0);
    sceCdMmode(2);
    while (!sceSifRebootIop(lit_240_0035BFF0)) {
    }
    while (!sceSifSyncIop()) {
    }
    sceSifInitRpc(0);
    sceSifInitIopHeap();
    sceCdInit(0);
    sceCdMmode(2);
    flPS2IopModuleLoadStart(lit_241_0035C020, 0, 0, &sp, 0xA);
    ps2HddPowerOffSet();
    sceFsReset();
    flPS2IopModuleLoad(lit_242_0035C040, 0, 0, 0);
    flPS2PADModuleInit();
    flPS2IopModuleLoad(lit_243_0035C070, 0, 0, 0);
    flPS2IopModuleLoad(lit_245_0035C0A0, 0x11, lit_244_0035C090, 0);
    ps2McModuleInit();
    flSndModuleInit();
    flAdxModuleInit();
    sceGsResetPath();
    sceDmaReset(1);
    p = (int *)flPs2State;
    for (i = 0; i < 10; i++) {
        p[0x3C4 / 4] = sceDmaGetChan(i);
        p++;
    }
    sceVpu0Reset();
    flPs2VIF1Control[0] = 1;
    flPS2DmaInitControl(flPs2VIF1Control, 0x1000, flPS2DmaInterrupt);
    ps2McInit();
    return 1;
}
#endif

/* flPS2VSyncCallback (0x0018C110, 172 bytes): original bytes, it ends with the `ei` instruction. */
asm void flPS2VSyncCallback(void)
{
#include "flPS2VSyncCallback.inc"
}

int flFlip(void) {
    flPS2DmaWait();
    flmwFlip();
    *(int *)0x10000010 = 0x83;
    *(int *)0x10000000 = 0;
    PS2S(int, 0x5C) = 0;
    flFrame++;
    flPS2DmaSend();
    flPS2SystemTmpBuffFlush();
    flPS2DrawPreparation();
    PS2S(int, 0x3F0) = 0;
    PS2S(int, 0x3EC) = -1;
    return 1;
}

void flPS2VramFullClear(void) {
    u8 img[0x60];
    int h;
    int a;
    u32 i;
    int y;

    i = 0;
    h = flPS2GetSystemMemoryHandle(0x40000, 0);
    a = flPS2GetSystemBuffAdrs(h);
    func_0016AE70(a, 0x4000);
    y = 0;
    do {
        sceGsSetDefLoadImage(img, 0, 0x10, 0, 0, (s16)y, 0x400, 0x40);
        FlushCache(0);
        sceGsExecLoadImage(img, a);
        sceGsSyncPath(0, 0);
        i++;
        y += 0x40;
    } while (i < 0x10);
    flPS2ReleaseSystemMemory(h);
}
