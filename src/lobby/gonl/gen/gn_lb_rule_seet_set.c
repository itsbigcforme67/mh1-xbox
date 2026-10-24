/* G online (PC): lb_rule_seet_set, written from Monster Hunter G's code at lobby 0x55BCF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 KinshiYogo_chk();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 cnWrap_SoundRequest();
s32 han2zen();
void * memcpy();
void * memset();
void * strcpy();
extern u8 D_0035EFEB;
extern u8 lb_num_str;
extern u8 lb_quest_exp;
extern u8 lb_rule_data_tbl;
extern u8 lb_rule_exp;
extern u8 lb_rule_msg_etc;
extern u8 lb_shop_msg;
extern u8 lb_sys;
s32 GetRoomRule();                              /* extern */
s32 Lb_get_cursor_col();                            /* extern */
s32 Lb_put_set01();                      /* extern */
s32 guild_input_message();                  /* extern */
s32 guild_input_pass();                     /* extern */
s32 lobby_func_0059D7B0();               /* extern */
s32 sprintf();    /* extern */
extern s32 Lb_guild_trans;
extern s32 lb_quest_data_tbl;
extern s32 lit_1137_00664A60;
extern s8 mhRule;

s32 lb_rule_seet_set(void) {
    u8 g_frame[0x80] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s8 temp_a1;
    s8 temp_v0;
    s8 temp_v0_2;

    temp_s1 = Lb_get_cursor_col();
    temp_s0 = Get_sw2(0) & 0xFFFF;
    GetRoomRule();
    switch ((*(s8 *)((u8 *)&lb_sys + 0x1E))) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(s8 *)((u8 *)&lb_sys + 0x1E)) += 1;
        Lbc_set_prim(0, (s32) &Lb_guild_trans, 0, 0);
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x30)) = 0x80206020;
        mhRule = 3;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x44)) = 0x80206020;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x58)) = 0x80206020;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = 0x80206020;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = 0x80206020;
        lb_quest_data_tbl = 0x80206020;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x8)) = 0xC000FF00;
        (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x1C)) = 0xC000FF00;
        sprintf(&(*(s32 *)((u8 *)&lb_rule_exp + 0x4)), &lit_1137_00664A60, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_num_str + 0x4)) + (mhRule * 4))), (*(s32 *)((u8 *)&lb_shop_msg + 0x8)));
        strcpy(&(*(s32 *)((u8 *)&lb_rule_exp + 0x68)), (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_rule_msg_etc + 0x4)) + ((*(s8 *)((u8 *)&mhRule + 0x7)) * 4))));
        strcpy(&(*(s32 *)((u8 *)&lb_quest_exp + 0x5C)), (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_rule_msg_etc + 0x4)) + ((*(s8 *)((u8 *)&mhRule + 0x11)) * 4))));
