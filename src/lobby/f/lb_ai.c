/* Lobby: player name tags (lb_disp_name) and the help/price bar (Lb_put_help), hand-written from m2c drafts. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 lb_quest_color_tbl[];
extern u8 room_price[];
extern u8 D_3C738C[];
extern char lit_427_00664C20[];
extern char lit_428_00664C30[];
extern char lit_429_00664C38[];
extern char lit_430_00664C40[];
extern char *lb_rule_msg_etc[];
void font_set_stack_no();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void flmatInit();
void flvecrRotTransPers();
void flfntLocate();
void flfntSetSize();
void font_print_double();
void font_set_palette();
int Lb_Pl_stg_ck();
int Lb_get_pl_stat2();
int Get_weapon_job();
void Lb_put_job();
void Lb_put_icon_free();
void Lb_put_status();
int strlen();
int sprintf(char *, const char *, ...);
int Online_ck();
void Lb_put_new_mail();
int SoftKeyboard_alive_check();
int Lb_check_hotel();
void Lb_put_gold();
void lb_put_sprite();
void lb_disp_name(u8 *arg0) {
    f32 v[3];
    f32 scr[3];
    u8 mat[0x40];
    u8 *p;
    u8 *pe;
    PLW *pl;
    int i;
    int off;
    int len;
    int half;
    f32 w;
    s16 x;
    s16 y;
    s16 px;
    s16 py;
    int pal;
    u8 *c;
    u8 f;
    char *name;
    pe = (u8 *)lb_player;
    p = (u8 *)lb_player;
    if (lb_sys.x68 != 8 && lb_sys.x68 != 0xF && lb_sys.x68 != 0) {
        return;
    }
    font_set_stack_no(*(s32 *)(arg0 + 0x18), lb_sys.x68);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    i = 0;
    off = 0;
    do {
        pl = *(PLW **)pe;
        if (*(u8 *)pl != 0 && *((u8 *)pl + 1) != 0 && (Lb_Pl_stg_ck(pl) & 0xFF) && Lb_get_pl_stat2((s8)i) == 0 && (pl->pos[0] != 0.0f || pl->pos[2] != 0.0f)) {
            name = (char *)p + 4;
            if (Online_ck() == 1 && *(u8 *)0x39DAD4 != 0) {
                name = (char *)p + 0x24;
            }
            len = (s16)strlen(name);
            flmatInit(mat);
            flSetRenderState(0x1A, mat);
            v[0] = pl->pos[0];
            v[1] = 190.0f + pl->pos[1];
            v[2] = pl->pos[2];
            flvecrRotTransPers(scr, v);
            if (scr[0] < 700.0f && !(scr[0] <= -60.0f)) {
                if (scr[1] < 500.0f && !(scr[1] <= -20.0f) && !(scr[2] <= 0.0f)) {
                    half = (s16)len >> 1;
                    w = (f32)(half * 8);
                    scr[0] -= w;
                    flfntLocate((int)(1.25f * w), (int)scr[1]);
                    px = (int)(1.25f * scr[0]);
                    py = (int)scr[1];
                    flfntSetSize(0x10, 0x10);
                    c = cw + off;
                    f = c[0x1347];
                    if (f == 0x14) {
                        pal = 2;
                    } else if (f >= 0xD) {
                        pal = 6;
                    } else {
                        pal = 5;
                    }
                    font_set_stack_no(0);
                    font_print_double((int)(1.25f * scr[0]), (int)scr[1], 1, pal);
                    if ((s16)i == game_w.master) {
                        if ((s8)Get_weapon_job(D_3C738C) == 5) {
                        }
                        x = px;
                        y = py;
                        Lb_put_job((s16)(x - 0x16), (s16)(y - 4), 0x16, -1);
                    } else {
                        x = px;
                        y = py;
                        Lb_put_job((s16)(x - 0x16), (s16)(y - 4), 0x16, -1);
                    }
                    f = c[0x1346 + 0x15];
                    if (f & 0xC0) {
                        if (!(f & 0x40)) {
                            if (*(u16 *)0x39B1F4 & 0x10) {
                                goto b37;
                            }
                        } else {
b37:
                            Lb_put_icon_free((s16)(x + len * 4 - 0xC), (s16)(y - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (f & 0xF) * 4));
                        }
                    } else if (f & 0x30) {
                        if ((f & 0x10) || (*(u16 *)0x39B1F4 & 0x10)) {
                            Lb_put_icon_free((s16)(x + len * 4 - 0xC), (s16)(y - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (f & 0xF) * 4));
                        }
                    } else {
                        Lb_put_status((s16)(x + len * 8 + 6), (s16)(y - 4), 0x14, -1);
                    }
                }
            }
        }
        p += 0x38;
        i = (s16)(i + 1);
        pe += 0x38;
        off += 0x2FC;
    } while (i < 8);
}
void Lb_put_help(u8 *arg0) {
    PLW *pl;
    u8 *e;
    u16 t;
    int v;
    pl = &player_work[game_w.master];
    if (lb_sys.x68 != 8 && lb_sys.x68 != 7 && lb_sys.x68 != 0x27 && lb_sys.x68 != 0) {
        return;
    }
    if (Online_ck() == 1) {
        Lb_put_new_mail(0x1A2, 0x1C);
    }
    font_set_stack_no(*(s32 *)(arg0 + 0x18));
    font_set_palette(0);
    flfntSetSize(0x14, 0x14);
    if ((s8)SoftKeyboard_alive_check() == 0) {
        flfntSetSize(0x14, 0x14);
        if (*((u8 *)&lb_sys + 0xA) == 0) {
            if (lb_sys.x68 == 8) {
                goto b11;
            }
            goto b83;
        }
b11:
        e = (u8 *)pl->fish878;
        if (e != 0) {
            t = *(u16 *)(e + 2);
            switch (t) {
            case 19:
                if (pl->flag14 != 1) {
                    v = Lb_check_hotel(0x52);
                    switch (v) {
                    case 0:
                        *((u8 *)&lb_sys + 0xA) = 0;
                        break;
                    case 3:
                    case 1:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate((s16)(strlen((char *)&lb_sys + 0xA) * 0xA + 0x50), 0x1F);
                        font_print(lit_427_00664C20, *(void **)(room_price + 4));
                    case 2:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x1F);
                        font_print(lit_428_00664C30, (char *)&lb_sys + 0xA);
                        Lb_put_gold();
                        break;
                    }
                    v = Lb_check_hotel(0x53);
                    switch (v) {
                    case 0:
                        *((u8 *)&lb_sys + 0x28) = 0;
                        break;
                    case 3:
                    case 1:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate((s16)(strlen((char *)&lb_sys + 0x28) * 0xA + 0x50), 0x39);
                        font_print(lit_427_00664C20, *(void **)(room_price + 8));
                    case 2:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x39);
                        font_print(lit_428_00664C30, (char *)&lb_sys + 0x28);
                        break;
                    }
                    goto b83;
                }
                break;
            case 20:
                if (pl->flag14 != 1) {
                    v = Lb_check_hotel(0x54);
                    switch (v) {
                    case 0:
                        *((u8 *)&lb_sys + 0xA) = 0;
                        break;
                    case 3:
                    case 1:
                        flfntLocate((s16)(strlen((char *)&lb_sys + 0xA) * 0xA + 0x50), 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, *(void **)(room_price + 0xC));
                    case 2:
                        flfntLocate(0x30, 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, (char *)&lb_sys + 0xA);
                        Lb_put_gold();
                        break;
                    }
                    v = Lb_check_hotel(0x55);
                    switch (v) {
                    case 0:
                        *((u8 *)&lb_sys + 0x28) = 0;
                        break;
                    case 3:
                    case 1:
                        flfntLocate((s16)(strlen((char *)&lb_sys + 0x28) * 0xA + 0x50), 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, *(void **)(room_price + 0x10));
                    case 2:
                        flfntLocate(0x30, 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, (char *)&lb_sys + 0x28);
                        break;
                    }
                    goto b83;
                }
                break;
            case 18:
                if (pl->flag14 != 1) {
                    if (Lb_check_hotel(0x51) == 0) {
                        *((u8 *)&lb_sys + 0x28) = 0;
                        return;
                    }
                    flfntSetSize(0x14, 0x14);
            default:
                    flfntLocate(0x30, 0x1F);
                    if (lb_sys.x68 != 8) {
                        if (lb_sys.x68 == 0x28) {
                            goto b69;
                        }
                        if (pl->flag14 != 1) {
                            font_print(lit_428_00664C30, (char *)&lb_sys + 0xA);
                            goto b79;
                        }
                    } else {
b69:
                        if (*(u16 *)((u8 *)pl->fish878 + 2) == 6) {
                            if (cw[0x32C5] != 0) {
                                goto b72;
                            }
                            font_set_palette(0);
                            font_print(lit_429_00664C38, lb_rule_msg_etc[0x2C / 4]);
                            goto b79;
                        }
b72:
                        if (pl->flag14 != 1) {
                            font_set_palette(6);
                            font_print(lit_429_00664C38, lb_rule_msg_etc[0x1C / 4]);
b79:
                            sprintf((char *)&lb_sys + 0x28, lit_430_00664C40);
                            goto b83;
                        }
                    }
                }
                break;
            }
        } else {
            if (lb_sys.x68 == 8) {
                flfntLocate(0x30, 0x1F);
                font_set_palette(6);
                font_print(lit_429_00664C38, lb_rule_msg_etc[0x1C / 4]);
            }
b83:
            lb_put_sprite();
        }
    }
}
