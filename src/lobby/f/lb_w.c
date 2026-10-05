/* Lobby: guild rule sheet editor (lb_rule_seet_set), hand-written from the m2c draft. */
#include "lobby_f.h"

#define RDT(o) (*(u32 *)(lb_rule_data_tbl + (o)))
extern u8 lb_rule_data_tbl[];
extern u8 lb_rule_exp[];
extern char *lb_num_str[];
extern char *lb_rule_msg_etc[];
extern char lit_1137_00664A60[];
extern u8 lb_pit[0xC];
int Lb_get_cursor_col();
int Get_sw2();
void GetRoomRule();
void Lbc_set_prim();
void Lb_guild_trans();
int sprintf(char *, const char *, ...);
char *strcpy();
void Lbc_init_network_work();
int guild_input_pass();
int guild_input_message();
void han2zen();
void KinshiYogo_chk();
void Lb_put_set01();
int lb_rule_seet_set(void) {
    char sp30[0x50];
    int col;
    int pad;
    col = Lb_get_cursor_col();
    pad = Get_sw2(0) & 0xFFFF;
    GetRoomRule();
    switch (lb_sys.x08) {
    case 0:
        lb_sys.x08 = lb_sys.x08 + 1;
        Lbc_set_prim(0, Lb_guild_trans, 0);
        RDT(0x30) = 0x80206020;
        mhRule.x00 = 3;
        RDT(0x44) = 0x80206020;
        RDT(0x58) = 0x80206020;
        RDT(0x6C) = 0x80206020;
        RDT(0x80) = 0x80206020;
        RDT(0x94) = 0x80206020;
        RDT(8) = 0xC000FF00;
        RDT(0x1C) = 0xC000FF00;
        sprintf((char *)lb_rule_exp + 4, lit_1137_00664A60, lb_num_str[1 + mhRule.x00], lb_rule_msg_etc);
        strcpy((char *)lb_rule_exp + 0x68, lb_rule_msg_etc[1 + mhRule.x07]);
        strcpy((char *)lb_rule_exp + 0x130, lb_rule_msg_etc[1 + mhRule.x11]);
    default:
        return 0;
    case 1:
        switch (mhRule.x4F) {
        case 0:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                mhRule.x4F = 7;
                RDT(8) = 0x80206020;
                RDT(0x1C) = 0x80206020;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                RDT(8) = 0x80206020;
                RDT(0x1C) = 0x80206020;
                mhRule.x4F = mhRule.x4F + 1;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x800) {
                s8 w = mhRule.x00 - 1;
                mhRule.x00 = w;
                if (w < 0) {
                    mhRule.x00 = 3;
                }
                cnWrap_SoundRequest(1);
            } else if ((pad & 0xFFFF) & 0x400) {
                s8 w = mhRule.x00 + 1;
                mhRule.x00 = w;
                if (w >= 4) {
                    mhRule.x00 = 0;
                }
                cnWrap_SoundRequest(1);
            }
            RDT(8) = col;
            RDT(0x1C) = col;
            sprintf((char *)lb_rule_exp + 4, lit_1137_00664A60, lb_num_str[1 + mhRule.x00], lb_rule_msg_etc);
            return 0;
        case 1:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                RDT(0x30) = 0x80206020;
                RDT(0x44) = 0x80206020;
                mhRule.x4F = mhRule.x4F - 1;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                if (mhRule.x07 == 0) {
                    mhRule.x4F = 4;
                    RDT(0x30) = 0x80206020;
                    RDT(0x44) = 0x80206020;
                } else {
                    RDT(0x30) = 0x80206020;
                    RDT(0x44) = 0x80206020;
                    mhRule.x4F = mhRule.x4F + 1;
                }
                return 0;
            }
            if ((pad & 0xFFFF) & 0xC00) {
                cnWrap_SoundRequest(1);
                mhRule.x07 = mhRule.x07 ^ 1;
                strcpy((char *)lb_rule_exp + 0x68, lb_rule_msg_etc[1 + mhRule.x07]);
            }
            RDT(0x30) = col;
            RDT(0x44) = col;
            return 0;
        case 2:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                RDT(0x58) = 0x80206020;
                mhRule.x4F = mhRule.x4F - 1;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                mhRule.x4F = 4;
                RDT(0x58) = 0x80206020;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x20) {
                mhRule.x4F = mhRule.x4F + 1;
                Lbc_init_network_work(1);
                *(s8 *)0x3F36AB = 0;
                cnWrap_SoundRequest(6);
            }
            RDT(0x58) = col;
            return 0;
        case 3:
            if (guild_input_pass(mhRule.pass) == 1) {
                mhRule.x4F = mhRule.x4F - 1;
                Lbc_init_network_work();
                *(u8 *)0x3F36AB = 1;
            }
            memcpy(sp30, mhRule.pass, 9);
            han2zen(sp30, lb_rule_exp + 0xCC);
            return 0;
        case 4:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                if (mhRule.x07 == 0) {
                    mhRule.x4F = 1;
                    RDT(0x6C) = 0x80206020;
                    RDT(0x80) = 0x80206020;
                } else {
                    mhRule.x4F = 2;
                    RDT(0x6C) = 0x80206020;
                    RDT(0x80) = 0x80206020;
                }
                return 0;
            }
            if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                if (mhRule.x11 == 0) {
                    mhRule.x4F = 7;
                    RDT(0x6C) = 0x80206020;
                    RDT(0x80) = 0x80206020;
                } else {
                    RDT(0x6C) = 0x80206020;
                    RDT(0x80) = 0x80206020;
                    mhRule.x4F = mhRule.x4F + 1;
                }
                return 0;
            }
            if ((pad & 0xFFFF) & 0xC00) {
                cnWrap_SoundRequest(1);
                mhRule.x11 = mhRule.x11 ^ 1;
                strcpy((char *)lb_rule_exp + 0x130, lb_rule_msg_etc[1 + mhRule.x11]);
            }
            RDT(0x6C) = col;
            RDT(0x80) = col;
            return 0;
        case 5:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                mhRule.x4F = mhRule.x4F - 1;
            } else if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                mhRule.x4F = 7;
            } else if ((pad & 0xFFFF) & 0x20) {
                *(u8 *)0x3F36AB = 0;
                cnWrap_SoundRequest(6);
                Lbc_init_network_work();
                mhRule.x4F = mhRule.x4F + 1;
            }
            return 0;
        case 6:
            if (guild_input_message(mhRule.msg) == 1) {
                KinshiYogo_chk(mhRule.msg);
                mhRule.x4F = mhRule.x4F - 1;
                Lbc_init_network_work();
                *(u8 *)0x3F36AB = 1;
            }
            memcpy(lb_rule_exp + 0x194, mhRule.msg, 0x3D);
            return 0;
        case 7:
            if ((pad & 0xFFFF) & 0x2000) {
                cnWrap_SoundRequest(1);
                if (mhRule.x11 == 0) {
                    mhRule.x4F = 4;
                    RDT(0x94) = 0x80206020;
                } else {
                    RDT(0x94) = 0x80206020;
                    mhRule.x4F = 5;
                }
                return 0;
            }
            if ((pad & 0xFFFF) & 0x1000) {
                cnWrap_SoundRequest(1);
                mhRule.x4F = 0;
                RDT(0x94) = 0x80206020;
                return 0;
            }
            if ((pad & 0xFFFF) & 0x20) {
                if (mhRule.x07 != 0 && mhRule.pass[0] == 0) {
                    Lb_put_set01(3);
                    return 0;
                }
                if (mhRule.x11 != 0 && mhRule.msg[0] == 0) {
                    Lb_put_set01(4);
                    return 0;
                }
                if (mhRule.x07 == 0) {
                    memset(mhRule.pass, 0, 9);
                }
                if (mhRule.x11 == 0) {
                    memset(mhRule.msg, 0, 0x3D);
                }
                lb_sys.x08 = 0;
                cnWrap_SoundRequest(0);
                return 1;
            }
            RDT(0x94) = col;
            return 0;
        }
        break;
    }
}