block_99:
    default:                                        /* switch 1 */
        return 0;
    case 1:                                         /* switch 1 */
        temp_a1 = (*(s8 *)((u8 *)&mhRule + 0x4F));
        switch (temp_a1) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
            temp_v1 = temp_s0 & 0xFFFF;
            if (temp_v1 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) = 7;
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x8)) = 0x80206020;
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x1C)) = 0x80206020;
                return 0;
            }
            if (temp_v1 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x8)) = 0x80206020;
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x1C)) = 0x80206020;
                (*(s8 *)((u8 *)&mhRule + 0x4F)) += 1;
                return 0;
            }
            if (temp_v1 & 0x800) {
                temp_v0 = mhRule - 1;
                mhRule = temp_v0;
                if ((((s8) ((s32) temp_v0))) < 0) {
                    mhRule = 3;
                }
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            } else if (temp_v1 & 0x400) {
                temp_v0_2 = mhRule + 1;
                mhRule = temp_v0_2;
                if ((((s8) ((s32) temp_v0_2))) >= 4) {
                    mhRule = 0;
                }
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x8)) = temp_s1;
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x1C)) = temp_s1;
            sprintf(&(*(s32 *)((u8 *)&lb_rule_exp + 0x4)), &lit_1137_00664A60, (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_num_str + 0x4)) + (mhRule * 4))), (*(s32 *)((u8 *)&lb_shop_msg + 0x8)));
            goto block_99;
        case 1:                                     /* switch 2 */
            temp_v1_2 = temp_s0 & 0xFFFF;
            if (temp_v1_2 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x30)) = 0x80206020;
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x44)) = 0x80206020;
                (*(s8 *)((u8 *)&mhRule + 0x4F)) -= 1;
                return 0;
            }
            if (temp_v1_2 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if ((*(s8 *)((u8 *)&mhRule + 0x7)) == 0) {
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 4;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x30)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x44)) = 0x80206020;
                } else {
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x30)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x44)) = 0x80206020;
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) += 1;
                }
                return 0;
            }
            if (temp_v1_2 & 0xC00) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x7)) ^= 1;
                strcpy(&(*(s32 *)((u8 *)&lb_rule_exp + 0x68)), (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_rule_msg_etc + 0x4)) + ((*(s8 *)((u8 *)&mhRule + 0x7)) * 4))));
            }
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x30)) = temp_s1;
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x44)) = temp_s1;
            goto block_99;
        case 2:                                     /* switch 2 */
            temp_v1_3 = temp_s0 & 0xFFFF;
            if (temp_v1_3 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x58)) = 0x80206020;
                (*(s8 *)((u8 *)&mhRule + 0x4F)) -= 1;
                return 0;
            }
            if (temp_v1_3 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) = 4;
                (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x58)) = 0x80206020;
                return 0;
            }
            if (temp_v1_3 & 0x20) {
                (*(s8 *)((u8 *)&mhRule + 0x4F)) = temp_a1 + 1;
                Lbc_init_network_work(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)&D_0035EFEB) = 0;
                cnWrap_SoundRequest(6, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x58)) = temp_s1;
            goto block_99;
        case 3:                                     /* switch 2 */
            if (guild_input_pass(&(*(s8 *)((u8 *)&mhRule + 0x8)), temp_a1) == 1) {
                (*(s8 *)((u8 *)&mhRule + 0x4F)) -= 1;
                Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)&D_0035EFEB) = 1;
            }
            memcpy(&sp30, &(*(s8 *)((u8 *)&mhRule + 0x8)), 9);
            han2zen(&sp30, &(*(s32 *)((u8 *)&lb_rule_exp + 0xCC)));
            goto block_99;
        case 4:                                     /* switch 2 */
            temp_v1_4 = temp_s0 & 0xFFFF;
            if (temp_v1_4 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if ((*(s8 *)((u8 *)&mhRule + 0x7)) == 0) {
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 1;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = 0x80206020;
                } else {
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 2;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = 0x80206020;
                }
                return 0;
            }
            if (temp_v1_4 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if ((*(s8 *)((u8 *)&mhRule + 0x11)) == 0) {
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 7;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = 0x80206020;
                } else {
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = 0x80206020;
                    (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = 0x80206020;
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) += 1;
                }
                return 0;
            }
            if (temp_v1_4 & 0xC00) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x11)) ^= 1;
                strcpy(&(*(s32 *)((u8 *)&lb_quest_exp + 0x5C)), (*(s32 *)((u8 *)&(*(s32 *)((u8 *)&lb_rule_msg_etc + 0x4)) + ((*(s8 *)((u8 *)&mhRule + 0x11)) * 4))));
            }
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x6C)) = temp_s1;
            (*(s32 *)((u8 *)&lb_rule_data_tbl + 0x80)) = temp_s1;
            goto block_99;
        case 5:                                     /* switch 2 */
            temp_v1_5 = temp_s0 & 0xFFFF;
            if (temp_v1_5 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) -= 1;
            } else if (temp_v1_5 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) = 7;
            } else if (temp_v1_5 & 0x20) {
                (*(u8 *)&D_0035EFEB) = 0;
                cnWrap_SoundRequest(6, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) += 1;
            }
            goto block_99;
        case 6:                                     /* switch 2 */
            if (guild_input_message(&(*(s8 *)((u8 *)&mhRule + 0x12)), temp_a1) == 1) {
                KinshiYogo_chk(&(*(s8 *)((u8 *)&mhRule + 0x12)));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) -= 1;
                Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(u8 *)&D_0035EFEB) = 1;
            }
            memcpy(&(*(s32 *)((u8 *)&lb_quest_exp + 0xC0)), &(*(s8 *)((u8 *)&mhRule + 0x12)), 0x3D);
            goto block_99;
        case 7:                                     /* switch 2 */
            temp_v1_6 = temp_s0 & 0xFFFF;
            if (temp_v1_6 & 0x2000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                if ((*(s8 *)((u8 *)&mhRule + 0x11)) == 0) {
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 4;
                    lb_quest_data_tbl = 0x80206020;
                } else {
                    lb_quest_data_tbl = 0x80206020;
                    (*(s8 *)((u8 *)&mhRule + 0x4F)) = 5;
                }
                return 0;
            }
            if (temp_v1_6 & 0x1000) {
                cnWrap_SoundRequest(1, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)((u8 *)&mhRule + 0x4F)) = 0;
                lb_quest_data_tbl = 0x80206020;
                return 0;
            }
            if (temp_v1_6 & 0x20) {
                if (((*(s8 *)((u8 *)&mhRule + 0x7)) != 0) && ((*(s8 *)((u8 *)&mhRule + 0x8)) == 0)) {
                    if ((Lb_put_set01(3, temp_a1) == 1) && ((*(s8 *)((u8 *)&mhRule + 0x11)) != 0) && ((*(s8 *)((u8 *)&mhRule + 0x12)) == 0)) {
                        lobby_func_0059D7B0(4);
                    }
                    return 0;
                }
                if (((*(s8 *)((u8 *)&mhRule + 0x11)) != 0) && ((*(s8 *)((u8 *)&mhRule + 0x12)) == 0)) {
                    Lb_put_set01(4, temp_a1);
                    return 0;
                }
                if ((*(s8 *)((u8 *)&mhRule + 0x7)) == 0) {
                    memset(&(*(s8 *)((u8 *)&mhRule + 0x8)), 0, 9);
                }
                if ((*(s8 *)((u8 *)&mhRule + 0x11)) == 0) {
                    memset(&(*(s8 *)((u8 *)&mhRule + 0x12)), 0, 0x3D);
                }
                (*(s8 *)((u8 *)&lb_sys + 0x1E)) = 0;
                cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return 1;
            }
            lb_quest_data_tbl = temp_s1;
            goto block_99;
        }
        break;
    }
    return 0;
}

