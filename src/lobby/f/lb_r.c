/* Lobby: lb_basic_master (town player master: stick, NPC talk, chairs, doors) hand-written. */
#include "lobby_f.h"
extern u8 npc_sound_tbl[0x3B];
extern u8 lb_pit[0xC];
extern u8 User_data[];
extern PLW player_work[];
extern u8 *pNet;
extern u8 ClassInfo[];
void flMemset();
u8 Lb_stick_pow_get();
u16 Lb_stick_dir_set();
void Lb_Pl_chat_act_set();
int trade_get_ck_005D0750();
void Lb_pl_status_i();
void NPCZoomInCameraRequest();
void Lb_shop_init();
int Event_flag_ck();
void Lb_send_data_to_myself();
void Lb_send_chair_req();
void Lb_put_set01();
void SetDialogData();
void SetDialogYesNo();
void fade_set();
void lb_goto_guest_room();
void flvecCopy();
void cpRotMatrix();
void SetVector(f32 *, f32, f32, f32);
extern u8 D_3E5506[];
void flvecApplyMat33();
void pl_flag_set();
u8 Warehouse_search_space();
void LegendSwordCameraRequest();
void Event_flag_set();
void Warehouse_equip_stack();
void Lb_reset_talk_count();
int Lb_ItemBox_open();
void Lb_Pl_act_set2();
void Lb_send_trade_start();
void Lbc_SendMiniData();
void Lb_put_chat();
void cnWrap_SoundRequest();
u8 *Lbs_GetRoomInfo();
int Online_ck();
void Lbc_set_prim();
void Lb_shop_init();
void Lb_pl_to_chair();
void Lb_Pl_adj_calc();
extern u8 *cw;
static void sound_call_005D3640(u8 *a, int b) {
    lb_sys.x80 = (s32)a;
    lb_sys.x84 = b;
    lb_sys.x7C = 20;
}
void lb_basic_master(PLW *pl) {
    f32 pos[3];
    f32 off[4];
    f32 vec[4];
    s32 rot[4];
    f32 off2[4];
    f32 vec2[4];
    s32 rot2[4];
    u8 mat[0x40];
    u8 mat2[0x40];
    u32 un;
    u8 *tgt;
    u8 *em;
    u8 *fh;
    u8 *pm;
    u8 *ri;
    u16 ang;
    int a;
    int n;
    PLW *mpl;
    switch (Lb_stick_pow_get(pl) & 0xFF) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
        a = pl->flag15;
        if (a != 2 && a != 0x24 && a != 0x1F) {
            if (pl->id == game_w.master) {
                pl->ang_y = Lb_stick_dir_set(pl, 0);
            }
            Lb_Pl_act_set(pl, 0, 2, 0);
        }
        break;
    case 5:
        a = pl->flag15;
        if (a != 0x1F && a != 0x3F && a != 0x13 && a != 0x24) {
            if (pl->id == game_w.master) {
                pl->ang_y = Lb_stick_dir_set(pl, 0);
            }
            un = ((u16)pl->ang_y + 0x10000 - pl->ang[1]) & 0xFFFF;
            if (un >= 0x6000 && un < 0xA001) {
                Lb_Pl_act_set(pl, 0, 0x3F, 0);
            } else {
                Lb_Pl_act_set(pl, 0, 0x1F, 0);
            }
        }
        break;
    }
    if (PLU8(pl, 0x8C4) != 0) {
        Lb_Pl_chat_act_set(pl);
    }
    if (pl->id != game_w.master) {
        return;
    }
    if ((pl->sw.trg & 0x20) && lb_sys.x68 == 0 && trade_get_ck_005D0750(pl) == 1) {
        return;
    }
    tgt = (u8 *)pl->x3B0;
    if (tgt != 0 && (pl->sw.trg & 0x20)) {
        if (tgt[0x1E] == 0 && lb_sys.x68 == 0) {
            lb_sys.x6C = 0;
            lb_sys.x68 = 4;
            Lb_pl_status_i();
            goto block_119;
        }
        if (lb_sys.x68 != 0) {
            goto block_119;
        }
        em = tgt + 0x444;
        if (*(s8 *)(tgt + 0x471) == 1 || lb_sys.x87 != 0) {
            return;
        }
        *(PLW **)(tgt + 0x7A0) = pl;
        flMemset(lb_pit, 0, 0xC);
        if (tgt[2] == 0) {
            a = em[0xE];
            n = (s8)npc_sound_tbl[a];
            if (n != -1 && a < 0x3B) {
                sound_call_005D3640(tgt, n);
            }
        }
        if (em[0xE] == 0x54) {
            s16 k = game_w.stage - 0x51;
            if (k < 0 || k > 0x55) {
                k = 0;
            }
            if (tgt[0x15] != 0x8C && (&lb_sys.x87)[1 + k] == 0) {
                Lb_act_set(tgt, 0, 0x64);
            }
        } else {
            a = tgt[0x15];
            if (a != 0x79 && a != 0x7A) {
                Lb_act_set(tgt, 0, 0x64);
            }
        }
        PLU8(pl, 0x8EC) = 0;
        switch (em[0xE]) {
        case 0x4B:
        case 0x48:
        case 0x4:
        case 0x55:
            lb_sys.x6C = 1;
            a = tgt[0x15];
            if (a == 0x79 || a == 0x7A) {
                lb_sys.x68 = 6;
                Lb_act_set(tgt, 0, 0x64);
            } else {
                lb_sys.x68 = 5;
            }
            NPCZoomInCameraRequest(tgt);
            goto block_119;
        case 0x47:
        case 0x0:
            lb_sys.x6C = 1;
            lb_sys.x68 = 2;
            if (em[0xE] == 0) {
                Lb_act_set(tgt, 0, 0x65);
            }
            lb_sys.x06 = 0;
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x1:
        case 0x2D:
            lb_sys.x6C = 1;
            lb_sys.x68 = 3;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x38:
            lb_sys.x6C = 1;
            lb_sys.x68 = 0x22;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x3:
            lb_sys.x6C = 1;
            lb_sys.x68 = 9;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x4C:
            lb_sys.x6C = 1;
            lb_sys.x68 = 0xA;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x49:
            lb_sys.x6C = 1;
            lb_sys.x68 = 0xB;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x5:
            if (Event_flag_ck(0x51) == 0 && Event_flag_ck(0x33) == 1) {
                lb_sys.x6C = 1;
                lb_sys.x68 = 0x2B;
                NPCZoomInCameraRequest(tgt);
            } else {
                lb_sys.x6C = 1;
                lb_sys.x68 = 0xC;
                Lb_shop_init();
                NPCZoomInCameraRequest(tgt);
            }
            return;
        case 0x2:
        case 0x2E:
            lb_sys.x6C = 1;
            lb_sys.x68 = 0xD;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            return;
        case 0x4A:
        case 0x3A:
            lb_sys.x6C = 1;
            lb_sys.x68 = 0xE;
            Lb_shop_init();
            NPCZoomInCameraRequest(tgt);
            goto block_119;
        case 0x54: {
            s16 k = game_w.stage - 0x51;
            if (k < 0 || k > 0x55) {
                k = 0;
            }
            if ((&lb_sys.x87)[1 + k] == 0) {
                lb_sys.x68 = 0x12;
                lb_sys.x6C = 1;
                NPCZoomInCameraRequest(pl->x3B0);
            }
            return;
        }
        default:
            lb_sys.x6C = 1;
            a = tgt[0x15];
            if (a == 0x79 || a == 0x7A) {
                lb_sys.x68 = 6;
                Lb_act_set(tgt, 0, 0x64);
            } else {
                lb_sys.x68 = 5;
            }
            return;
        }
    }
