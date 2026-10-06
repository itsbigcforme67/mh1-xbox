/* Network play sync, system packets (SLPM_654.95 0x001BC200-0x001BC688): net_receive_sys, applies the session control messages
 * sent by net_send_sys (see netsyn08.c): player count, character select, start handshake, item box, quest info, map sign, drop. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 player_work[];
extern u8 select_w[];

typedef struct NQWR {
    u8 x00[0x180];
    u8 x180;
    u8 x181;
    u16 x182;
    s16 x184;
    s16 x186;
} NQWR;
extern NQWR quest_w;

void Quest_net_sub(u8);
void MapSignRequest(u8);
void mcsls_force_drop(u8);
void net_send_sys(u8, u8);

void net_receive_sys(int slot0, u8 *buf) {
    u8 *p;
    u8 *sw;
    u8 kind;
    u8 slot;
    u8 s;
    u8 *t8;
    u8 who;
    int v;
    int bit;
    u16 t;
    s8 i;
    int j;
    u8 *q;
    u8 *r;

    sw = select_w;
    if (Online_ck() != 0) {
        kind = buf[0];
        p = buf + 4;
        switch (kind) {
        case 1:
            if (game_w.master != game_w.x21B) {
                game_w.pl_num = p[0];
            }
            break;
        case 2:
            slot = p[0];
            sw[slot + 0xC] = p[8];
            sw[slot + 0x94] = p[9];
            sw[slot + 0x54] = p[1];
            sw[slot * 6 + 0x5D] = p[2];
            *(u16 *)(sw + slot * 6 + 0x5E) = *(u16 *)(p + 4);
            *(s16 *)(sw + slot * 6 + 0x60) = p[3];
            sw[slot + 0x8C] = p[7];
            sw[slot + 0xAE] = p[0xB];
            for (j = 0; j < 8; j++) {
                game_w.x1E8[slot][j] = ((s8 *)p)[0xC + j];
            }
            if (game_w.master != game_w.x21B) {
                if (slot == 0) {
                    *(s16 *)(sw + 0xA) = p[6];
                    *(s16 *)(sw + 0xAC) = p[0xA];
                }
            }
            break;
        case 3:
            game_w.xD8[p[0]] = p[1];
            game_w.xE0[0] = p[2];
            game_w.xE0[1] = p[3];
            game_w.xE0[2] = p[4];
            game_w.xE0[3] = p[5];
            break;
        case 4:
        case 11:
            slot = p[0];
            game_w.x108[slot] = p[1];
            if (kind == 0xB) {
                game_w.xE8[slot * 2] = slot;
                game_w.xE8[slot * 2 + 1] = game_w.x108[slot];
                game_w.x108[slot] = 0;
                game_w.x108[slot + 4] = 1;
                net_send_sys(5, game_w.master);
            }
            break;
        case 5:
            slot = p[0];
            t8 = (u8 *)&game_w + slot * 2;
            q = t8 + 0xF8;
            r = t8 + 0xF9;
            for (i = 0; i < 4; i++) {
                if (p[1] == game_w.master && p[2] != 0) {
                    *q = slot;
                    *r = p[2];
                }
                p += 2;
            }
            break;
        case 6:
            who = p[0];
            if (game_w.master != who) {
                quest_w.x180 = who;
                quest_w.x182 = *(u16 *)(p + 2);
                quest_w.x181 = p[1];
                quest_w.x184 = *(s16 *)(p + 4);
                quest_w.x186 = *(s16 *)(p + 6);
                Quest_net_sub(who);
            }
            break;
        case 7:
        case 10:
            s = p[3];
            if (kind == 7) {
                if (game_w.master == game_w.x21B) {
                    v = s & 0xFF;
                    game_w.x1E2 = p[0];
                    bit = 1 << (v % 32);
                    t = s & 0xFF;
                    if (game_w.x1A8[t >> 5] & bit) {
                        game_w.x1E4 = 0xFF;
                    } else {
                        game_w.x1A8[t >> 5] |= bit;
                        game_w.x1E4 = t;
                    }
                    net_send_sys(0xA, 0);
                }
            } else {
                who = p[2];
                if (who == game_w.master && (s & 0xFF) != 0xFF) {
                    Pl_item_stack(player_work + who * 0xA00, game_w.item[s & 0xFF].id, game_w.item[s & 0xFF].num);
                }
                game_w.x1A8[0] |= *(s32 *)(p + 4);
                game_w.x1A8[1] |= *(s32 *)(p + 8);
            }
            break;
        case 9:
            MapSignRequest(p[0]);
            break;
        case 12:
            game_w.x21E |= (1 << p[0]) & 0xFF;
            mcsls_force_drop(p[0]);
            break;
        }
    }
}
