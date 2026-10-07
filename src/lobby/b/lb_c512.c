/* lb_c512 - agent C round 5 0x005B1160-0x005B134C: lb_select_room (room list entry screen state; one-case switch for the first-time init, tm temp for the 3-way wrap). lb_select_set_data is still asm: called as a global. */
#include "lobby_b.h"
extern char lit_427_0065E260[];
extern char lit_428_0065E270[];
extern char lit_429_0065E280[];
extern char lit_430_0065E288[];
extern char lb_board_exp[];
extern char lb_quest_data_tbl[];
extern s16 lb_quest_color_tex[];
extern char *lb_quest_attribute[];
extern char *lb_num_str[];
extern char *lb_rule_msg_etc[];
extern char *lb_guild_str[];
extern char *map_name[];
typedef struct { s16 a, b; } P2;
typedef struct { u8 pad0[2]; u8 x2; u8 pad3; s32 x4; s32 x8; u8 padC[4]; s32 x10; s32 x14; char **x18; u8 pad1C; u8 x1D; } QI;
typedef struct { u8 pad0[2]; u16 x2; u16 x4; u8 pad6[0xB]; u8 x11; u8 pad12[0x43]; char x55[1]; } RI;
void han2zen(char *, char *);
char *Lb_get_quest_str();
void Lb_num_to_str();
void KinshiYogo_chk();
RI *Lbs_GetRoomInfo();

void lb_select_set_data(s32);
void Lb_put_set01();
s32 lb_select_room(void) {
    RI *r;
    s32 sw;
    s32 t;
    s32 v;
    u8 d;
    u8 tm;
    s32 vv;

    r = Lbs_GetRoomInfo(F(u8, pNet, 7));
    sw = Get_sw2(0) & 0xFFFF;
    d = pNet->depth;
    switch (d) {
    case 0:
        pNet->depth = d + 1;
        v = (u32)(F(s32, r, 0x158) & 0x1FE) >> 1;
        F(s8, pNet, 0x12) = v;
        vv = v & 0xFF;
        if (F(u8, r, 0x10) != 3) {
            F(s8, pNet, 0xE) = 1;
        } else {
            F(s8, pNet, 0xE) = 0;
        }
        lb_select_set_data(vv);
        break;
    }
    Lbs_GetClassAdd();
    switch (F(u8, r, 0x10)) {
    case 4:
        Lb_put_set01(7);
        goto done;
    default:
        Lb_put_set01(6);
done:
        cnWrap_SoundRequest(3);
        return 3;
    case 3:
        t = sw & 0xFFFF;
        if (t & 0x20) {
            lb_sys.x73 = F(u8, pNet, 7);
            Lbs_GetRoomInfo(lb_sys.x73);
            cnWrap_SoundRequest(0);
            return 0;
        }
        if (t & 0x40) {
            lb_sys.x06 = 0xE;
            cnWrap_SoundRequest(3);
            return 3;
        }
        if (t & 0x200) {
            tm = pNet->menu + 1;
            pNet->menu = tm;
            if ((tm & 0xFF) > 2) {
                pNet->menu = 0;
            }
            cnWrap_SoundRequest(6);
        }
        sprintf(lb_board_exp + 0x130, lit_428_0065E270, lb_num_str[r->x2], lb_num_str[0xB], lb_num_str[r->x4], lb_rule_msg_etc[0]);
        return 2;
    }
}
