/* lb_cbpl01 - agent C 0x005BA940-0x005BABF4: LobbyMember callback (game_w.pl_num/master). */
/* CallBack_Result_Plaza_LobbyMember (0x5BA940): logic complete; 7/173 differ (v0/v1 naming of the cw+off pointer vs the counter at the Lb_set_player call). Not built. */
#include "lobby_b.h"
typedef struct { char id[8]; char name[0x10]; u8 pad18[4]; char mini[0x40]; } LUSER;
void CallBack_Result_Plaza_LobbyMember(CNET_RES res) {
    LUSER u;
    s32 i;
    s32 j;
    u8 *w;
    s32 found;
    s32 off;
    s32 o;
    u8 *p3;
    u8 *e;

    if (cw[0x2C31] != 5) {
        if (cw[0x2C45] == 10) {
            cw[0x2C45] = 0;
            if (res.val == 0) {
                game_w.pl_num = 0;
                i = 0;
                off = 0;
                w = (u8 *)lbCommer;
                do {
                    cnLBS_Get_LobbyMemberList(i & 0xFFFF, &u, u.name, u.mini);
                    if (u.id[0] != 0) {
                        found = 0;
                        j = 0;
                        o = 0;
                        do {
                            if (memcmp(&u, cw + o + 0x132C, 8) == 0) {
                                found = 1;
                            }
                            j++;
                            o += 0x2FC;
                        } while (j < 8);
                        if (found == 0) {
                            strcpy(cw + off + 0x132C, u.id);
                            strcpy(cw + off + 0x1334, u.name);
                            memcpy(cw + off + 0x1346, u.mini, 0x40);
                            memcpy(w, cw + off + 0x132C, 8);
                            memcpy(w + 8, cw + off + 0x1334, 0x10);
                            memcpy(w + 0x1C, cw + off + 0x1346, 0x40);
                            if (memcmp(&u, cw + 0x440, 8) == 0) {
                                game_w.master = i;
                            }
                            if (game_w.pl_num == 0 || 0 > memcmp(p3 = cw + 3, cw + off + 0x132C, 8)) {
                                memcpy(cw + 3, cw + off + 0x132C, 8);
                            }
                            Lb_set_mini_data_to_pl((s8)i, cw + off + 0x1346);
                            game_w.pl_num++;
                            e = cw + off;
                            Lb_set_player(i & 0xFF, e + 0x132C, e + 0x1334);
                        }
                    }
                    i++;
                    off += 0x2FC;
                    w += 0x5C;
                } while (i < 8);
                cw[0x35D1] = game_w.pl_num;
                cw[0x2C35]++;
                lb_sys.x04 = 0;
            } else {
                cnLBS_Get_ServerMessage(cw + 0x32D1);
                cw[0x2C35] += 2;
            }
        }
    }
}
