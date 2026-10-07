/* lb_c503 - agent C round 5 0x005BDD50-0x005BDF6C: lbc_game_ready_00 (read the match information, copy the room members; locals declared in reverse register order). */
#include "lobby_a.h"
#include "lbnet.h"
extern u8 USER_PL_ID;
extern char room_member_id[];
extern char room_member_handle[];
extern char room_member_mini_data[];
extern char CallBack_Result_Match_MatchInformation[];
typedef struct { u8 pad0[0x2C33]; u8 x2C33; u8 x2C34; u8 pad2C35[0x10]; s8 x2C45; u8 pad2C46; u8 x2C47; } CWS_gr;
#define CWX ((CWS_gr *)cw)
void lbc_game_ready_00(void) {
    CNET_W5D4 mi;
    u8 *e;
    s32 o;
    char *id;
    char *hd;
    char *mn;
    s32 i;
    u8 st;
    u8 *stp;

    st = CWX->x2C34;
    stp = &CWX->x2C34;
    switch (st) {
    case 0:
        *stp = st + 1;
        fade_set(2);
    case 1:
        CWX->x2C34++;
    case 2:
        CWX->x2C34++;
        CallBackWaitInit();
        CWX->x2C45 = 0x20;
        cnLBS_Read_MatchInfomation(CallBack_Result_Match_MatchInformation);
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        CWX->x2C33++;
        CWX->x2C34 = 0;
        cnLBS_Get_MatchInfomation(&mi);
        CWX->x2C47 = *(u8 *)&mi;
        USER_PL_ID = *((u8 *)&mi + 1);
        memcpy(cw + 0x35E0, (u8 *)&mi + 2, 0x10);
        i = 0;
        if (0 < CWX->x2C47) {
            e = (u8 *)&mi;
            o = 0;
            id = room_member_id;
            hd = room_member_handle;
            mn = room_member_mini_data;
            do {
                strcpy(cw + o + 0x73C, e + 0x114);
                strcpy(cw + o + 0x744, e + 0x11C);
                memcpy(cw + o + 0x756, e + 0x130, 0x40);
                memcpy(id, e + 0x114, 8);
                memcpy(hd, e + 0x11C, 0x11);
                memcpy(mn, e + 0x130, 0x40);
                i++;
                id += 8;
                hd += 0x11;
                mn += 0x40;
                *(s8 *)(cw + o + 0x796) = *(u16 *)(e + 0x190);
                *(s32 *)(cw + o + 0x7B4) = *(s32 *)(e + 0x194);
                e += 0x98;
                o += 0x2FC;
            } while (i < CWX->x2C47);
        }
        break;
    }
}
