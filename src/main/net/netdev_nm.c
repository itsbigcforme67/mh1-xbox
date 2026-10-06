/* netdev_nm - near-matches written in the ninth assignment, NOT built:
   DeviceUpdateStatus (SLPM_654.95 0x002337F0, 12 of 83 instructions off: register order of n/i/p and the hoisted SifRpcWork_buf pointers),
   light_init (0x0011DB10, all instructions differ in order: see agent-C.md). Names are guesses. */
#include "types.h"
#include "game.h"
typedef struct DEVREC { s32 x00; s32 x04; s32 id; s32 sub; s32 x10; s32 x14; s32 x18; } DEVREC;
extern DEVREC *CurDevice;
extern DEVREC DeviceWork[];
extern s32 SifRpcWork_buf[8];
extern s32 TotalDeviceNum;
extern s32 UsbDeviceNum;
void *memset();
int set_device_no();
int search_sif_call_rpc();
int DeviceUpdateStatus(void) {
    int n;
    s32 *b1;
    int i;
    DEVREC *p;
    s32 *b3;
    s32 *b2;

    if (CurDevice != 0) {
        return 0;
    }
    search_sif_call_rpc(0);
    n = SifRpcWork_buf[0];
    if (n != UsbDeviceNum) {
        p = DeviceWork;
        b1 = &SifRpcWork_buf[1];
        b2 = &SifRpcWork_buf[2];
        b3 = &SifRpcWork_buf[3];
        for (i = 0; i < 0x20; i++, p++) {
            SifRpcWork_buf[0] = i;
            search_sif_call_rpc(1);
            if (SifRpcWork_buf[0] == -1) {
                memset(p, 0xFF, 0x1C);
            } else {
                set_device_no(p, SifRpcWork_buf[0], *b1, *b2, *b3);
            }
        }
        UsbDeviceNum = n;
        TotalDeviceNum = n;
        for (i = 0x20; i < 0x23; i++) {
            if (DeviceWork[i].x00 != -1) {
                TotalDeviceNum++;
            }
        }
        return 1;
    }
    return 0;
}

/* light_init: logic complete (two sets of three lights filled from stg_light_tbl / pl_light_tbl), schedule not matching. */
extern GAME_W game_w;
extern u8 light_work[];
typedef struct LTBL { u8 *p[5]; } LTBL;
extern LTBL stg_light_tbl;
extern LTBL pl_light_tbl[];
void light_init(void) {
    int i;
    u8 *t;
    int o12;
    int o16;
    f32 *q;
    LTBL *tb;

    memset(light_work, 0, 0x290);
    i = 0;
    t = light_work + 0x10;
    o12 = 0;
    o16 = 0;
    do {
        i++;
        q = (f32 *)(stg_light_tbl.p[0] + o12);
        *(f32 *)(t + 0x48) = q[0];
        *(f32 *)(t + 0x4C) = q[1];
        *(f32 *)(t + 0x50) = q[2];
        q = (f32 *)(stg_light_tbl.p[1] + o12);
        o12 += 12;
        *(f32 *)(t + 0x3C) = q[0];
        *(f32 *)(t + 0x40) = q[1];
        *(f32 *)(t + 0x44) = q[2];
        q = (f32 *)(stg_light_tbl.p[2] + o16);
        *(f32 *)(t + 0xC) = 10.0f * q[0];
        *(f32 *)(t + 0x10) = 10.0f * q[1];
        *(f32 *)(t + 0x14) = 10.0f * q[2];
        q = (f32 *)(stg_light_tbl.p[3] + o16);
        *(f32 *)(t + 0x2C) = q[0];
        *(f32 *)(t + 0x30) = q[1];
        *(f32 *)(t + 0x34) = q[2];
        q = (f32 *)(stg_light_tbl.p[4] + o16);
        o16 += 16;
        *(f32 *)(t + 0x1C) = q[0];
        *(f32 *)(t + 0x20) = q[1];
        *(f32 *)(t + 0x24) = q[2];
        *(s32 *)(t + 8) = 0;
        t += 0x68;
    } while (i < 3);
    i = 0;
    t = light_work + 0x140 + 0x10;
    o12 = 0;
    o16 = 0;
    do {
        i++;
        tb = &pl_light_tbl[game_w.stage];
        q = (f32 *)(tb->p[0] + o12);
        *(f32 *)(t + 0x48) = q[0];
        *(f32 *)(t + 0x4C) = q[1];
        *(f32 *)(t + 0x50) = q[2];
        q = (f32 *)(tb->p[1] + o12);
        o12 += 12;
        *(f32 *)(t + 0x3C) = q[0];
        *(f32 *)(t + 0x40) = q[1];
        *(f32 *)(t + 0x44) = q[2];
        q = (f32 *)(tb->p[2] + o16);
        *(f32 *)(t + 0xC) = q[0];
        *(f32 *)(t + 0x10) = q[1];
        *(f32 *)(t + 0x14) = q[2];
        q = (f32 *)(tb->p[3] + o16);
        *(f32 *)(t + 0x2C) = q[0];
        *(f32 *)(t + 0x30) = q[1];
        *(f32 *)(t + 0x34) = q[2];
        q = (f32 *)(tb->p[4] + o16);
        o16 += 16;
        *(f32 *)(t + 0x1C) = q[0];
        *(f32 *)(t + 0x20) = q[1];
        *(f32 *)(t + 0x24) = q[2];
        *(s32 *)(t + 8) = 0;
        t += 0x68;
    } while (i < 3);
}
