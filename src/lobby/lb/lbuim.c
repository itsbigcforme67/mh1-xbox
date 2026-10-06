/* lbui, run 13: lb_chatMemberCheck .. Lb_checkChatID (lobby.bin 0x00598B80-0x00598DB0): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int lb_chatMemberCheck(void) {
    u8 *p;
    s16 i;
    LB_NETW *n;

    switch (pNet->x04) {
    case 0:
        p = (u8 *)chatIDList + pNet->idx * 8;
        if ((s8)p[0] != 0) {
            memcpy(CW->x2F80, p, 8);
            pNet->x04++;
            break;
        }
        return 0;
    case 1:
        switch (Lbs_SeekId()) {
        case 0:
            if (ClassInfo.plaza == CW->x30B4 && CW->x30B6 == 0) {
                n = pNet;
                chatListFlag = chatListFlag | (1 << n->idx);
            } else {
                n = pNet;
                chatListFlag = chatListFlag & ~(1 << n->idx);
            }
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        case 1:
            n = pNet;
            chatListFlag = chatListFlag & ~(1 << n->idx);
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        }
        break;
    }
    return 2;
}

int Lb_checkChatID(id)
u8 *id;
{
    s8 i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i++;
        p += 8;
        if (i >= 7) {
            return 0;
        }
    }
}
