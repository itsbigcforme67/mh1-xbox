/* lm_member_list_mv (0x5B2E90): logic complete (decoded from asm), 191/228 instr differ: register allocation of pNet/step/&menu/sw differs (original keeps p in a2, step in a1, stp in a3 reused for sw). Not built. */
#include "lobby_b.h"
s32 lm_member_list_mv(sw)
u16 sw;
{
    s32 i;
    s32 s;
    s32 r;
    u8 *stp;
    LBNETW_B *p;
    u8 st;

    p = pNet;
    st = p->depth;
    stp = &p->depth;
    switch (st) {
    case 0:
        s = sw & 0xFFFF;
        if (s & 0x20) {
            if (*((u8 *)&player_work[p->menu]) != 0) {
                Lb_PlStatusSet();
                pNet->depth++;
                cnWrap_SoundRequest(6);
                *(u8 *)0x39DAD0 = 0;
                return 0;
            }
            cnWrap_SoundRequest(7);
            return 0;
        }
        *(u8 *)0x39DAD0 = 1;
        if (s & 0x40) {
            *(s16 *)0x39DAD2 = 0xA;
            return sw;
        }
        i = (s16)p->menu;
        if (s & 0x2000) {
            i = (s16)(i - 1);
            if (i < 0) {
                i = 7;
            }
            if ((s16)i == game_w.master) {
                i = (s16)(i - 1);
            }
            if (i < 0) {
                i = 7;
            }
        } else if (s & 0x1000) {
            i = (s16)(i + 1);
            if (i >= 8) {
                i = 0;
            }
            if ((s16)i == game_w.master) {
                i = (s16)(i + 1);
            }
            if (i >= 8) {
                i = 0;
            }
        }
        if ((s16)i != p->menu) {
            pNet->menu = i;
            cnWrap_SoundRequest(1);
            pNet->step = 0;
        }
        *(s16 *)0x39DAD2 = 0x10;
        break;
    case 1:
        s = sw & 0xFFFF;
        if (s & 0x40) {
            *stp = st - 1;
            cnWrap_SoundRequest(3);
            *(u8 *)0x39DAD0 = 1;
            *(s16 *)0x39DAD2 = 0x10;
            break;
        }
        if (p->step == 0 && (s & 0x200)) {
            *stp = st + 1;
            pNet->sel = 5;
            pNet->x05 = 0;
            cnLBS_Get_ConditionSearchUser(&SearchResult);
            memcpy(SearchResult + 4, cw + pNet->menu * 0x2FC + 0x132C, 8);
            memcpy(SearchResult + 0xC, cw + pNet->menu * 0x2FC + 0x1334, 0x12);
            cnWrap_SoundRequest(6);
        }
        if (s & 0xC00) {
            pNet->step = pNet->step ^ 1;
            cnWrap_SoundRequest(9);
        }
        if (Lb_get_pl_stat2(pNet->menu) == 2) {
            pNet->depth--;
            cnWrap_SoundRequest(3);
            *(u8 *)0x39DAD0 = 1;
            *(s16 *)0x39DAD2 = 0x10;
        }
        break;
    case 2:
        r = Plaza_add_friend(SearchResult + 4);
        if (r == 1 || r == 0) {
            pNet->depth--;
        }
        break;
    }
    return 0;
}