block_119:
    fh = (u8 *)pl->fish878;
    if (fh != 0 && ((pl->sw.trg & 0x200) || (pl->sw.trg & 0x40)) && (lb_sys.x68 == 0 || lb_sys.x68 == 8)) {
        a = pl->sw.trg & 0x200;
        if ((a != 0 || *(u16 *)(fh + 2) == 0x13 || *(u16 *)(fh + 2) == 0x14) && lb_sys.x87 == 0 && (lb_sys.x68 != 8 || *(u16 *)(fh + 2) == 6)) {
            pos[0] = *(f32 *)(fh + 4);
            pos[1] = *(f32 *)((u8 *)pl->fish878 + 8);
            fh = (u8 *)pl->fish878;
            pos[2] = *(f32 *)((u8 *)pl->fish878 + 0xC);
            ang = *(u16 *)(fh + 0x14);
            switch ((u32) * (u16 *)(fh + 2)) {
            case 0:
            case 2:
            case 3:
            case 4:
            case 16:
            case 17:
            case 21:
            case 24:
            case 25:
            default:
                return;
            case 1:
                a = pl->flag15;
                if (a != 0 && a != 0x55) {
                    return;
                }
                n = *(u16 *)fh;
                if (!(lb_sys.chair_mask & (1 << n))) {
                    if (game_w.stage == 0x4D) {
                        lb_sys.x66 = n;
                        if (memcmp(cw + 3, cw + 0x440, 8) == 0) {
                            lb_sys.x74 = 0x96;
                            lb_sys.x68 = 0x18;
                            lb_sys.x6C = 1;
                            Lb_send_data_to_myself(2, 4, pl->fish878);
                        } else {
                            lb_sys.x74 = 0x96;
                            lb_sys.x68 = 0x18;
                            lb_sys.x6C = 1;
                            Lb_send_chair_req(pl);
                        }
                    } else {
                        lb_sys.x66 = n;
                        Lb_pl_to_chair();
                    }
                }
                return;
            case 26:
                if (cw[0x35D6] == 0) {
                    lb_sys.x66 = *(u16 *)fh;
                    lb_sys.x68 = 0x11;
                    Lb_pl_to_chair();
                }
                return;
            case 5:
                switch (game_w.stage) {
                case 0x4D:
                    if (cw[0x35D3] == 1) {
                        Lb_put_set01(1);
                    } else {
                        pl->x73A = 0x4C;
                        lb_sys.x03 = 5;
                        lb_sys.x68 = 0x14;
                        lb_sys.x71 = 0;
                    }
                    return;
                case 0x4E:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x4C;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 1;
                default:
                    return;
                case 0x4F:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x4C;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 3;
                    break;
                case 0x50:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x4C;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 4;
                    break;
                case 0x51:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x50;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 1;
                    break;
                case 0x52:
                case 0x53:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x50;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 2;
                    break;
                case 0x54:
                case 0x55:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x50;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 3;
                    break;
                case 0x56:
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x57;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 0;
                }
                return;
            case 6:
                switch (game_w.stage) {
                case 0x4D:
                    if (cw[0x35D3] != 0) {
                        a = cw[0x32C5];
                        if (a == 1) {
                            if (lb_sys.x68 != 7 && lb_sys.x68 != 8) {
                                ri = Lbs_GetRoomInfo((s16)(ClassInfo[8] - 1));
                                if ((*(u16 *)(cw + 0x32C6) + 1) >= (s32) * (u16 *)(ri + 2)) {
                                    SetDialogData(0x36, 2);
                                    SetDialogYesNo(0);
                                    *(s8 *)0x3F36AB = 0;
                                    lb_sys.x68 = 0x30;
                                    lb_sys.x6C = 1;
                                } else {
                                    SetDialogData(0x34, 2);
                                    SetDialogYesNo(1);
                                    *(s8 *)0x3F36AB = 0;
                                    lb_sys.x68 = 0x1D;
                                    lb_sys.x6C = 1;
                                }
                            }
                        } else if (lb_sys.x68 == 8) {
                            lb_sys.x6C = 1;
                            lb_sys.x68 = 0x28;
                            lb_sys.x06 = 0;
                            *(s8 *)0x3F36AB = 0;
                            SetDialogData(0x35, 2);
                            SetDialogYesNo(1);
                        } else {
                            lb_sys.x68 = 0x1A;
                            pm = D_3E5506 + game_w.master * 0xA00;
                            *pm |= 0x20;
                            *pm = *pm & ~0x10;
                            Lbc_SendMiniData(pm);
                            Lb_put_chat(0xD);
                        }
                    } else {
                        cnWrap_SoundRequest(7);
                    }
                    return;
                case 0x57:
                    if (cw[0x35D3] != 0) {
                        lb_sys.x68 = 0x1E;
                        fade_set(1);
                    }
                    return;
                case 0x4E:
                    pl->x73A = 0x4C;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 2;
                    lb_sys.x68 = 0x14;
                }
                return;
            case 18:
                if (Event_flag_ck(4) == 1) {
                    lb_sys.x68 = 0x14;
                    pl->x73A = 0x51;
                    lb_sys.x03 = 5;
                    lb_sys.x71 = 0;
                    n = User_data[0x3E8];
                    if (n < 0x96) {
                        User_data[0x3E8] = n + 1;
                    }
                }
                return;
            case 19:
                if (Event_flag_ck(4) == 1) {
                    if (pl->sw.trg & 0x200) {
                        lb_goto_guest_room(pl, 0x52);
                    } else {
                        lb_goto_guest_room(pl, 0x53);
                    }
                }
                return;
            case 20:
                if (Event_flag_ck(4) == 1) {
                    if (pl->sw.trg & 0x200) {
                        lb_goto_guest_room(pl, 0x54);
                    } else {
                        lb_goto_guest_room(pl, 0x55);
                    }
                }
                return;
            case 7:
                lb_sys._pad73 = 0;
                lb_pit[8] = 0;
                lb_sys.x6C = 1;
                lb_sys.x68 = 1;
                return;
            case 8:
                lb_sys.x68 = 0x14;
                pl->x73A = 0x4D;
                lb_sys.x03 = 5;
                lb_sys.x71 = 0;
                break;
            case 11:
                lb_sys.x68 = 0x14;
                pl->x73A = 0x4F;
                lb_sys.x03 = 5;
                lb_sys.x71 = 0;
                break;
            case 12:
                if (Online_ck() == 0 && cw[0x35D3] == 1) {
                    Lb_put_set01(1);
                } else {
                    lb_sys.x68 = 0x14;
                    if (game_w.stage == 0x4C) {
                        pl->x73A = 0x50;
                    } else {
                        pl->x73A = 0x56;
                    }
                    lb_sys.x71 = 0;
                    lb_sys.x03 = 5;
                }
                break;
            case 9:
                lb_sys.x68 = 0x14;
                pl->x73A = 0x4E;
                lb_sys.x03 = 5;
                lb_sys.x71 = 0;
                break;
            case 10:
                lb_sys.x68 = 0x14;
                pl->x73A = 0x4E;
                lb_sys.x03 = 5;
                lb_sys.x71 = 1;
                break;
            case 13:
                lb_sys.x68 = 0x10;
                fade_set(0xA);
                break;
            case 14:
                flvecCopy(&pl->work800, pos);
                rot[1] = ang & 0xFFFF;
                rot[0] = 0;
                rot[2] = 0;
                cpRotMatrix(rot, mat);
                SetVector(off, 0, 0, 0);
                flvecApplyMat33(vec, off, mat);
                pl->work800 += vec[0];
                pl->work804 += vec[1];
                pl->work808 += vec[2];
                pl->ang_y = ang;
                Lb_Pl_adj_calc(pl, 0x14);
                Lb_Pl_act_set(pl, 0, 0x33, 0);
                pl_flag_set(pl, 0x20000);
                *(s8 *)0x3F36AB = 0;
                lb_sys.x68 = 0x13;
                break;
            case 23:
                if (Event_flag_ck(1) == 0) {
                    flvecCopy(&pl->work800, pos);
                    rot2[1] = ang & 0xFFFF;
                    rot2[0] = 0;
                    rot2[2] = 0;
                    cpRotMatrix(rot2, mat2);
                    SetVector(off2, 0, 0, 0);
                    flvecApplyMat33(vec2, off2, mat2);
                    pl->work800 += vec2[0];
                    pl->work804 += vec2[1];
                    pl->work808 += vec2[2];
                    pl->ang_y = ang;
                    Lb_Pl_adj_calc(pl, 0x14);
                    if (Event_flag_ck(3) == 1) {
                        if ((Warehouse_search_space(User_data) & 0xFF) == 0xFF) {
                            Lb_put_set01(8);
                        } else {
                            LegendSwordCameraRequest(8);
                            Lb_Pl_act_set(pl, 0, 0x52, 0);
                            pl_flag_set(pl, 0x20000);
                            Event_flag_set(1);
                            Warehouse_equip_stack(User_data, 6, 0xBE, 0);
                            Lb_reset_talk_count();
                            lb_sys.x68 = 0x26;
                        }
                    } else {
                        Lb_Pl_act_set(pl, 0, 0x51, 0);
                        pl_flag_set(pl, 0x20000);
                        lb_sys.x68 = 0x26;
                    }
                }
                break;
            case 15:
                if (Lb_ItemBox_open() == 1) {
                    lb_sys.x6C = 1;
                    lb_sys.x68 = 0x1B;
                    Lbc_set_prim(0, 0, 0);
                } else {
                    cnWrap_SoundRequest(7);
                }
                break;
            case 22:
                if (lb_sys.x87 == 0) {
                    lb_sys.x06 = 0;
                    lb_sys.x6C = 1;
                    lb_sys.x68 = 0x27;
                }
                break;
            }
        }
    }
    if (PLU8(pl, 0x908) != 0) {
        PLU8(pl, 0x90B) = 1;
        Lb_Pl_act_set2(pl, 0, 0x2D, 0x10);
        Lb_send_trade_start(pl);
    }
}
