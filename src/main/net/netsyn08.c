/* Network play sync, system packets (SLPM_654.95 0x001BBE60-0x001BC1F8): net_send_sys, session control messages
 * (player count, character select, start handshake, item box, quest info), 8..0x18 bytes each. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 player_work[];
extern s8 my_user_id[16];

/* select_w / quest_w by offset, only what net_send_sys reads */
typedef struct NV3 { u16 x, y, z; } NV3;
typedef struct NSEL {
    u8 x00[0xA];
    s16 xA;
    u8 xC_[2];
    u8 x0C[4];
    u8 x10[0x54 - 0x10];
    u8 x54[4];
    u8 x58[0x5C - 0x58];
    NV3 x5C[4];
    u8 x74[0x8C - 0x74];
    s8 x8C[4];
    u8 x90[0x94 - 0x90];
    s8 x94[4];
    u8 x98[0xAC - 0x98];
    u16 xAC;
    u8 xAE[4];
} NSEL;
typedef struct NQW {
    u8 x00[0x181];
    u8 x181;
    u16 x182;
    s16 x184;
    s16 x186;
} NQW;
extern NSEL select_w;
extern NQW quest_w;
extern s16 no_send_timer;
extern s8 send_flag;

typedef union NPKS {
    u8 b[0x18];
    struct {
        u8 cmd, len, x2, x3, slot, b5, b6, b7;
        u16 u8_;
        s8 sA, sB;
        u8 bC;
        s8 sD, sE;
        u8 bF;
        s8 id[8];
    } c2;
    struct {
        u8 cmd, len, x2, x3, slot, b5, b6, b7, b8, b9;
    } c3;
    struct {
        u8 cmd, len, x2, x3, slot;
        u8 a[8], c[8];
    } c5;
    struct {
        u8 cmd, len, x2, x3, slot, b5;
        u16 u6;
        s16 s8_, sA;
    } c6;
    struct {
        u8 cmd, len, x2, x3, slot, b5, b6, b7;
        s32 w8, wC;
    } c7;
} NPKS;

void net_send_sys(u8 kind, u8 slot) {
    NPKS pk;
    int i;
    u8 *sw;
    NPLV *pl;
    u8 *a;
    u8 *c;

    sw = (u8 *)&select_w;
    pl = (NPLV *)(player_work + game_w.master * 0xA00);
    if (Online_ck(game_w.master) != 0) {
        if (slot == game_w.master) {
            pk.c2.x3 = 0;
            pk.c2.x2 = 0;
            switch (kind) {
            case 1:
                pk.c3.cmd = kind;
                pk.c3.len = 8;
                pk.c3.slot = game_w.pl_num;
                break;
            case 2:
                pk.c2.len = 0x18;
                pk.c2.cmd = kind;
                pk.c2.slot = slot;
                pk.c2.b5 = sw[slot + 0x54];
                pk.c2.b6 = sw[slot * 6 + 0x5D];
                pk.c2.u8_ = *(u16 *)(sw + slot * 6 + 0x5E);
                pk.c2.b7 = *(u16 *)(sw + slot * 6 + 0x60);
                pk.c2.sA = *(s16 *)(sw + 0xA);
                pk.c2.sB = *(s8 *)(sw + slot + 0x8C);
                pk.c2.bC = sw[slot + 0xC];
                pk.c2.sD = *(s8 *)(sw + slot + 0x94);
                pk.c2.bF = sw[slot + 0xAE];
                pk.c2.sE = *(u16 *)(sw + 0xAC);
                for (i = 0; i < 8; i++) {
                    pk.c2.id[i] = my_user_id[i];
                }
                break;
            case 3:
                pk.c3.len = 0xC;
                pk.c3.cmd = kind;
                pk.c3.slot = slot;
                no_send_timer = 0;
                pk.c3.b5 = game_w.xD6 + 1;
                pk.c3.b6 = game_w.xD8[0];
                pk.c3.b7 = game_w.xD8[1];
                pk.c3.b8 = game_w.xD8[2];
                pk.c3.b9 = game_w.xD8[3];
                break;
            case 4:
            case 11:
                pk.c3.len = 8;
                pk.c3.b5 = game_w.xD7;
                pk.c3.cmd = kind;
                pk.c3.slot = slot;
                no_send_timer = 0;
                break;
            case 5:
                pk.c5.len = 0x10;
                pk.c5.cmd = kind;
                pk.c5.slot = slot;
                a = &pk.c3.b5;
                c = &pk.c3.b6;
                a[0] = game_w.xE8[0];
                c[0] = game_w.xE8[1];
                a[2] = game_w.xE8[2];
                c[2] = game_w.xE8[3];
                a[4] = game_w.xE8[4];
                c[4] = game_w.xE8[5];
                a[6] = game_w.xE8[6];
                c[6] = game_w.xE8[7];
                no_send_timer = 0;
                break;
            case 6:
                pk.c6.u6 = quest_w.x182;
                pk.c6.len = 0xC;
                pk.c6.cmd = kind;
                pk.c6.slot = slot;
                pk.c6.b5 = quest_w.x181;
                pk.c6.s8_ = quest_w.x184;
                pk.c6.sA = quest_w.x186;
                break;
            case 7:
            case 10:
                pk.c7.cmd = kind;
                pk.c7.len = 0x10;
                pk.c7.slot = slot;
                if (kind == 7) {
                    pk.c7.b5 = 0;
                    pk.c7.b7 = pl->x8C3;
                } else {
                    pk.c7.b5 = 1;
                    pk.c7.b6 = game_w.x1E2;
                    pk.c7.b7 = game_w.x1E4;
                    pk.c7.w8 = game_w.x1A8[0];
                    pk.c7.wC = game_w.x1A8[1];
                }
                break;
            case 8:
                pk.c3.len = 8;
                pk.c3.cmd = kind;
                pk.c3.slot = slot;
                break;
            case 9:
            case 12:
                pk.c3.len = 8;
                pk.c3.cmd = kind;
                pk.c3.slot = slot;
                break;
            }
            send_flag = AQ_data_put(0xA, pk.b, 0);
        }
    }
}
