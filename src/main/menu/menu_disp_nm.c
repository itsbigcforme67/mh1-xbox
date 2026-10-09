/* menu_disp_nm - f_menu display half (0x0012B690-0x00134950, main.bin): written
 * from the asm, near-match C not built; matching runs are built as menuNN.c.
 * Field meanings and struct names are guesses. */
#include "menu.h"
#include "em.h"
#include "pl.h"
#include "fl.h"

#define FX(p, o) (*(f32 *)((u8 *)(p) + (o)))
extern u8 quest_w[];
extern u8 room_member_id[];
extern char *item_str[];
extern char *enemy_name[];
extern char *map_name[];
extern char *quest_condition_str[];
extern u8 lb_no_player[];
extern u8 PitMenuB[];

void flfntSetSize(int, int);
void flfntLocate(int, int);
void font_set_palette(int);
void font_print_uf(void *, ...);
void font_print(void *, ...);
int sprintf(char *, const char *, ...);
u32 strlen(const char *);
int Pl_stg_ck(PLW *);
int Online_ck();
void flSetRenderState(int, u32);
void flvecrRotTransPers(f32 *, f32 *);
void SetTrnslMode(int, int);
void SetFilterMode(int);
void SetTextureStage(int);
void reload_tex(int, int);
void DispFrameList(void *, void *, int, ...);
void DispFrameListOptionArrow(void *);
void DispFrameMessage(void *, void *);
void ItemListWindow(int, u32, int);
void PutArrow(int, int, int, int);
char *Quest_str_get(int);
u32 Quest_time_get(int);
int Share_item_num_ck(u16, int);
int Item_ok_chk(int, void *);
int item_present_chk();
int func_5D8370(s8);
void func_5CB310(u8);
int Item_preparation_rate_0(void *, int);
void player_name_id_print(void *);
void font_print_quest_money(int);
void font_print_quest_time(int);
void font_print_quest_lv(int);
void font_print_quest_target(int, int);
void font_print_Bdragon(void);
void font_print_BBQquest(void *, int);
void quest_condition_print(u8);
void disp_item_list_present(int, void *, void *);
void put_mix_material(u8, u16);
extern u8 pfl_quest[];
extern u8 lit_2124[], lit_2125[], lit_2126[], lit_2127[], lit_2128[];
extern u8 lit_2155[], lit_2176[], lit_2222[], lit_2242[], lit_2264[], lit_2265[];
extern u8 lit_2328[], lit_2329[], lit_2330[], lit_2331[], lit_2332[];
extern u8 lit_2439[], lit_2538[], lit_2539[], lit_2540[], lit_2541[], lit_2542[];
extern u8 lit_2543[], lit_2544[], lit_2545[], lit_2546[], lit_2567[];
extern u8 quest_str[][0x34];
extern u8 item_cmd_frame[], item_cmd_str[], item_yn_frame[], item_cmd_present[];
extern u8 item_cmd_item_num[], D_6EAC80[];
extern u8 frame_matA[], frame_matB[], frame_mix_cmd[], frame_mixed_item[];

/* 0x12B690 */
void disp_name(void) {
    PLW *pl;
    int i;
    f32 pos[3];
    f32 scr[3];
    char *name;
    u8 mat[0x40];

    flfntSetSize(0x10, 0x10);
    font_set_palette(0);
    pl = player_work;
    for (i = 0; i < 4; i++, pl++) {
        if (*(u16 *)((u8 *)pl + 0xC) != game_w.master && game_w.pl_state[i] == 1 &&
            *(u8 *)pl != 0 && *((u8 *)pl + 1) != 0 && (u8)Pl_stg_ck(pl)) {
            flmatInit((void *)mat);
            flSetRenderState(0x1A, (u32)mat);
            pos[0] = *(f32 *)((u8 *)pl + 0xAC);
            pos[1] = *(f32 *)((u8 *)pl + 0xB0) + 200.0f;
            pos[2] = *(f32 *)((u8 *)pl + 0xB4);
            flvecrRotTransPers(scr, pos);
            if (scr[0] < 700.0f && scr[0] > -60.0f) {
                if (scr[1] < 500.0f && scr[1] > -20.0f) {
                    name = (char *)pl + 0x8D4;
                    if (scr[2] > 0.0f) {
                        if (Online_ck() == 1 && PitMenu.x14 != 0) {
                            name = (char *)room_member_id + *(u16 *)((u8 *)pl + 0xC) * 8;
                        }
                        scr[0] -= (s16)strlen(name) / 2 * 8;
                        flfntLocate((int)(1.25f * scr[0]), (int)scr[1]);
                        font_print_uf(name);
                    }
                }
            }
        }
    }
}

/* 0x12B900 */
void Pit_disp_quest(void) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */

    SetTrnslMode(4, 5);
    if (lpPit->x46 == 0) {
        *(void **)(pfl_quest + 0xC) = 0;
        DispFrameList(pfl_quest, lit_2124, -1);
        flfntLocate(0x167, 0xB6);
        font_print_uf(lit_2125);
        return;
    }
    if (lpPit->x43 == 0) {
        sprintf(buf, (char *)lit_2126);
    } else {
        sprintf(buf, (char *)lit_2127, lpPit->x43 + 1);
    }
    *(void **)(pfl_quest + 0xC) = quest_str[lpPit->x43];
    DispFrameList(pfl_quest, buf, -1, lpPit->x43);
    DispFrameListOptionArrow(pfl_quest);
    font_set_palette(0);
    flfntLocate(0x167, 0x66);
    font_print_uf(Quest_str_get(0));
    switch (lpPit->x43) {
    case 0:
        if (game_w.x1DC == 0) {
            quest_condition_print(lpPit->x43);
        } else {
            func_5CB310(lpPit->x43);
        }
        return;
    case 1:
        flfntLocate(0x1C1, 0xA2);
        font_print_quest_money(*(s32 *)(*(u8 **)(quest_w + 0x94) + 8));
        flfntLocate(0x1C1, 0xB6);
        font_print_quest_money(*(s32 *)(*(u8 **)(quest_w + 0x94) + 4));
        flfntLocate(0x1C1, 0xDE);
        font_print_quest_time(1);
        flfntLocate(0x1C1, 0xF2);
        font_print_uf(map_name[*(s32 *)(*(u8 **)(quest_w + 0x94) + 0x14)]);
        flfntLocate(0x167, 0x12E);
        if ((*(u8 **)(quest_w + 0x94))[3] == 0) {
            font_print_uf(lit_2128);
        }
        return;
    case 2:
        flfntLocate(0x1E5, 0xA2);
        font_print_quest_lv((*(u8 **)(quest_w + 0x94))[2]);
        flfntLocate(0x167, 0xDE);
        font_print_uf(Quest_str_get(1));
        flfntLocate(0x167, 0x12E);
        font_print_uf(Quest_str_get(2));
        return;
    case 3:
        flfntLocate(0x167, 0xB6);
        font_print_uf(Quest_str_get(3));
        break;
    }
}

/* 0x12BBC0 */
void font_print_quest_time(int n) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u16 out[12];
    u16 *o;
    char *p;
    int t, m, s;

    t = Quest_time_get((u8)n);
    t = t / 30;
    m = t / 60;
    s = t - m * 60;
    sprintf(buf, (char *)lit_2155, m, s);
    o = out;
    for (p = buf; *p != 0; p++, o++) {
        if (*p != 0x3A) {
            *o = ((*p + 0x1F) << 8) | 0x82;
        } else {
            *o = 0x4681;
        }
    }
    *o = 0;
    font_print_uf(out);
}

/* 0x12BCA0 */
void font_print_quest_money(int n) {
    u16 out[32];
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u16 *o;
    char *p;

    sprintf(buf, (char *)lit_2176, n);
    o = out;
    for (p = buf; *p != 0; p++, o++) {
        *o = ((*p + 0x1F) << 8) | 0x82;
    }
    o[0] = 0x9A82;
    o[1] = 0;
    font_print_uf(out);
}

/* 0x12BD10 */
void font_print_quest_lv(int n) {
    u16 out[12];
    u16 *o = out;

    while (n != 0) {
        *o++ = 0x9A81;
        n--;
    }
    *o = 0;
    font_print_uf(out);
}

/* 0x12BD60 */
void font_print_quest_target(int a, int b) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u16 out[12];
    u16 *o;
    char *p;

    sprintf(buf, (char *)lit_2222, (u16)a, (u16)b);
    o = out;
    for (p = buf; *p != 0; p++, o++) {
        if (*p == 0x2F) {
            *o = 0x5E81;
        } else if (*p == 0x20) {
            *o = 0x4081;
        } else {
            *o = ((*p + 0x1F) << 8) | 0x82;
        }
    }
    *o = 0;
    font_print_uf(out);
}

/* 0x12BE00 */
void font_print_Bdragon(void) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u16 out[12];
    u16 *o;
    char *p;

    sprintf(buf, (char *)lit_2176, (10 - game_w.x21D) * 10);
    o = out;
    for (p = buf; *p != 0; p++, o++) {
        *o = ((*p + 0x1F) << 8) | 0x82;
    }
    *o = 0;
    font_print(lit_2242, out);
}

/* 0x12BE90 */
void font_print_BBQquest(void *pl, int y) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u16 out[12];
    u16 *o;
    char *p;

    flfntLocate(0x167, y);
    player_name_id_print(pl);
    flfntLocate(0x1F7, y);
    sprintf(buf, (char *)lit_2264, (s16)Share_item_num_ck(*(u16 *)(quest_w + 0x1C), *(u16 *)((u8 *)pl + 0xC) + 2));
    o = out;
    for (p = buf; *p != 0; p++, o++) {
        *o = ((*p + 0x1F) << 8) | 0x82;
    }
    *o = 0;
    font_print(lit_2265, out);
}

/* 0x12BF50 */
void quest_condition_print(u8 unused) {
    font_set_palette(5);
    if (lpPit->x47 != 4) {
        flfntLocate(0x167, 0xF2);
        font_print_uf(lit_2328);
        flfntLocate(0x167, 0x106);
        font_print_uf(lit_2329);
    }
    flfntLocate(0x167, 0x12E);
    font_print_uf(lit_2330);
    font_set_palette(0);
    if (lpPit->x47 != 4) {
        flfntLocate(0x1C1, 0xF2);
        font_print_quest_money(*(s32 *)(*(u8 **)(quest_w + 0x94) + 8));
        flfntLocate(0x1C1, 0x106);
        font_print_quest_money(*(s32 *)(quest_w + 0x14));
    }
    flfntLocate(0x1C1, 0x12E);
    font_print_quest_time(0);
    font_set_palette(5);
    flfntLocate(0x167, 0xA2);
    font_print_uf(quest_condition_str[lpPit->x47]);
    font_set_palette(0);
    switch (lpPit->x47) {
    case 0: {
        u32 i = 0;
        int y = 0xB6;
        u8 *q = quest_w;
        for (; i < 2; i++, q += 2) {
            if (*(s16 *)(q + 0x144) != 0) {
                flfntLocate(0x167, y);
                font_print(lit_2331, enemy_name[*(s16 *)(q + 0x144)]);
                flfntLocate(0x209, y);
                font_print_quest_target((u16)(*(s16 *)(q + 0x148) - *(s16 *)(q + 0x30)), (u16)*(s16 *)(q + 0x148));
                y = (s16)(y + 0x14);
            }
        }
        return;
    }
    case 1: {
        u32 i = 0;
        int y = 0xB6;
        u8 *q = quest_w;
        for (; i < 4; i++, q += 2) {
            if (*(s16 *)(q + 0x1C) != 0) {
                flfntLocate(0x167, y);
                font_print(lit_2331, item_str[*(s16 *)(q + 0x1C)]);
                flfntLocate(0x209, y);
                font_print_quest_target((u16)Share_item_num_ck(*(s16 *)(q + 0x1C), 0), *(u16 *)(q + 0x24));
                y = (s16)(y + 0x14);
            }
        }
        return;
    }
    case 2:
        flfntLocate(0x167, 0xB6);
        font_print(lit_2331, enemy_name[*(s16 *)(quest_w + 0x2C)]);
        flfntLocate(0x209, 0xB6);
        font_print_quest_target((u16)Share_item_num_ck(0x90, 0), 1);
        return;
    case 3:
        flfntLocate(0x1D3, 0xA2);
        font_print_Bdragon();
        flfntLocate(0x167, 0xB6);
        font_print_uf(lit_2332);
        return;
    case 4: {
        PLW *pl = player_work;
        int y = 0xCA;
        u32 i = 0;
        flfntLocate(0x167, 0xB6);
        font_print_BBQquest(lpPit->pl, 0xB6);
        for (; i < 4; i++, pl++) {
            if (i != game_w.master && game_w.pl_state[i] == 1) {
                font_print_BBQquest(pl, y);
                y = (s16)(y + 0x14);
            }
        }
        return;
    }
    }
}

/* 0x12C310 */
void Pit_disp_item_list(void) {
    int col, sel;
    void *msg;

    switch (lpPit->x48) {
    case 0:
        sel = 8;
        col = 0xA9182;
        break;
    case 1:
        sel = 0;
        if (Item_ok_chk((int)lpPit->pl, lpPit) == 1) {
            if (item_present_chk() == 1) {
                msg = item_cmd_str;
            } else {
                msg = item_cmd_str + 8;
            }
        } else {
            msg = item_cmd_str + 0x10;
        }
        *(void **)(item_cmd_frame + 0xC) = msg;
        *(s32 *)(item_cmd_frame + 0x10) = 0xA9182;
        DispFrameList(item_cmd_frame, 0, lpPit->x4A);
        col = 0x808080;
        break;
    case 2:
        sel = 0;
        switch (lpPit->x4A) {
        case 0:
            *(s32 *)(item_cmd_frame + 0x10) = 0x808080;
            DispFrameList(item_cmd_frame, 0, lpPit->x4A);
            DispFrameList(item_yn_frame, 0, lpPit->yn);
            break;
        case 1:
            disp_item_list_present((int)lpPit->pl, lpPit, &lpPit->x4A);
            break;
        }
        col = 0x808080;
        break;
    }
    ItemListWindow(lpPit->x49, col, sel);
}

/* 0x12C4A0 */
void disp_item_list_present(int unused, void *unused2, void *unused3) {
    char *names[10];
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    int i, n;
    int sel;
    u8 *rm;
    PLW *pl;

    switch (lpPit->x4B) {
    case 1:
        if (game_w.x1DC == 0) {
            n = 0;
            pl = player_work;
            rm = room_member_id;
            for (i = 0; i < 4; i++, pl++, rm += 8) {
                if (game_w.master != i) {
                    if (game_w.pl_state[i] == 1) {
                        names[n] = (char *)pl + 0x8D4;
                        if (Online_ck() == 1 && PitMenu.x14 != 0) {
                            names[n] = (char *)rm;
                        }
                    } else {
                        names[n] = (char *)lb_no_player;
                    }
                    n++;
                }
            }
            names[n] = 0;
        } else {
            u8 *d = D_6EAC80;
            n = 0;
            for (i = 0; i < 8; i++, d += 0x38) {
                if (game_w.master != i) {
                    switch (func_5D8370(i)) {
                    case 0:
                        names[n] = (char *)d + 4;
                        if (Online_ck() == 1) {
                            if (PitMenu.x14 != 0) {
                                names[n] = (char *)d + 0x24;
                            }
                        }
                        break;
                    case 2:
                    case 1:
                        names[n] = (char *)lb_no_player;
                        break;
                    }
                    n++;
                }
            }
            names[n] = 0;
        }
        sel = lpPit->x4C;
        if (game_w.master < sel) {
            sel -= 1;
        }
        *(void **)(item_cmd_present + 0xC + game_w.x1DC * 0x18) = names;
        DispFrameList(item_cmd_present + game_w.x1DC * 0x18, 0, sel, names);
        return;
    case 0:
        sprintf(buf, (char *)lit_2439, lpPit->x4D);
        DispFrameMessage(item_cmd_item_num, buf);
        SetFilterMode(1);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        if (lpPit->x4E == 0) {
            PutArrow(0x18A, 0x3E, 0x18, 0x12);
        }
        if (lpPit->x4D != 1) {
            PutArrow(0x14C, 0x3E, 0x18, 0x12);
        }
        return;
    }
}

/* 0x12C780 */
void Pit_disp_item_mix(void) {
    u32 col;
    int sel;

    SetFilterMode(0);
    SetTrnslMode(4, 5);
    col = 0;
    if (lpPit->x7A < 2) {
        col = 0xA9182;
    }
    if (lpPit->x7A < 3) {
        sel = 0xD;
    } else {
        sel = 5;
    }
    ItemListWindow(lpPit->x49, col, sel);
    if (lpPit->x7A < 3) {
        DispFrameMessage(frame_matA, lit_2538);
        flfntLocate(0x10D, 0x51);
        put_mix_material(*(u8 *)&lpPit->x70, lpPit->x6C);
    }
    if (lpPit->x7A > 0 && lpPit->x7A < 3) {
        DispFrameMessage(frame_matB, lit_2539);
        flfntLocate(0x10D, 0x8C);
        put_mix_material(*((u8 *)&lpPit->x70 + 1), lpPit->x6E);
    }
    if (lpPit->x7A >= 2 && lpPit->x7A < 5) {
        DispFrameMessage(frame_mixed_item, lit_2540);
        font_set_palette(0);
        flfntLocate(0x10D, 0xC7);
        if (lpPit->x74 > 0) {
            font_print_uf(item_str[*(s16 *)((u8 *)lpPit->x68 + 2)]);
        } else {
            font_print_uf(lit_2541);
        }
        flfntLocate(0x167, 0xDA);
        if (lpPit->x74 > 0) {
            font_print(lit_2542, (s8)Item_preparation_rate_0((void *)lpPit->x68, 0));
        } else {
            font_print_uf(lit_2543);
        }
        if (lpPit->x7A == 2) {
            DispFrameList(frame_mix_cmd, 0, lpPit->x7B);
            if (lpPit->x7C == 0) {
                font_set_palette(0);
            } else {
                font_set_palette(0xA);
            }
            flfntLocate(0x131, 0x104);
            font_print_uf(lit_2544);
        }
    }
    if (lpPit->x7A == 5 || lpPit->x7A == 6) {
        if (lpPit->x78 != 0x8F) {
            DispFrameMessage(frame_mixed_item, lit_2545);
        } else {
            DispFrameMessage(frame_mixed_item, lit_2546);
        }
        font_set_palette(0);
        flfntLocate(0x10D, 0xC7);
        font_print_uf(item_str[lpPit->x78]);
    }
}

/* 0x12CA50 */
void put_mix_material(u8 a, u16 id) {
    if (id != 0xFFFF) {
        if (a != 0xFF) {
            font_set_palette(0);
        } else {
            font_set_palette(0xA);
        }
        font_print_uf(item_str[id]);
        return;
    }
    font_set_palette(3);
    font_print_uf(lit_2567);
}

/* ===== map display (0x12CCD0-0x12E3E0) ===== */
extern u8 enemy_icon_tbl[];
extern u8 camp_pos[][8];
extern u32 disp_pl_rgb[];
extern f32 ofs_x_3192[];
extern s16 ofs_y_3193[];
void flps0008(void *);
void flps000C(void *);
void *Stage_data_get(u8);
int enemy_mark_chk(PLW *, EMW *);
void wyvern_area(f32 *, f32 *, f32 *, u8);
void maru_disp_sub(int col, f32 x, f32 y, f32 r);
void camp_disp_sub(f32 x, f32 y);
void enemy_on_map(EMW *em, f32 x, f32 y, f32 scale);
void player_on_map(f32 x, f32 y, f32 scale, u16 ang, u16 id);
void disp_whole_map(int ofs, f32 x, f32 scale);
void disp_partial_map(int ofs, f32 x, f32 scale);
void disp_map_sign(int x, int y, s16 timer, int color);
void wyvn_efct_ripple(void);
void flmatSetZYX33(f32, f32, f32, FLMAT *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
int Pl_item_num_ck(PLW *, int);
typedef struct PFLPS2 {
    s16 s[4];
    u32 col;
    s16 uv[4];
} PFLPS2;
typedef struct PFLP12 {
    s16 p[6];
    u32 col;
    s16 uv[6];
} PFLP12;

/* 0x12D210 */
void player_on_map(f32 x, f32 y, f32 scale, u16 ang, u16 id) {
    PFLP12 q;
    FLMAT mat;
    f32 out[4][3];
    f32 v[3];
    s16 i;
    s16 u;
    s16 u2;
    u32 *rgb;
    s16 sg;
    u32 a;

    x *= 0.8f;
    SetFilterMode(0);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x119);
    SetTextureStage(0x119);
    u = (id == game_w.master) ? 0 : 0x50;
    u2 = u + 0xF;
    flmatInit(&mat);
    a = (0x18000 - ang) & 0xFFFF;
    flmatSetZYX33(0.0f, 0.0f, 2.0f * (3.1415927f * (((360.0f * (f32)a) / 65536.0f) / 360.0f)), &mat);
    v[2] = 0.0f;
    for (i = 0; i < 4; i++) {
        v[0] = -9.0f + 18.0f * (f32)(i / 2);
        v[1] = -9.0f + 18.0f * (f32)(i & 1);
        flvecApplyMat33(out[i], v, &mat);
        out[i][0] = x + scale * (0.8f * out[i][0]);
        out[i][1] = y + scale * out[i][1];
    }
    rgb = &disp_pl_rgb[id & 3];
    q.col = *rgb;
    sg = lpPit->x34 ? 0 : 0;
    sg = *(s16 *)((u8 *)lpPit + id * 2 + 0x34);
    if (sg >= 0 && sg < 100 && sg % 20 == 0) {
        q.col = -1;
    }
    q.p[0] = out[0][0];
    q.p[1] = out[0][1];
    q.p[2] = out[1][0];
    q.p[3] = out[1][1];
    q.p[4] = out[2][0];
    q.p[5] = out[2][1];
    q.uv[0] = u;
    q.uv[1] = 0xF0;
    q.uv[2] = u;
    q.uv[3] = 0xFF;
    q.uv[4] = u2;
    q.uv[5] = 0xF0;
    flps000C(&q);
    q.p[0] = out[3][0];
    q.p[1] = out[3][1];
    q.uv[0] = u2;
    q.uv[1] = 0xFF;
    flps000C(&q);
    if (*(s16 *)((u8 *)lpPit + id * 2 + 0x34) >= 0) {
        SetFilterMode(1);
        SetTrnslMode(4, 1);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        disp_map_sign((int)x, (int)y, *(s16 *)((u8 *)lpPit + id * 2 + 0x34), *rgb);
    }
}

/* 0x12D700 */
void disp_whole_map(int ofs, f32 x0, f32 scale) {
    PFLPS2 q;
    f32 x, y, r;
    f32 sx, sy;
    PLW *me;
    u8 *st;
    f32 lo_x, hi_x, lo_y, hi_y;

    me = lpPit->pl;
    reload_tex(1, 0x119);
    SetTextureStage(0x119);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    sx = 320.0f * lpPit->map_sx * scale;
    sy = 320.0f * lpPit->map_sy * scale;
    SetTrnslMode(4, 1);
    {
    EMW *em = em_work;
    s16 i;
    for (i = 0; i < 20; i++, em++) {
        if (enemy_mark_chk(me, em) != 0 && em->x9E9 != 0) {
            wyvern_area(&x, &y, &r, em->stg);
            x *= scale;
            y *= scale;
            if (r == 0.0f) {
                st = Stage_data_get(em->stg);
                x = sx * (*(f32 *)st + 0.5f * *(f32 *)(st + 0x10));
                r = 32.0f;
                y = sy * (*(f32 *)(st + 4) + 0.5f * *(f32 *)(st + 0x14));
            }
            y += (f32)ofs;
            x += x0;
            maru_disp_sub(0x70C0C010, x, y, r * scale);
        }
    }
    }
    SetTrnslMode(4, 5);
    SetFilterMode(0);
    if (Pl_Skill_ck(me, 0x2E) == 1 || Pl_item_num_ck(me, 0x8E) != 0) {   /* asm: beqz -> the explored-window branch only WITHOUT the map (item 142) */
        q.uv[0] = 0;
        q.uv[1] = 0;
        *(u32 *)&q.uv[2] = 0xF000F0;
        q.s[0] = 0.8f * x0;
        q.s[1] = ofs;
        q.s[2] = 0.8f * (320.0f * scale);
        q.s[3] = 0.8f * (320.0f * scale);
    } else {
        x = 240.0f * (FX(me, 0x754) * lpPit->map_sx);
        lo_x = x - 32.0f;
        hi_x = 32.0f + x;
        y = 240.0f * (FX(me, 0x75C) * lpPit->map_sy);
        if (lo_x < 0.0f) lo_x = 0.0f;
        if (!(hi_x <= 240.0f)) hi_x = 240.0f;
        lo_y = y - 32.0f;
        hi_y = 32.0f + y;
        if (lo_y < 0.0f) lo_y = 0.0f;
        if (!(hi_y <= 240.0f)) hi_y = 240.0f;
        q.uv[0] = lo_x;
        q.uv[1] = lo_y;
        q.uv[2] = hi_x;
        q.uv[3] = hi_y;
        q.s[0] = 0.8f * (x0 + 1.3333334f * lo_x * scale);
        q.s[2] = 0.8f * (scale * (1.3333334f * (hi_x - lo_x)));
        q.s[1] = (s16)ofs + (s16)(((320.0f * lo_y) / 240.0f) * scale);
        q.s[3] = scale * ((320.0f * (hi_y - lo_y)) / 240.0f);
    }
    q.col = 0xC8FFFFFF;
    flps0008(&q);
    x = sx * camp_pos[game_w.x2E][0];
    y = sy * *(f32 *)&camp_pos[game_w.x2E][4];
    x += x0;
    y += (f32)ofs;
    camp_disp_sub(x, y);
    {
    EMW *em = em_work;
    s16 i;
    for (i = 0; i < 20; i++, em++) {
        if (enemy_mark_chk(me, em) != 0) {
            x = em->x754[0] * sx;
            y = em->x754[2] * sy;
            x += x0;
            y += (f32)ofs;
            enemy_on_map(em, x, y, scale);
        }
    }
    }
    {
    PLW *pl = player_work;
    s16 i;
    for (i = 0; i < game_w.pl_num; i++, pl++) {
        if (*(u8 *)pl != 0 && i != game_w.master) {
            x = FX(pl, 0x754) * sx;
            y = FX(pl, 0x75C) * sy;
            x += x0;
            y += (f32)ofs;
            player_on_map(x, y, scale, *(u16 *)((u8 *)pl + 0xA4), *(u16 *)((u8 *)pl + 0xC));
        }
    }
    }
    {
        PLW *p2 = lpPit->pl;
        x = FX(p2, 0x754) * sx;
        y = FX(p2, 0x75C) * sy;
        x += x0;
        y += (f32)ofs;
        player_on_map(x, y, scale, *(u16 *)((u8 *)p2 + 0xA4), *(u16 *)((u8 *)p2 + 0xC));
    }
}

/* 0x12DE00 */
void disp_partial_map(int ofs, f32 x0, f32 scale) {
    PFLPS2 q;
    f32 px, py, lx, ly, w;
    f32 t, t2;
    PLW *me;
    int c;

    reload_tex(1, 0x119);
    SetTextureStage(0x119);
    SetFilterMode(0);
    flSetRenderState(0x60, 0);
    me = lpPit->pl;
    t = 32.0f * scale;
    px = 240.0f * (FX(me, 0x754) * lpPit->map_sx);
    py = 240.0f * (FX(me, 0x75C) * lpPit->map_sy);
    if (px < t) {
        lx = 0.0f;
    } else if (!(px <= 240.0f - t)) {
        lx = 240.0f - 64.0f * scale;
    } else {
        lx = px - t;
    }
    if (py < t) {
        ly = 0.0f;
    } else if (!(py <= 240.0f - t)) {
        ly = 240.0f - 64.0f * scale;
    } else {
        ly = py - t;
    }
    reload_tex(1, 0x119);
    SetTextureStage(0x119);
    q.s[1] = ofs;
    q.col = 0xC8FFFFFF;
    q.s[0] = 0.8f * x0;
    q.s[2] = 256.0f * scale;
    w = 64.0f * scale;
    q.s[3] = 320.0f * scale;
    q.uv[0] = lx;
    q.uv[1] = ly;
    q.uv[2] = lx + w;
    q.uv[3] = ly + w;
    flps0008(&q);
    SetFilterMode(0);
    c = game_w.x2E * 8;
    t2 = (240.0f * (lpPit->map_sx * *(f32 *)((u8 *)camp_pos + c))) - lx;
    if (!(t2 < 0.0f) && t2 <= w) {
        t = (240.0f * (lpPit->map_sy * *(f32 *)((u8 *)camp_pos + 4 + c))) - ly;
        if (!(t < 0.0f) && t <= w) {
            camp_disp_sub(x0 + 5.0f * t2, ofs + 5.0f * t);
        }
    }
    {
        EMW *em = em_work;
        u32 i = 0;
        for (; i < 20; i++, em++) {
            if (enemy_mark_chk(me, em) != 0) {
                t2 = (240.0f * (FX(em, 0x754) * lpPit->map_sx)) - lx;
                if (!(t2 < 0.0f) && t2 <= w) {
                    t = (240.0f * (FX(em, 0x75C) * lpPit->map_sy)) - ly;
                    if (!(t < 0.0f) && t <= w) {
                        enemy_on_map(em, x0 + 5.0f * t2, ofs + 5.0f * t, 2.0f * scale);
                    }
                }
            }
        }
    }
    {
        PLW *pl = player_work;
        u32 i = 0;
        if (game_w.pl_num != 0) {
            do {
                if (*(u8 *)pl != 0 && i != game_w.master) {
                    t2 = (240.0f * (FX(pl, 0x754) * lpPit->map_sx)) - lx;
                    if (!(t2 < 0.0f) && t2 <= w) {
                        t = (240.0f * (FX(pl, 0x75C) * lpPit->map_sy)) - ly;
                        if (!(t < 0.0f) && t <= w) {
                            player_on_map(x0 + 5.0f * t2, ofs + 5.0f * t, 1.0f, *(u16 *)((u8 *)pl + 0xA4), *(u16 *)((u8 *)pl + 0xC));
                        }
                    }
                }
                i++;
                pl++;
            } while (i < game_w.pl_num);
        }
    }
    me = lpPit->pl;
    player_on_map(x0 + 5.0f * (px - lx), ofs + 5.0f * (py - ly), 1.0f, *(u16 *)((u8 *)me + 0xA4), *(u16 *)((u8 *)me + 0xC));
}

/* 0x12E2F0 */
void disp_map(void) {
    s16 ofs;

    if (lpPit->lb == 0 && lpPit->x83 == 0) {
        ofs = (PitMenu.open == 0) ? 0x3C : 0xB6;
        if (lpPit->x3E == 0) {
            disp_whole_map(ofs + ofs_y_3193[game_w.x2E], 400.0f + ofs_x_3192[game_w.x2E], 0.7f);
            wyvn_efct_ripple();
        } else {
            disp_partial_map(ofs, 400.0f, 0.7f);
        }
    }
}

/* ===== item window and gauges (0x12E3E0-0x1306A0) ===== */
extern u8 Item_data[][16];
extern u32 item_col_tbl[];
extern u8 item_normal_base[];
extern u8 btn_item_sel_3262[];
extern u8 btn_item_sel_3617[];
extern u8 lit_3253[], lit_3659[], lit_3660[], lit_3772[];
extern u32 item_stock_color_tbl0[];
extern u32 item_stock_color_tbl1[];
extern u8 pf_item_stock_cmd[], pf_item_stock_full[], pf_item_stock_name[], pf_item_stock_yn[];
int UseItemChk(PLW *, u16);
void PutButtonICON(void *, int);
void PutSpriteDiv3(void *, int, int);
int Item_valid_chk(u16, u8 *, PLW *);
void Disp_help_mess(int, u16);
void Put_shousai(u8, void *);
void player_info_sub(f32, PLW *, s16);
void disp_item_sub_normal(void);
void disp_item_sub_select(void);
int disp_shell_name(u8, int);
void disp_item_icon(u8, s16, int, s8);
typedef struct PFLPS3 {
    s16 s[4];
    u32 col;
    u32 uv0;
    u32 uv1;
} PFLPS3;

/* 0x12E3E0 */
void disp_item(void) {
    PLW *pl = lpPit->pl;
    u16 id;
    u8 *d;
    int w;

    if (*(u8 *)pl == 0) return;
    if (*((u8 *)pl + 1) == 0) return;
    if (lpPit->x83 != 0) return;
    SetFilterMode(1);
    if (pl->work88C == 0) {
        disp_item_sub_normal();
    } else {
        disp_item_sub_select();
    }
    if (UseItemChk(pl, pl->work888) == 1) {
        id = pl->item[pl->work888].id;
        if (pl->item[pl->work888].num > 0) {
            d = &Item_data[id][3];
            if (*d > 1) {
                flfntSetSize(0x12, 0x12);
                flfntLocate(0x212, 0x178);
                font_set_palette(0);
                if (pl->item[pl->work888].num >= *d) {
                    font_set_palette(2);
                }
                font_print(lit_2264, pl->item[pl->work888].num);
            }
        }
        flfntSetSize(0x14, 0x14);
        w = strlen(item_str[pl->item[pl->work888].id]) * 10;
        flfntLocate((int)(419.0f + (186.0f - (f32)w) / 2.0f), 0x18C);
        font_set_palette(0);
        font_print(lit_3253, item_str[id]);
    }
    if (lpPit->x58 > 0) {
        lpPit->x58--;
    }
    if (lpPit->x5C > 0) {
        lpPit->x5C--;
    }
}

/* 0x12E640 */
void disp_item_sub_normal(void) {
    PFLPS3 q;
    PLW *pl = lpPit->pl;
    u8 c;

    DispFrameMessage(item_normal_base, 0);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    q.s[0] = 0x14F;
    q.s[2] = 0x94;
    q.s[1] = 0x18A;
    q.s[3] = 0x18;
    q.col = -1;
    q.uv0 = 0x9A00C5;
    q.uv1 = 0xB000E5;
    PutSpriteDiv3(&q, 0xC, 0xF);
    q.s[0] = 0x170;
    q.s[2] = 0x53;
    q.s[1] = 0x156;
    q.s[3] = 0x36;
    q.uv0 = 0x700000;
    q.uv1 = 0xA60068;
    flps0008(&q);
    q.s[0] = 0x17E;
    q.s[2] = 0x36;
    q.s[1] = 0x147;
    q.s[3] = 0x44;
    q.uv0 = 0x31008F;
    q.uv1 = 0x6700C5;
    flps0008(&q);
    *(s16 *)(btn_item_sel_3262 + 0xA) = 0x173;
    if (pl->work8F2 & 1) {
        *(s16 *)(btn_item_sel_3262 + 0xA) += 2;
    }
    PutButtonICON(btn_item_sel_3262, 2);
    if (UseItemChk(pl, pl->work888) == 1) {
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        disp_item_icon(pl->work888, 0x185, 1, 0);
    }
    c = *((u8 *)pl + 2);
    if (c == 1 || c == 5) {
        q.col = 0xFF808080;
        if (pl->work8BC != 0) {
            flfntSetSize(0x14, 0x14);
            if (disp_shell_name(pl->work88E, 0x132) == 1) {
                q.col = item_col_tbl[Item_data[pl->item[pl->work88E].id][6]];
            }
        }
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q.s[0] = 0x14F;
        q.s[2] = 0x94;
        q.s[1] = 0x130;
        q.s[3] = 0x18;
        q.uv0 = 0x1A00E0;
        q.uv1 = 0x300100;
        PutSpriteDiv3(&q, 0xC, 0xF);
    }
}

/* 0x12F830 */
void disp_item_sub_select_ex(void) {
    PFLPS3 q;
    PLW *pl = lpPit->pl;

    if (lpPit->x57 != 0) {
        *(s16 *)(btn_item_sel_3617 + 2) = 0xDB;
        if (pl->work8F2 & 0x10) {
            *(s16 *)(btn_item_sel_3617 + 2) += 2;
        }
        *(s16 *)(btn_item_sel_3617 + 0xA) = 0x131;
        if (pl->work8F2 & 8) {
            *(s16 *)(btn_item_sel_3617 + 0xA) += 2;
        }
        PutButtonICON(btn_item_sel_3617, 2);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q.s[0] = 0x14B;
        q.s[2] = 0x9C;
        q.s[1] = 0x105;
        q.s[3] = 0x1A;
        q.col = -1;
        q.uv0 = 0xAA0000;
        q.uv1 = 0xC40020;
        PutSpriteDiv3(&q, 9, 0xC);
    }
}

/* 0x12F930 */
int disp_shell_name(u8 slot, int y) {
    char buf[64];
    PLW *pl = lpPit->pl;
    u16 id;
    u8 *d;
    u8 num;
    int w;

    if (slot == 0xFF) return 0;
    id = pl->item[slot].id;
    d = Item_data[id];
    if (d[1] != 2) return 0;
    num = *((u8 *)&pl->item[slot] + 2);
    if (num == 0) return 0;
    if (d[8] != pl->work56D) {
        font_set_palette(5);
    } else {
        font_set_palette(0);
    }
    if (d[3] == 0xFF) {
        sprintf(buf, (char *)lit_3659, item_str[id]);
    } else {
        sprintf(buf, (char *)lit_3660, item_str[id], num);
    }
    w = strlen(buf) * 10;
    flfntLocate((int)(419.0f + (186.0f - (f32)w) / 2.0f), y);
    font_print_uf(buf);
    return 1;
}

/* 0x12FAF0 */
void disp_item_icon(u8 slot, s16 x, int big, s8 side) {
    PFLPS3 q;
    PLW *pl = lpPit->pl;
    u16 id = pl->item[slot].id;
    u8 *d = Item_data[id];
    u8 icon = d[5];
    u8 col;
    s8 mark;
    int a, b, n;

    if (icon != 0xFF) {
        col = d[6];
        mark = 0;
        if (Item_valid_chk(id, d + 5, pl) == 0) {
            col = 10;
            mark = -1;
        } else if (d[4] & 2) {
            mark = 1;
        }
        if (big != 0) {
            q.s[1] = 0x151;
            q.s[2] = 0x28;
            q.s[3] = 0x32;
        } else {
            q.s[1] = 0x152;
            q.s[3] = 0x20;
            q.s[2] = 0x20;
        }
        n = icon + 1;
        a = (n & 7) << 5;
        b = (n >> 3) << 5;
        q.s[0] = x;
        ((s16 *)&q.uv0)[0] = a + 1;
        ((s16 *)&q.uv0)[1] = b + 1;
        ((s16 *)&q.uv1)[0] = a + 0x1F;
        ((s16 *)&q.uv1)[1] = b + 0x1F;
        if (side > 0) {
            q.s[2] = 0x10;
            ((s16 *)&q.uv1)[0] = a + 0x2F;
        } else if (side < 0) {
            q.s[2] = 0x10;
            q.s[0] = (s16)x + 0x10;
            ((s16 *)&q.uv0)[0] = a - 0xF;
        }
        q.col = item_col_tbl[col];
        flps0008(&q);
        if (mark != 0) {
            if (mark > 0) {
                q.uv0 = 0xE00080;
                q.col = -1;
                q.uv1 = 0x010000A0;
            } else {
                q.uv0 = 0xE000A0;
                q.uv1 = 0x010000C0;
                q.col = 0xFFFF0000;
            }
            if (side > 0) {
                q.uv1 -= 0x10;
            } else if (side < 0) {
                q.uv0 += 0x10;
            }
            flps0008(&q);
        }
    }
}

/* 0x12FD50 */
void disp_item_stock(void) {
    u8 m = lpPit->x07;

    if (m > 1) {
        if (m == 2) {
            if (lpPit->x55 == 0) {
                DispFrameMessage(pf_item_stock_full, lit_3772);
            }
            return;
        }
        ItemListWindow(lpPit->x49, item_stock_color_tbl0[m], 8);
        DispFrameMessage(pf_item_stock_name, item_str[(u16)lpPit->x52]);
        Disp_help_mess(1, lpPit->x50);
        switch (lpPit->x07) {
        case 4:
            if (lpPit->x56 == 0) {
                Put_shousai(lpPit->x07, lpPit);
            }
        case 3:
            *(u32 *)(pf_item_stock_cmd + 0x10) = item_stock_color_tbl1[lpPit->x07];
            DispFrameList(pf_item_stock_cmd, 0, lpPit->x4F);
            return;
        case 5:
            DispFrameList(pf_item_stock_yn, 0, lpPit->yn);
            break;
        }
    }
}

/* 0x12FEB0 */
void disp_others_info(void) {
    s16 y;
    u32 i;
    PLW *pl;
    u8 *g;

    player_info_sub(20.0f, lpPit->pl, 0x50);
    y = 0x6A;
    pl = player_work;
    i = 0;
    g = (u8 *)&game_w;
    do {
        if (*(u8 *)pl != 0 && i != game_w.master && g[0x208] == 1) {
            player_info_sub(20.0f, pl, y);
            y = y + 0x1A;
        }
        i++;
        g++;
        pl++;
    } while (i < 4U);
}

extern u8 pl_type_uv[];
extern u16 uv_tbl_3869[][4];
void flps0002(void *);
typedef struct PFLPS1 {
    s16 s[4];
    u32 col;
} PFLPS1;

/* 0x12FF70 */
void player_info_sub(f32 x, PLW *pl, s16 y) {
    PFLPS2 q;
    PFLPS1 b;
    s16 yy;
    int n;
    s16 yy2;
    int i;
    EMW *em;
    u8 f;
    s16 t;
    u8 k;
    int cnt;

    SetFilterMode(1);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    if (*(u16 *)((u8 *)pl + 0xC) == game_w.master) {
        q.col = 0x80000000;
    } else {
        q.col = 0x80503108;
    }
    q.s[1] = y;
    q.s[3] = 0x18;
    q.s[0] = 0.8f * x;
    q.s[2] = 0x13;
    *(u32 *)&q.uv[0] = 0x1A00AA;
    *(u32 *)&q.uv[2] = 0x2F00C2;
    flps0008(&q);
    q.s[0] += q.s[2];
    q.s[2] = 0x48;
    q.uv[0] = 0xC2;
    q.uv[2] = 0xD2;
    flps0008(&q);
    q.s[0] += q.s[2];
    q.s[2] = 6;
    q.uv[0] = 0xD2;
    q.uv[2] = 0xDA;
    flps0008(&q);
    yy = y;
    yy2 = yy + 2;
    q.s[0] = 0.8f * (2.0f + x);
    q.s[1] = yy2;
    q.s[2] = 0x10;
    q.s[3] = 0x14;
    q.col = disp_pl_rgb[*(u16 *)((u8 *)pl + 0xC) & 3];
    q.uv[0] = pl_type_uv[*((u8 *)pl + 2)];
    q.uv[2] = q.uv[0] + 0x14;
    q.uv[1] = 0xED;
    q.uv[3] = 0xFF;
    flps0008(&q);
    if (*(u16 *)((u8 *)pl + 0xC) != game_w.master) {
        f32 t3 = 5.0f + x;
        s16 v = yy + 0x15;
        b.col = 0xFF00FF00;
        b.s[3] = v;
        b.s[1] = v;
        b.s[0] = 0.8f * t3;
        b.s[2] = 0.8f * (t3 + 112.0f * ((f32)*(s16 *)((u8 *)pl + 0x302) / (f32)*(s16 *)((u8 *)pl + 0x792)));
        flps0002(&b);
    }
    flfntSetSize(0x12, 0x12);
    font_set_palette(0);
    {
        f32 nx = 24.0f + x;
        flfntLocate((int)nx, (s16)(yy + 1));
        if (*(u16 *)((u8 *)pl + 0xC) == game_w.master) {
            flfntLocate((int)nx, yy2);
        }
    }
    player_name_id_print(pl);
    q.s[2] = 0x10;
    q.s[1] = yy2;
    q.s[3] = 0x14;
    q.col = -1;
    q.uv[1] = 0xED;
    q.uv[3] = 0xFF;
    cnt = 0;
    if (*((u8 *)pl + 0x81E) != 0) {
        cnt = 1;
    } else {
        em = em_work;
        for (i = 0; i < 20; i++, em++) {
            if (*(u8 *)em != 0 && em->stg == ((EMW *)pl)->stg && *((u8 *)em + 2) != 0xF) {
                if (em->x9E9 != 0 && *((u8 *)em + 0x881) == 1 && *((u8 *)em + 0x882) == 0 &&
                    *(u16 *)((u8 *)pl + 0xC) == *((u8 *)em + 0x883) && *((u8 *)em + 0x888) == 1) {
                    cnt = 1;
                    break;
                } else if (*((u8 *)em + 0x7EE) & (1 << *(u16 *)((u8 *)pl + 0xC))) {
                    cnt = 1;
                    break;
                }
            }
        }
    }
    if (cnt > 0) {
        q.s[0] = 0.8f * (122.0f + x);
        q.uv[0] = cnt * 0x14 + 0x64;
        q.uv[2] = cnt * 0x14 + 0x78;
        flps0008(&q);
    }
    f = *((u8 *)pl + 0x4D5);
    if ((f & 0x3F) != 0) {
        u16 id = *(u16 *)((u8 *)pl + 0xC);
        s16 *cd = (s16 *)((u8 *)lpPit + id * 2 + 0x10);
        u8 *sel = (u8 *)lpPit + id + 0x18;
        if (*cd < 0) {
            for (k = 0; k < 6; k++) {
                if (f & (1 << k)) {
                    *sel = k;
                    *cd = 0x14;
                    break;
                }
            }
        } else if ((f & (1 << *sel)) && *cd > 0) {
            *cd -= 1;
        } else {
            int c2 = 6;
            do {
                (*sel)++;
                if (*sel >= 6) *sel = 0;
                if (*((u8 *)pl + 0x4D5) & (1 << *sel)) {
                    *cd = 0x14;
                    break;
                }
            } while (--c2 != 0);
        }
        q.s[0] = 0.8f * (142.0f + x);
        q.s[1] = yy2;
        q.s[2] = 0x10;
        q.s[3] = 0x14;
        q.uv[0] = uv_tbl_3869[*sel][0];
        q.uv[1] = uv_tbl_3869[*sel][1];
        q.uv[2] = uv_tbl_3869[*sel][2];
        q.uv[3] = uv_tbl_3869[*sel][3];
        flps0008(&q);
    } else {
        *(s16 *)((u8 *)lpPit + *(u16 *)((u8 *)pl + 0xC) * 2 + 0x10) = -1;
    }
    if (*(s16 *)((u8 *)lpPit + *(u16 *)((u8 *)pl + 0xC) * 2 + 0x34) >= 0) {
        u16 id = *(u16 *)((u8 *)pl + 0xC);
        SetTrnslMode(4, 1);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        disp_map_sign((s16)((s16)(int)(0.8f * (5.0f + x)) + 8), (s16)(yy + 0xC),
                      *(s16 *)((u8 *)lpPit + id * 2 + 0x34), disp_pl_rgb[id & 3]);
    }
}

/* ===== timer, gauges (0x1306A0-0x131580) ===== */
extern u16 System_timer;
extern u8 needle_data[][0x24];
void flSinCos(f32, f32 *, f32 *);
f32 flSin(f32);
struct GAGE;
void gage_disp(struct GAGE *, int);
void bar_disp(struct GAGE *, int);
void disp_needle(int, int);
typedef struct GAGE {
    f32 x;      /* 0x00 */
    f32 len;    /* 0x04 */
    s16 y;      /* 0x08 */
    s16 cur;    /* 0x0A */
    s16 max;    /* 0x0C */
    u8 _pad0E[2];
    u32 col;    /* 0x10 */
} GAGE;

/* 0x1306A0 */
void disp_timer(void) {
    PFLPS3 q;
    int t0, t1;
    int v;
    int a;

    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    switch (game_w.x0D5) {
    case 2:
        lpPit->time0 = Quest_time_get(0);
        lpPit->time1 = Quest_time_get(1);
        break;
    case 4:
    case 3:
        switch (game_w.quest) {
        case 0xCF:
        case 0x6A:
        case 0x69:
        case 0x68:
        case 0x67:
        case 0xCE:
        case 0x6B:
        case 0x65:
            if (*(s16 *)(quest_w + 0x34) != 0) {
                lpPit->time0 = 0;
            }
            break;
        case 0xD0:
        case 0xCD:
            lpPit->time0 = 0;
            break;
        }
        break;
    }
    t0 = lpPit->time0;
    t1 = lpPit->time1;
    q.s[2] = 0x47;
    q.s[3] = 0x40;
    q.uv0 = 0x310000;
    q.uv1 = 0x710047;
    q.s[0] = 0x10;
    q.s[1] = 0x10;
    if (t0 > 0x2328) {
        q.col = -1;
    } else if (t0 > 0x708) {
        v = ((System_timer & 0x1F) << 11) & 0xFFFF;
        a = (s8)(60.0f * flSin(0.0000958738f * (f32)v)) + 0xC0;
        q.col = (a << 8) | 0xFFFF0000 | a;
    } else {
        v = ((System_timer & 0xF) << 12) & 0xFFFF;
        a = (s8)(120.0f * flSin(0.0000958738f * (f32)v)) + 0x80;
        q.col = (a << 8) | 0xFFFF0000 | a;
    }
    flps0008(&q);
    SetFilterMode(0);
    disp_needle(t1, 1);
    v = t1 - t0;
    if (v < 0) {
        v = 0;
    }
    disp_needle(v, 0);
}

/* 0x1309A0 */
void disp_needle(int n, int sel) {
    PFLP12 q;
    f32 s, c;
    f32 *d = (f32 *)needle_data[sel];
    u8 *b = needle_data[sel];
    int k;

    q.col = -1;
    q.uv[0] = b[0x20];
    q.uv[1] = b[0x21];
    q.uv[2] = b[0x22];
    q.uv[3] = b[0x21];
    q.uv[4] = b[0x20];
    q.uv[5] = b[0x23];
    k = n / 1800;
    k = k / 5 * 5;
    flSinCos((f32)k * 0.10471976f - 3.1415927f, &s, &c);
    q.p[0] = 0.5f + 0.8f * (64.5f + d[0] * c - d[1] * s);
    q.p[1] = 0.5f + (48.0f + d[0] * s + d[1] * c);
    q.p[2] = 0.5f + 0.8f * (64.5f + d[2] * c - d[3] * s);
    q.p[3] = 0.5f + (48.0f + d[2] * s + d[3] * c);
    q.p[4] = 0.5f + 0.8f * (64.5f + d[4] * c - d[5] * s);
    q.p[5] = 0.5f + (48.0f + d[4] * s + d[5] * c);
    flps000C(&q);
    q.uv[0] = b[0x22];
    q.uv[1] = b[0x23];
    q.p[0] = 0.5f + 0.8f * (64.5f + d[6] * c - d[7] * s);
    q.p[1] = 0.5f + (48.0f + d[6] * s + d[7] * c);
    flps000C(&q);
}

/* 0x130C70 */
void disp_pl_vital(void) {
    GAGE g;
    PLW *pl = lpPit->pl;
    int a;
    u32 col;
    int v;

    SetFilterMode(0);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    g.x = 101.0f;
    g.y = 0x19;
    g.len = 288.0f;
    g.max = 0x64;
    g.cur = *(s16 *)((u8 *)pl + 0x792);
    g.col = -1;
    gage_disp(&g, 0);
    if (*(s16 *)((u8 *)pl + 0x302) > 0) {
        s16 mx = *(s16 *)((u8 *)pl + 0x790);
        if (lpPit->x24 < mx) {
            g.cur = mx;
            g.col = 0xFFC01010;
            bar_disp(&g, 0);
        }
        g.cur = lpPit->x24;
        g.col = 0xFF10C010;
        bar_disp(&g, 0);
    }
    g.y = 0x26;
    g.max = 0x12C;
    g.cur = *(s16 *)((u8 *)pl + 0x882);
    g.col = -1;
    gage_disp(&g, 1);
    if (*(s16 *)((u8 *)pl + 0x748) < 0x4C) {
        a = ((s16)(64.0f * flSin(2.0f * (3.1415927f * ((f32)((System_timer % 45) * 8) / 360.0f)))) + 0x40) & 0xFF;
        col = (a << 8) | 0xFFFF0000 | a;
    } else if (*(s16 *)((u8 *)pl + 0x8CC) == 0) {
        col = 0xFFF0F000;
    } else {
        v = ((System_timer & 0x3F) << 10) & 0xFFFF;
        a = ((s8)40.0f + 0xE7) & 0xFF;
        col = (((s8)(40.0f * flSin(0.0000958738f * (f32)v)) + 0x9F) & 0xFF) | ((a << 16) | 0xFF000000 | (a << 8));
    }
    g.col = col;
    g.cur = *(s16 *)((u8 *)pl + 0x748);
    bar_disp(&g, 1);
}

/* 0x130F50 */
void gage_disp(GAGE *g, int sel) {
    PFLPS2 q;
    f32 x, l, t;
    u32 w, n, r;

    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    q.s[1] = g->y;
    q.s[3] = 0xD;
    q.col = g->col;
    if (sel == 0) {
        q.uv[1] = 0x55;
        q.uv[3] = 0x62;
    } else {
        q.uv[1] = 0x61;
        q.uv[3] = 0x54;
    }
    t = g->len * ((f32)g->cur / (f32)g->max);
    w = (u32)t;
    x = g->x;
    r = w % 48;
    q.uv[0] = 0x48;
    n = w / 48;
    q.uv[2] = 0x78;
    while (n != 0) {
        t = 0.8f * x;
        x += 54.216003f;
        q.s[0] = t;
        q.s[2] = (s16)(0.8f * x) - (s16)t;
        flps0008(&q);
        n--;
    }
    q.s[0] = 0.8f * x;
    q.s[2] = 0.8f * (1.1295f * (f32)r);
    q.uv[2] = r + 0x48;
    flps0008(&q);
    q.s[0] = (f32)q.s[0] + (-3.2f + (f32)q.s[2]);
    q.s[2] = 0x19;
    q.s[3] = 0x12;
    q.uv[0] = 0x47;
    q.uv[2] = 0x67;
    if (sel == 0) {
        q.s[1] = g->y - 5;
        q.uv[1] = 0x31;
        q.uv[3] = 0x43;
    } else {
        q.s[1] = g->y - 2;
        q.uv[1] = 0x42;
        q.uv[3] = 0x54;
    }
    flps0008(&q);
}

/* 0x131280 */
void bar_disp(GAGE *g, int sel) {
    PFLPS3 q;

    if (g->cur > 0) {
        if (g->max <= 0) {
        } else {
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q.s[0] = 0.8f * g->x;
        q.s[2] = 0.90360004f * (g->len * ((f32)g->cur / (f32)g->max));
        q.s[3] = 4;
        q.s[1] = g->y + (s16)((sel != 0) ? 2 : 6);
        q.col = g->col;
        q.uv0 = 0x56007A;
        q.uv1 = 0x5A007C;
        flps0008(&q);
        }
    }
}

/* ===== slash level, pachinger, cannon, menu list (0x1313A0-0x1324B0) ===== */
extern u8 slash_lev_tbl[];
typedef struct PACHISIGHT {
    s16 x, y, w, h;     /* 0x00 */
    s8 rot;             /* 0x08 */
    u8 axis;            /* 0x09 */
    s16 a, b;           /* 0x0A */
} PACHISIGHT;
extern PACHISIGHT pachisight_tbl[4];
extern s16 gun_load_mess[][8];
extern u8 menu_str_002EF860[];
extern u8 pfl_menu[];
extern u8 lit_4454_0035A600[], lit_4493_0035A620[], lit_136_00358B70[];
extern u8 pf_chat_cnfg[], pf_chcnfg_reibun[], pf_lb_chcnfg_sendpl[];
extern u8 str_tbl_reibun0[];
extern void *q_sendpl_list[];
extern void *lb_sendpl_list[];
int Pl_slash_lv_ck(PLW *);
int PachingerCamChk(PLW *);
s8 GetPachingerInfo(PLW *, u8 *, u8 *, f32 *);
void Put_sprite_rotate(void *, s8, ...);
f32 flCos(f32);
int Pl_shell_set(PLW *, u16, int);
void disp_gun_load_mess(int);
void func_5B4B20();
int Game_clear_ck(int);
void DispFrameListOptionArrowC(void *, u32);
void Disp_name_or_id(int);
void Reibun_print(int, int);
void lb_disp_chat_cnfg_sendpl(int, PIT_W *);
void font_print_strings(s16, s16, void *, int);
void disp_menu(int, PIT_W *);

/* 0x1313A0 */
void disp_slash_level(void) {
    PFLPS2 q;
    PLW *pl = lpPit->pl;
    u8 c = *((u8 *)pl + 2);
    u32 lv;
    int v;

    if (c == 1 || c == 5) return;
    SetFilterMode(0);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    lv = (u8)Pl_slash_lv_ck(pl);
    q.s[0] = 0x53;
    q.s[1] = 0x2B;
    q.s[2] = 0x2F;
    q.s[3] = 0x1A;
    q.col = -1;
    q.uv[0] = 0xC5;
    q.uv[2] = 0xFF;
    q.uv[1] = slash_lev_tbl[lv];
    q.uv[3] = q.uv[1] + 0x1A;
    flps0008(&q);
    if (lv < 3U) {
        if (lv == 0) goto lab;
    } else {
lab:
        SetFilterMode(1);
        SetTrnslMode(4, 1);
        v = ((System_timer & 0x7F) << 9) & 0xFFFF;
        q.col = (((s8)(127.0f * flSin(0.0000958738f * (f32)v)) + 0x7F) << 24) | 0xFFFFFF;
        if (lv == 0) {
            q.col &= 0xFFFF0000;
        }
        q.uv[1] = 0x31;
        q.uv[3] = 0x4B;
        flps0008(&q);
        SetTrnslMode(4, 5);
    }
}

/* 0x131580 */
void disp_pachinger(void) {
    PFLPS3 q;
    u8 a, b;
    f32 f;
    PLW *pl = lpPit->pl;
    s8 st;
    int i;
    f32 x;

    if (PachingerCamChk(pl) == 0) {
        lpPit->x2A = -1;
        return;
    }
    SetFilterMode(1);
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    lpPit->x2A = GetPachingerInfo(pl, &a, &b, &f);
    if (b != 0) {
        q.col = -1;
        q.s[1] = 0x14A;
        q.s[2] = 0x10;
        q.s[3] = 0x14;
        q.s[0] = 0xBC;
        q.uv0 = 0xC40014;
        q.uv1 = 0xD80028;
        flps0008(&q);
        q.s[0] = 0x136;
        q.uv0 = 0x28;
        q.uv1 = 0x14;
        flps0008(&q);
        q.uv0 = 0;
        x = 263.0f;
        q.uv1 = 0x14;
        for (i = 5; i != 0; i--) {
            q.s[0] = 0.8f * x;
            flps0008(&q);
            x += 24.0f;
        }
        q.s[0] = 242.0f + 142.0f * f;
        q.s[1] = 0x15D;
        q.s[2] = 0x14;
        q.s[3] = 0x14;
        q.uv0 = 0xD800B4;
        q.uv1 = 0xEC00C8;
        Put_sprite_rotate(&q, 2, 0x14, 0x15D);
    }
    if (a != 0) {
        st = lpPit->x28;
        if (st < 4) {
            int s2 = st;
            int s3;
            PACHISIGHT *p;
            int n;
            if (st == 0) {
                int v = ((lpPit->x29 & 0x1F) << 11) & 0xFFFF;
                q.col = (((s8)(94.0f * flCos(0.0000958738f * (f32)v)) + 0xA0) << 24) | 0xFFFFFF;
                lpPit->x29++;
            } else {
                q.col = -1;
                lpPit->x29 = 0;
            }
            s3 = (s16)(4 - s2);
            q.s[3] = 0x14;
            q.s[2] = 0x14;
            p = pachisight_tbl;
            q.uv0 = 0xD800B4;
            q.uv1 = 0xEC00C8;
            for (n = 4; n != 0; n--, p++) {
                if (p->axis == 0) {
                    q.s[0] = (p->a * s2 + p->b * s3) >> 2;
                    q.s[1] = p->y;
                } else {
                    q.s[0] = p->x;
                    q.s[1] = (p->a * s2 + p->b * s3) >> 2;
                }
                Put_sprite_rotate(&q, p->rot);
            }
            if (lpPit->x2B != 0) {
                se_req(1, 0x7A, 0);
            }
        }
        q.col = -1;
        ((s16 *)&q.uv0)[1] = 0xD8;
        q.uv1 = 0x100;
        ((s16 *)&q.uv1)[1] = 0xEC;
        {
            PACHISIGHT *p = pachisight_tbl;
            int n;
            for (n = 4; n != 0; n--, p++) {
                q.s[0] = p->x;
                q.s[1] = p->y;
                q.s[2] = p->w;
                q.s[3] = p->h;
                *(s16 *)&q.uv0 = (s16)(*(s16 *)&q.uv1 - q.s[2]);
                Put_sprite_rotate(&q, p->rot);
            }
        }
    }
}

/* 0x1319D0 */
void disp_cannon(void) {
    PFLP12 q;
    PLW *pl = lpPit->pl;
    f32 x;
    u32 i;

    switch (*((u8 *)pl + 2)) {
    case 5:
    case 1:
        SetFilterMode(0);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        if (pl->work8BC == 0) {
            if (*((u8 *)pl + 0x12) != 0 && (u16)Pl_shell_set(pl, pl->work88E, 1) != 0xFF) {
                disp_gun_load_mess(2);
            }
            return;
        }
        if (lpPit->x2A != 0) {
            x = 120.0f;
            q.p[1] = 0x32;
        } else {
            q.p[1] = 0x123;
            x = 320.0f - 6.0f * (f32)*((u8 *)pl + 0x1D);
        }
        q.p[2] = 9;
        q.p[3] = 0x14;
        q.col = -1;
        q.uv[1] = 0xEC;
        q.uv[5] = 0xFF;
        i = 0;
        if (*((u8 *)pl + 0x1D) != 0) {
            do {
                if (i < *((u8 *)pl + 0x1C)) {
                    q.uv[3] = 0xD8;
                    q.uv[0] = 0xCC;
                } else {
                    q.uv[0] = 0xE0;
                    q.uv[3] = 0xEC;
                }
                q.p[0] = 0.8f * x;
                flps0008(&q);
                i++;
                x += 12.0f;
            } while (i < *((u8 *)pl + 0x1D));
        }
        if (*((u8 *)pl + 0x1C) <= 0 && *((u8 *)pl + 0x12) != 0) {
            disp_gun_load_mess(0);
        }
        if (*(u16 *)((u8 *)pl + 0x2DC) == 0x57C) {
            disp_gun_load_mess(1);
        }
        return;
    }
}

/* 0x131C00 */
void disp_gun_load_mess(int n) {
    PFLP12 q;
    s16 *m = gun_load_mess[n];
    int v = ((System_timer & 0x1F) << 11) & 0xFFFF;
    int a = ((s8)(96.0f * flSin(0.0000958738f * (f32)v)) + 0x9F) & 0xFF;

    if (lpPit->x2A != 0) {
        q.col = ((a & 0xFF) << 24) | 0xFFFFFF;
        q.p[0] = m[0];
        q.p[1] = 0x3A;
    } else {
        int b = a & 0xFF;
        q.col = b | ((b << 16) | 0xFF000000 | (b << 8));
        q.p[0] = m[7];
        q.p[1] = 0xD5;
    }
    q.p[2] = m[1];
    q.p[3] = m[2];
    q.uv[0] = m[3];
    q.uv[1] = m[4];
    q.uv[2] = m[5];
    q.uv[3] = m[6];
    flps0008(&q);
}

/* 0x131D50 */
void disp_menu(int sw, PIT_W *p) {
    char buf[48];   /* the title " ~C05menu  ~C00%01d/..." is longer than 16 bytes */
    int n, base, s3;
    u32 col;
    int v;

    if (game_w.x1DC != 0) {
        func_5B4B20(sw);
        return;
    }
    n = lpPit->x41 / 5;
    base = (s16)(n * 5);
    SetTrnslMode(4, 5);
    if (n == 0) {
        *(void **)(pfl_menu + 0xC) = menu_str_002EF860;
    } else {
        s3 = 1;
        if (Online_ck() == 0) {
            s3 = (s16)2;
        }
        if (Game_clear_ck(0) == 1) {
            s3 = (s16)(s3 + 2);
        }
        *(void **)(pfl_menu + 0xC) = menu_str_002EF860 + (s16)s3 * 0x14;
    }
    if ((s8)sw == 0) {
        v = ((System_timer & 0x3F) << 10) & 0xFFFF;
        *(u32 *)(pfl_menu + 0x10) = 0xA9182;
        col = (((s8)(48.0f * flSin(0.0000958738f * (f32)v)) + 0xAF) << 8) | 0xF0200020;
    } else {
        *(u32 *)(pfl_menu + 0x10) = 0x808080;
        col = 0xA0207020;
    }
    sprintf(buf, (char *)lit_4454_0035A600, n + 1);
    DispFrameList(pfl_menu, buf, lpPit->x41 - (s16)base);
    DispFrameListOptionArrowC(pfl_menu, col);
    if (Online_ck() == 1 && (s8)sw == 0) {
        Disp_name_or_id(0xD1);
    }
}

/* 0x131FB0 */
void Pit_disp_chat_cnfg(void) {
    char buf[64];   /* PC: room for the longest game format string (the PS2 frame is smaller) */
    u32 i;
    int y;
    u8 *e;
    s8 k;

    switch (lpPit->x7E) {
    case 0:
        disp_menu(1, lpPit);
        DispFrameList(pf_chat_cnfg + game_w.x1DC * 0x18, 0, lpPit->x7D);
        break;
    case 1:
        switch (lpPit->x7D) {
        case 0:
            lb_disp_chat_cnfg_sendpl(1, lpPit);
            break;
        case 1:
            k = PitMenu.x1B;
            sprintf(buf, (char *)lit_4493_0035A620, k / 6 + 1);
            DispFrameList(pf_chcnfg_reibun, buf, k % 6);
            DispFrameListOptionArrow(pf_chcnfg_reibun);
            flfntSetSize(0x12, 0x12);
            font_set_palette(0);
            i = 6;
            e = str_tbl_reibun0 + (PitMenu.x1B / 6) * 0x60;
            y = 0x54;
            do {
                flfntLocate(0x1AF, y);
                Reibun_print(10, *(s32 *)(e + 0xC));
                i--;
                e += 0x10;
                y = (s16)(y + 0x16);
            } while (i != 0);
            break;
        }
        break;
    }
}

/* 0x132160 */
void lb_disp_chat_cnfg_sendpl(int sw, PIT_W *p) {
    PFLPS3 q;
    u32 cnt = (game_w.x1DC != 0) ? 8U : 4U;
    s8 sel;
    u32 i;
    int bit;

    flfntSetSize(0x12, 0x12);
    if (game_w.x1DC == 0) {
        void **out = q_sendpl_list + 1;
        PLW *pl = player_work;
        u8 *rm = room_member_id;
        for (i = 0; i < 4; i++, pl++, rm += 8) {
            if (game_w.master != i) {
                if (game_w.pl_state[i] == 1) {
                    *out = (u8 *)pl + 0x8D4;
                    if (Online_ck() == 1 && PitMenu.x14 != 0) {
                        *out = rm;
                    }
                } else {
                    *out = lb_no_player;
                }
                out++;
            }
        }
    } else {
        void **out = lb_sendpl_list + 1;
        u8 *d = D_6EAC80;
        unsigned long long j;
        for (j = 0; j < 8; j++, d += 0x38) {
            if (game_w.master != j) {
                if (func_5D8370((s8)j) == 0) {
                    *out = d + 4;
                    if (Online_ck() == 1) {
                        if (PitMenu.x14 != 0) {
                            *out = d + 0x24;
                        }
                    }
                } else {
                    *out = lb_no_player;
                }
                out++;
            }
        }
    }
    sel = lpPit->x80;
    if (sel < 0) {
        sel = 0;
    } else if (sel < game_w.master) {
        sel += 1;
    }
    DispFrameList(pf_lb_chcnfg_sendpl + game_w.x1DC * 0x18, lit_136_00358B70, sel);
    font_set_palette(0);
    if (game_w.x1DC == 0) {
        font_print_strings(*(s16 *)pf_lb_chcnfg_sendpl + 0x24, *(s16 *)(pf_lb_chcnfg_sendpl + 2) + 0x16, q_sendpl_list, 0x16);
    } else {
        font_print_strings(*(s16 *)(pf_lb_chcnfg_sendpl + 0x18) + 0x24, *(s16 *)(pf_lb_chcnfg_sendpl + 0x1A) + 0x16, lb_sendpl_list, 0x16);
    }
    q.s[0] = 0x17C;
    q.s[2] = 0x10;
    q.s[3] = 0x14;
    q.col = -1;
    q.uv1 = 0x010000F0;
    q.uv0 = 0xEC00DC;
    if (PitMenu.x15 != 0) {
        q.s[1] = 0x53;
        flps0008(&q);
        return;
    }
    i = 0;
    q.s[1] = 0x69;
    bit = 1;
    if (cnt != 0) {
        do {
            if (i != game_w.master) {
                if (PitMenu.x16 & bit) {
                    flps0008(&q);
                }
                q.s[1] += 0x16;
            }
            i++;
            bit *= 2;
        } while (i < cnt);
    }
}

/* ===== wyvern ripple, data/option windows (0x132510-0x133A00) ===== */
extern u8 pfl_menu_data[];
extern u8 pf_mix_list_base[], pf_monster_list_base[];
extern u8 lit_4947[], lit_4948[], lit_4949[], lit_4985[], lit_4986[], lit_4987[], lit_5012[], lit_5054[];
extern u8 mix_level_color[8];
extern char *mix_level_str[];
extern u8 monster_data[][8];
extern u8 frame_status_main_002F0D70[];
extern u8 pfl_option[];
extern u8 option_list_str[];
extern u8 option_val_str[];
extern u8 opt_map_invalid_str[];
int Item_preparation_list_num();
u32 Monster_list_num();
void disp_mix_list(int, PIT_W *);
void disp_monster_list(int, PIT_W *);
void EquipmentDescriptionWindow(int, int, int, u8, int);
void PlayerEquipmentWindow(PLW *);
void flps0004(void *);
int menu_equip_get_equip(u8);
void efct_circle(int, int, int, f32, f32);

/* 0x132510 */
void wyvn_efct_ripple(void) {
    if (lpPit->x3F != 0) {
        SetTrnslMode(4, 1);
        SetFilterMode(1);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        efct_circle(0x199, 0xAC, 0xFFFFFF, 120.0f, (f32)lpPit->x3F);
        SetTrnslMode(4, 5);
    }
}

/* 0x1332F0 */
void Pit_disp_data(void) {
    switch (lpPit->x42) {
    case 0:
        disp_menu(1, lpPit);
        DispFrameList(pfl_menu_data + game_w.x1DC * 0x18, 0, lpPit->x43);
        break;
    case 1:
        switch (lpPit->x43) {
        case 0:
            disp_mix_list(1, lpPit);
            break;
        case 1:
            disp_monster_list(1, lpPit);
            break;
        }
        break;
    }
}

/* 0x1333B0 */
void disp_mix_list(int sw, PIT_W *p) {
    DispFrameList(pf_mix_list_base, lit_4947, -1);
    if ((u8)Item_preparation_list_num() > 1) {
        DispFrameListOptionArrow(pf_mix_list_base);
    }
    flfntSetSize(0x12, 0x12);
    if (lpPit->x68 != 0) {
        font_set_palette(3);
        flfntLocate(0x1AF, 0x52);
        font_print(lit_4948, *(u8 *)&lpPit->x81 + 1);
        font_set_palette(0);
        flfntLocate(0x1C1, 0x7A);
        font_print_uf(item_str[*(s16 *)((u8 *)lpPit->x68 + 2)]);
        flfntLocate(0x1C1, 0xA2);
        font_print_uf(item_str[lpPit->x6C]);
        flfntLocate(0x1C1, 0xB6);
        font_print_uf(item_str[lpPit->x6E]);
        font_set_palette(mix_level_color[*(s8 *)((u8 *)lpPit->x68 + 4)]);
        flfntLocate(0x1C1, 0xDE);
        font_print_uf(mix_level_str[*(s8 *)((u8 *)lpPit->x68 + 4)]);
        return;
    }
    font_set_palette(2);
    flfntLocate(0x1AF, 0x52);
    font_print_uf(lit_4949);
}

/* 0x133550 */
void disp_monster_list(int sw, PIT_W *p) {
    PFLPS2 q;
    u8 *m;
    s8 sel;

    DispFrameList(pf_monster_list_base, lit_4985, -1);
    if (Monster_list_num() > 1U) {
        DispFrameListOptionArrow(pf_monster_list_base);
    }
    flfntSetSize(0x12, 0x12);
    sel = lpPit->x82;
    if (sel >= 0) {
        m = monster_data[sel];
        font_set_palette(3);
        flfntLocate(0x167, 0x52);
        font_print(lit_4948, lpPit->x82 + 1);
        flfntLocate(0x167, 0x66);
        font_print_sp(lit_4986, enemy_name[m[3]]);
        flfntLocate(0x167, 0xD2);
        font_print_uf(*(void **)(m + 4));
        SetTrnslMode(4, 5);
        SetFilterMode(1);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        q.s[0] = 0x15C;
        q.s[2] = 0x50;
        q.s[1] = 0x80;
        q.s[3] = 0x48;
        *(u32 *)&q.uv[0] = 0x380008;
        *(u32 *)&q.uv[2] = 0xB80088;
        q.col = -1;
        flps0008(&q);
        SetFilterMode(0);
        reload_tex(1, 0x11B);
        SetTextureStage(0x11B);
        q.s[0] = 0x16A;
        q.s[2] = 0x33;
        q.s[1] = 0x84;
        q.s[3] = 0x40;
        q.uv[0] = m[1];
        q.uv[1] = m[2];
        q.uv[2] = m[1] + 0x33;
        q.uv[3] = m[2] + 0x33;
        q.col = -1;
        flps0008(&q);
        return;
    }
    flfntLocate(0x167, 0x52);
    font_print_sp(lit_4987);
}

/* 0x1337A0 */
void Pit_disp_menu_equipment(void) {
    PFLPS1 q;
    PLW *pl = lpPit->pl;
    int e;
    u32 v;

    e = menu_equip_get_equip(lpPit->x43);
    DispFrameList(frame_status_main_002F0D70, lit_5012, -1);
    PlayerEquipmentWindow(pl);
    q.s[0] = 0x12C;
    q.s[2] = 0x1EB;
    q.s[1] = (lpPit->x43 << 5) + 0x55;
    v = ((System_timer & 0x3F) << 10) & 0xFFFF;
    q.s[3] = q.s[1] + 0x20;
    q.col = (((s8)(48.0f * flSin(0.0000958738f * (f32)v)) + 0xBF) << 24) | 0xA9182;
    flps0004(&q);
    EquipmentDescriptionWindow(e, 0x132, 0x12A, lpPit->x44, 0);
}

/* 0x1338E0 */
void disp_option(void) {
    PFLPS3 q;
    int y;
    long long i;
    u8 *vals;
    int v;

    SetTrnslMode(4, 5);
    *(void **)(pfl_option + 0xC) = option_list_str + lpPit->x88 * 0x14;
    DispFrameList(pfl_option, lit_5054, -1);
    font_set_palette(5);
    y = 0x54;
    i = 0;
    vals = option_val_str;
    do {
        flfntLocate(0x208, y);
        if (lpPit->x88 != 0) {
            if ((s16)i != 1) goto lab;
            font_print_sp(opt_map_invalid_str);
        } else {
lab:
            font_print_uf(*(void **)(vals + *((u8 *)lpPit + (s16)i + 0x88) * 4));
        }
        vals += 0xC;
        i = (s16)(i + 1);
        y = (s16)(y + 0x16);
    } while (i < 5);
    q.s[2] = 0xE;
    q.s[3] = 0x12;
    q.s[1] = lpPit->x43 * 0x16 + 0x54;
    if (lpPit->x88 != 0) {
        if (lpPit->x43 != 1) goto lab2;
        q.col = 0xF0707070;
    } else {
lab2:
        v = ((System_timer & 0x3F) << 10) & 0xFFFF;
        q.col = (((s8)(48.0f * flSin(0.0000958738f * (f32)v)) + 0xAF) << 8) | 0xF0200020;
    }
    q.s[0] = 0x192;
    q.uv0 = 0x1A00A6;
    q.uv1 = 0x2E0094;
    flps0008(&q);
    q.s[0] = 0x1DC;
    q.uv0 = 0x94;
    q.uv1 = 0xA6;
    flps0008(&q);
}

/* ===== mix / pit effects (0x133FB0-0x134950) ===== */
typedef struct PEF_DATA {
    s16 ang;        /* 0x00 */
    u16 blend;      /* 0x02 */
    u32 col;        /* 0x04 */
    s16 u, v;       /* 0x08 */
    s16 w, h;       /* 0x0C */
    s16 ox, oy;     /* 0x10 origin inside the cell */
    int *scale_tbl; /* 0x14 */
    int *alpha_tbl; /* 0x18 */
} PEF_DATA;
typedef struct PEF {                /* pit_efct[6], 0x20 bytes each */
    u8 on;          /* 0x00 */
    u8 show;        /* 0x01 */
    u8 alpha;       /* 0x02 */
    u8 delay;       /* 0x03 */
    f32 l, r, t, b; /* 0x04 corners, set by pef_get_scale */
    f32 x;          /* 0x14 */
    s16 y;          /* 0x18 */
    u8 _pad1A[2];
    PEF_DATA *d;    /* 0x1C */
} PEF;
extern PEF pit_efct[6];
extern PEF_DATA *ef1_tbl[];
extern PEF_DATA ef2_efct_tbl0, ef2_efct_tbl1;
extern PEF_DATA *ef3_tbl[];
extern s16 ofs_5159[][3][2];
void SetBlendingMode(u16);
int pef_get_scale(PEF *, int *, s16);
int pef_get_alpha(PEF *, int *, s16);

/* 0x133FB0 */
void mix_effect_set(s8 kind) {
    PEF *e;
    u32 i;
    s8 d;
    s16 (*o)[2];

    lpPit->x84 = 1;
    lpPit->x86 = 1;
    lpPit->x85 = kind;
    switch (kind) {
    case 0:
        pit_efct[0].on = 1; pit_efct[0].show = 0; pit_efct[0].delay = 0;
        pit_efct[0].x = 319.0f; pit_efct[0].y = 0xD2; pit_efct[0].d = ef1_tbl[0];
        pit_efct[1].on = 1; pit_efct[1].show = 0; pit_efct[1].delay = 5;
        pit_efct[1].x = 319.0f; pit_efct[1].y = 0xD2; pit_efct[1].d = ef1_tbl[1];
        pit_efct[2].on = 1; pit_efct[2].show = 0; pit_efct[2].delay = 10;
        pit_efct[2].x = 319.0f; pit_efct[2].y = 0xD2; pit_efct[2].d = ef1_tbl[2];
        pit_efct[3].on = 1; pit_efct[3].show = 0; pit_efct[3].delay = 15;
        pit_efct[3].x = 319.0f; pit_efct[3].y = 0xD2; pit_efct[3].d = ef1_tbl[3];
        pit_efct[4].on = 1; pit_efct[4].show = 0; pit_efct[4].delay = 20;
        pit_efct[4].x = 319.0f; pit_efct[4].y = 0xD2; pit_efct[4].d = ef1_tbl[0];
        pit_efct[5].on = 1; pit_efct[5].show = 0; pit_efct[5].delay = 25;
        pit_efct[5].x = 319.0f; pit_efct[5].y = 0xD2; pit_efct[5].d = ef1_tbl[1];
        return;
    case 1:
        e = pit_efct;
        i = 0;
        d = 0;
        o = ofs_5159[(System_timer * 3) >> 8];
        do {
            e[0].on = 1;
            e[0].show = 0;
            i++;
            e[0].delay = d;
            e[0].x = 269.0f + (f32)o[0][0];
            e[0].y = o[0][1] + 0xD2;
            e[0].d = &ef2_efct_tbl0;
            e[1].on = 1;
            e[1].show = 0;
            e[1].delay = d;
            d += 3;
            e[1].x = 269.0f + (f32)o[0][0];
            e[1].y = o[0][1] + 0xD2;
            o++;
            e[1].d = &ef2_efct_tbl1;
            e += 2;
        } while (i < 3U);
        return;
    case 2:
        pit_efct[0].on = 1; pit_efct[0].show = 0; pit_efct[0].delay = 0;
        pit_efct[0].x = 319.0f; pit_efct[0].y = 0xD2; pit_efct[0].d = ef3_tbl[0];
        pit_efct[1].on = 1; pit_efct[1].show = 0; pit_efct[1].delay = 0;
        pit_efct[1].x = 319.0f; pit_efct[1].y = 0xD2; pit_efct[1].d = ef3_tbl[1];
        pit_efct[2].on = 1; pit_efct[2].show = 0; pit_efct[2].delay = 0;
        pit_efct[2].x = 319.0f; pit_efct[2].y = 0xD2; pit_efct[2].d = ef3_tbl[2];
        pit_efct[3].on = 1; pit_efct[3].show = 0; pit_efct[3].delay = 0;
        pit_efct[3].x = 319.0f; pit_efct[3].y = 0xD2; pit_efct[3].d = ef3_tbl[3];
        pit_efct[4].on = 0;
        pit_efct[5].on = 0;
        return;
    }
}

/* 0x134280 */
void Pit_effect_move(void) {
    PEF *e;
    u32 k;
    int n;

    e = pit_efct;
    n = 0;
    k = 6;

    do {
        if (e->on != 0) {
            e->show = 0;
            n++;
            if (e->delay < lpPit->x86) {
                s16 t = lpPit->x86 - e->delay;
                e->show = 1;
                pef_get_scale(e, e->d->scale_tbl, t);
                pef_get_alpha(e, e->d->alpha_tbl, t);
            }
        }
        k--;
        e++;
    } while (k != 0);
    if (n != 0) {
        lpPit->x86++;
        return;
    }
    lpPit->x84 = 0;
}

/* 0x134360 */
void Pit_disp_pit_effect(void) {
    PFLP12 q;
    f32 s, c;
    PEF *e;
    int k;

    if (lpPit->x84 != 0) {
        SetFilterMode(1);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        k = 6;
        e = pit_efct;
        do {
            if (e->on != 0 && e->show != 0) {
                SetBlendingMode(e->d->blend);
                q.col = e->d->col | (e->alpha << 24);
                q.uv[4] = q.uv[0] = e->d->u + 1;
                q.uv[3] = q.uv[1] = e->d->v + 1;
                q.uv[2] = e->d->u + e->d->w - 1;
                q.uv[5] = e->d->v + e->d->h - 1;
                flSinCos((6.2831855f * (f32)e->d->ang) / 65536.0f, &s, &c);
                q.p[0] = 0.8f * (e->x + e->l * c - e->t * s);
                q.p[1] = e->y + (s16)(e->l * s + e->t * c);
                q.p[2] = 0.8f * (e->x + e->r * c - e->t * s);
                q.p[3] = e->y + (s16)(e->r * s + e->t * c);
                q.p[4] = 0.8f * (e->x + e->l * c - e->b * s);
                q.p[5] = e->y + (s16)(e->l * s + e->b * c);
                flps000C(&q);
                q.uv[0] = e->d->u + e->d->w - 1;
                q.uv[1] = e->d->v + e->d->h - 1;
                q.p[0] = 0.8f * (e->x + e->r * c - e->b * s);
                q.p[1] = e->y + (s16)(e->r * s + e->b * c);
                flps000C(&q);
            }
            k--;
            e++;
        } while (k != 0);
    }
}

/* 0x134710 */
int pef_get_scale(PEF *e, int *tbl, s16 t) {
    int *cur;
    int *nx;
    int t1;
    int ta;
    f32 a, b, f, g, dt, dd;

    cur = tbl;
    if (t < tbl[0]) {
        return 1;
    }
    t1 = cur[3];
    nx = cur + 3;
    if (t1 > 0) {
        for (;;) {
            if (!(t > t1)) {
                ta = cur[0];
                a = *(f32 *)&cur[1];
                b = *(f32 *)&cur[2];
                f = a + ((*(f32 *)&nx[1] - a) * (f32)(t - ta)) / (f32)(t1 - ta);
                g = b + ((*(f32 *)&nx[2] - b) * (f32)(t - ta)) / (f32)(t1 - ta);
                e->l = (f32)(-e->d->ox) * f;
                e->r = f * (f32)(e->d->w - e->d->ox);
                e->t = (f32)(-e->d->oy) * g;
                e->b = g * (f32)(e->d->h - e->d->oy);
                return 0;
            }
            cur = nx;
            nx += 3;
            t1 = *nx;
            if (t1 <= 0) break;
        }
    }
    e->on = 0;
    return -1;
}

/* 0x134860 */
int pef_get_alpha(PEF *e, int *tbl, s16 t) {
    int *cur;
    int *nx;
    int t1;
    f32 a, f;

    cur = tbl;
    if (t < tbl[0]) {
        return 1;
    }
    t1 = cur[2];
    nx = cur + 2;
    if (t1 > 0) {
        for (;;) {
            if (!(t > t1)) {
                a = *(f32 *)&cur[1];
                a = a + ((*(f32 *)&nx[1] - a) * (f32)(t - cur[0])) / (f32)(t1 - cur[0]);
                f = 255.0f * a;
                e->alpha = (u8)f;
                return 0;
            }
            cur = nx;
            nx += 2;
            t1 = *nx;
            if (t1 <= 0) break;
        }
    }
    e->on = 0;
    return -1;
}

/* ===== item box (0x1327D0-0x1332E4) ===== */
extern u8 lit_4892[], lit_4893[], lit_4894[], lit_4895[];
extern u8 pf_item_box_base[];
void DispFrameMessageA(void *, int, int);
void flps0004(void *);
int Pl_item_num_ck3(PLW *, u16);
f32 flSqrt(f32);
#define BOX_ID(i)   (*(u16 *)((u8 *)&game_w + 0x128 + (i) * 4))
#define BOX_NUM(i)  (*(s16 *)((u8 *)&game_w + 0x12A + (i) * 4))
#define BOX_FLAG(i) ((*(s32 *)((u8 *)&game_w + 0x1A8 + ((i) >> 5) * 4)) & (1 << ((i) & 0x1F)))

/* 0x1327D0 */
void trans_box(void) {
    PFLPS2 q;
    PFLPS3 r;
    char buf[0x28];
    PLW *pl = lpPit->pl;
    s16 i;
    long long k;
    int px;
    u16 id;
    s16 num;
    s16 sx;
    f32 t;
    u32 a;

    if (*((u8 *)pl + 0x8C2) != 0) {
        flSetRenderState(0x60, 0);
        DispFrameMessageA(pf_item_box_base, 0, 0xFF);
        r.col = 0xFF200000;
        for (k = 0; k < 0x20; k = (s16)(k + 1)) {
            s16 kk = k;
            sx = (s16)(0.8f * (313.0f + 36.0f * (f32)(kk & 7))) + 4;
            r.s[0] = sx;
            r.s[2] = 25.6f + (f32)sx;
            r.s[1] = ((kk >> 3) << 5) + 0x60;
            r.s[3] = r.s[1] + 0x1C;
            flps0004(&r);
        }
        SetFilterMode(0);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        q.s[2] = 0x20;
        q.s[3] = 0x20;
        for (i = 0; i < 0x20; i++) {
            id = BOX_ID(i);
            if (id != 0 && !BOX_FLAG(i)) {
                u8 *d = Item_data[id];
                if (d[5] != 0xFF) {
                    int n = d[5] + 1;
                    int uu = (n & 7) << 5;
                    int vv = (n >> 3) << 5;
                    q.s[0] = 0.8f * (313.0f + 36.0f * (f32)(i & 7));
                    q.s[1] = ((i >> 3) << 5) + 0x5E;
                    q.uv[0] = uu + 1;
                    q.uv[1] = vv + 1;
                    q.uv[2] = uu + 0x1F;
                    q.uv[3] = vv + 0x1F;
                    q.col = item_col_tbl[d[6]];
                    flps0008(&q);
                }
            }
        }
        if (lpPit->x64 > 0) {
            f32 p = (f32)lpPit->x64 / (f32)lpPit->x63;
            int n = Item_data[lpPit->x66][5] + 1;
            int uu = (n & 7) << 5;
            int vv = (n >> 3) << 5;
            q.s[0] = (0.8f * (313.0f + 36.0f * (f32)(lpPit->x65 & 7))) * p + 252.0f * (1.0f - p);
            q.s[1] = (((lpPit->x65 >> 3) << 5) + 0x5E) * p + 256.0f * (1.0f - p);
            q.uv[0] = uu + 1;
            q.uv[1] = vv + 1;
            q.uv[2] = uu + 0x1F;
            q.uv[3] = vv + 0x1F;
            q.col = item_col_tbl[Item_data[lpPit->x66][6]] & 0xFFFFFF;
            t = 57.0f * flSqrt((f32)lpPit->x64);
            q.col |= ((u32)t & 0xFF) << 24;
            flps0008(&q);
        }
        q.s[0] = 0.8f * (313.0f + 36.0f * (f32)(*((u8 *)pl + 0x8C3) & 7));
        q.s[1] = ((*((u8 *)pl + 0x8C3) >> 3) << 5) + 0x5E;
        q.uv[0] = 0;
        *(u32 *)&q.uv[2] = 0x200020;
        q.col = -1;
        flps0008(&q);
        if (lpPit->x60 > 0) {
            f32 f = (f32)(lpPit->x60 & 0x1F);
            a = (u32)(45.0f * flSqrt(f));
            t = 32.0f * (1.0f + 0.17960416f * flSqrt(31.0f - f));
            q.s[2] = t;
            q.s[3] = t;
            q.s[0] = (s16)(0.8f * (313.0f + 36.0f * (f32)(lpPit->x62 & 7))) - ((q.s[2] - 0x20) >> 1);
            q.s[1] = (((lpPit->x62 >> 3) << 5) + 0x5E) - ((q.s[3] - 0x20) >> 1);
            q.col = ((a & 0xFF) << 24) + 0xFFFFFF;
            flps0008(&q);
        }
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        q.s[0] = 0xFC;
        q.s[1] = 0x100;
        q.s[2] = 0x28;
        q.s[3] = 0x32;
        *(u32 *)&q.uv[0] = 0xEC0078;
        *(u32 *)&q.uv[2] = 0x0100008C;
        q.col = -1;
        flps0008(&q);
        flfntSetSize(0x12, 0x12);
        font_set_palette(0);
        flfntLocate(0x144, 0x46);
        font_print_uf(lit_4892);
        {
            u8 cur = *((u8 *)pl + 0x8C3);
            id = BOX_ID(cur);
            if (id != 0) {
                if (BOX_FLAG(cur)) goto none;
                num = BOX_NUM(cur);
                switch (Item_data[id][3]) {
                case 1:
                    sprintf(buf, (char *)item_str[id], BOX_NUM(cur));
                    break;
                case 0xFF:
                    sprintf(buf, (char *)lit_3659, item_str[id]);
                    break;
                default:
                    sprintf(buf, (char *)lit_4893, item_str[id], (s16)num);
                    break;
                }
                flfntLocate((s16)(0x1CB - (strlen(buf) * 9 >> 1)), 0xEE);
                font_print_uf(buf);
                {
                    long long px2 = -1;
                    s16 have = Pl_item_num_ck3(pl, id);
                    if (have < 0) {
                        px2 = 0x18C;
                        sprintf(buf, (char *)lit_4894);
                    } else if (have < (s16)num) {
                        px2 = 0x17A;
                        sprintf(buf, (char *)lit_4895);
                    }
                    if ((s16)px2 > 0) {
                        flfntLocate((int)px2, 0x10C);
                        font_set_palette(2);
                        font_print_uf(buf);
                    }
                }
            } else {
none:
                sprintf(buf, (char *)lit_3253, item_str[0]);   /* lw item_str: the first entry (was the table address: overflowed buf) */
                flfntLocate((s16)(0x1CB - (strlen(buf) * 9 >> 1)), 0xEE);
                font_print_uf(buf);
                id = 0;
            }
            Disp_help_mess(1, (u16)(id + 0x18));
        }
    }
}

/* ===== item window, select mode (0x12E910-0x12F830) ===== */
extern u8 item_select_base[];
extern u8 btn_item_sel_3360[];
extern s16 sy_tbl_3417[];
int Get_Use_itemnum(PLW *);
u16 item_sel_sub(PLW *, u16, int);
int Pl_shell_set(PLW *, u16, int);

/* 0x12E910 */
void disp_item_sub_select(void) {
    PFLPS3 q;
    PFLPS3 q2;
    PLW *pl = lpPit->pl;
    s16 n;
    s16 a0, b0, c0, d0, e0, f0, g0;
    u16 s18;
    u8 c;
    u8 sl[4];

    if (*((u8 *)pl + 2) == 1 || *((u8 *)pl + 2) == 5) {
        DispFrameMessage(item_select_base + 0x10, 0);
    } else {
        DispFrameMessage(item_select_base, 0);
    }
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    q.s[0] = 0x14F;
    q.s[2] = 0x94;
    q.s[1] = 0x18A;
    q.s[3] = 0x18;
    q.col = -1;
    q.uv0 = 0x9A00C5;
    q.uv1 = 0xB000E5;
    PutSpriteDiv3(&q, 0xC, 0xF);
    q.s[0] = 0x170;
    q.s[2] = 0x53;
    q.s[1] = 0x156;
    q.s[3] = 0x36;
    q.uv0 = 0x700000;
    q.uv1 = 0xA60068;
    flps0008(&q);
    if (lpPit->x58 != 0) {
        n = Get_Use_itemnum(pl);
        a0 = 0xFF;
        g0 = a0;
        f0 = a0;
        e0 = a0;
        d0 = a0;
        s18 = a0;
        if (lpPit->x59 < 0) {
            if (UseItemChk(pl, pl->work888) == 0) {
                n++;
            } else {
                s18 = pl->work888;
            }
            if (n >= 2) {
                d0 = item_sel_sub(pl, pl->work888, 0);
            }
        } else {
            if (UseItemChk(pl, pl->work888) == 0) {
                d0 = a0;
                n++;
            } else {
                d0 = pl->work888;
            }
            if (n >= 2) {
                s18 = item_sel_sub(pl, pl->work888, 1) & 0xFFFF;
            }
        }
        switch (n) {
        case 0:
        case 1:
        case 2:
            break;
        case 4:
            f0 = item_sel_sub(pl, s18, 1);
            e0 = item_sel_sub(pl, d0, 0);
            break;
        case 3:
            e0 = item_sel_sub(pl, d0, 0);
            f0 = item_sel_sub(pl, s18, 1);
            g0 = item_sel_sub(pl, e0, 0);
            break;
        case 5:
            e0 = item_sel_sub(pl, d0, 0);
            f0 = item_sel_sub(pl, s18, 1);
            g0 = item_sel_sub(pl, e0, 0);
            a0 = item_sel_sub(pl, f0, 1) & 0xFFFF;
            break;
        }
        q2.s[2] = 0x15;
        q2.s[1] = 0x147;
        q2.col = 0xFF808080;
        q2.s[3] = 0x36;
        ((s16 *)&q2.uv0)[1] = 0x31;
        ((s16 *)&q2.uv1)[0] = 0xC5;
        ((s16 *)&q2.uv1)[1] = 0x67;
        q2.s[0] = 0x14E;
        ((s16 *)&q2.uv0)[0] = 0xAA;
        flps0008(&q2);
        q2.s[0] = 0x1CF;
        ((s16 *)&q2.uv0)[0] = 0x8F;
        ((s16 *)&q2.uv1)[0] = 0xAA;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if ((u16)g0 != 0xFF) disp_item_icon((u8)g0, 0x1D4, 0, 1);
        if ((u16)a0 != 0xFF) disp_item_icon((u8)a0, 0x13E, 0, -1);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q2.s[2] = 0x2B;
        ((s16 *)&q2.uv1)[0] = 0xC5;
        q2.s[0] = 0x151;
        flps0008(&q2);
        q2.s[0] = 0x1B6;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if ((u16)e0 != 0xFF) disp_item_icon((u8)e0, 0x1BC, 0, 0);
        if ((u16)f0 != 0xFF) disp_item_icon((u8)f0, 0x157, 0, 0);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q2.s[0] = 0x171;
        flps0008(&q2);
        q2.s[0] = 0x197;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if ((u16)d0 != 0xFF) disp_item_icon((u8)d0, 0x19C, 0, 0);
        if (s18 != 0xFF) disp_item_icon((u8)s18, 0x176, 0, 0);
    } else {
        s16 cur = pl->work888;
        s16 hh = 0xFF;
        u16 s17 = 0xFF;
        s18 = 0xFF;
        a0 = cur;
        c0 = hh;
        b0 = hh;
        n = Get_Use_itemnum(pl);
        if (UseItemChk(pl, pl->work888) == 0) {
            a0 = hh;
            n++;
        }
        if (n == 4) {
            s18 = item_sel_sub(pl, pl->work888, 1) & 0xFFFF;
            b0 = item_sel_sub(pl, pl->work888, 0);
            c0 = item_sel_sub(pl, s18, 1);
        } else if (n == 2) {
            s18 = item_sel_sub(pl, pl->work888, 1) & 0xFFFF;
        } else if (n == 3) {
            b0 = item_sel_sub(pl, pl->work888, 0);
        } else if (n == 1 || n == 0) {
        } else {
            s18 = item_sel_sub(pl, pl->work888, 1) & 0xFFFF;
            b0 = item_sel_sub(pl, pl->work888, 0);
            c0 = item_sel_sub(pl, s18, 1);
            s17 = item_sel_sub(pl, b0, 0) & 0xFFFF;
        }
        q2.s[2] = 0x2B;
        q2.s[1] = 0x147;
        q2.s[3] = 0x36;
        q2.col = 0xFF808080;
        q2.uv0 = 0x31008F;
        q2.uv1 = 0x6700C5;
        q2.s[0] = 0x148;
        flps0008(&q2);
        q2.s[0] = 0x1BF;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if (s17 != 0xFF) disp_item_icon((u8)s17, 0x1C4, 0, 0);
        if ((u16)c0 != 0xFF) disp_item_icon((u8)c0, 0x14E, 0, 0);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q2.s[0] = 0x161;
        flps0008(&q2);
        q2.s[0] = 0x1A6;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if ((u16)b0 != 0xFF) disp_item_icon((u8)b0, 0x1AC, 0, 0);
        if (s18 != 0xFF) disp_item_icon((u8)s18, 0x167, 0, 0);
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q2.s[0] = 0x17E;
        q2.s[2] = 0x36;
        q2.s[3] = 0x44;
        q2.col = -1;
        flps0008(&q2);
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        if ((u16)a0 != 0xFF) disp_item_icon((u8)a0, 0x185, 1, 0);
    }
    reload_tex(1, 0x11A);
    SetTextureStage(0x11A);
    *(s16 *)(btn_item_sel_3360 + 2) = 0x16F;
    if (pl->work8F2 & 4) {
        *(s16 *)(btn_item_sel_3360 + 2) += 2;
    }
    *(s16 *)(btn_item_sel_3360 + 0xA) = 0x16F;
    if (pl->work8F2 & 2) {
        *(s16 *)(btn_item_sel_3360 + 0xA) += 2;
    }
    PutButtonICON(btn_item_sel_3360, 2);
    c = *((u8 *)pl + 2);
    if (c == 1 || c == 5) {
        reload_tex(1, 0x11A);
        SetTextureStage(0x11A);
        q.s[3] = 0x18;
        ((s16 *)&q.uv0)[1] = 0x1A;
        ((s16 *)&q.uv1)[1] = 0x30;
        if (lpPit->x5C != 0) {
            if (lpPit->x5D < 0) {
                sl[1] = Pl_shell_set(pl, pl->work88E, 1);
                sl[3] = 0xFF;
                sl[2] = 0xFF;
                sl[0] = 0xFF;
                if (sl[1] != 0xFF) {
                    if (pl->item[sl[1]].id == pl->item[pl->work88E].id) {
                        sl[1] = 0xFF;
                    } else {
                        flfntSetSize(0x14, 0x14);
                        if (disp_shell_name(sl[1], 0x116) == 0) {
                            sl[1] = 0xFF;
                        } else {
                            u8 v = Pl_shell_set(pl, sl[1], 1);
                            if (pl->item[v].id != pl->item[sl[1]].id) {
                                if (pl->item[v].id != pl->item[pl->work88E].id) {
                                    flfntSetSize(0x14, 0xA);
                                    if (disp_shell_name(v, 0x12D) == 1) {
                                        sl[0] = v;
                                        v = Pl_shell_set(pl, pl->work88E, 0);
                                        if (pl->item[v].id != pl->item[sl[0]].id) {
                                            if (disp_shell_name(v, 0xF1) == 1) sl[3] = v;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                flfntSetSize(0x14, 0x14);
                if (disp_shell_name(pl->work88E, 0xFE) == 1) {
                    sl[2] = pl->work88E;
                }
            } else {
                sl[0] = Pl_shell_set(pl, pl->work88E, 1);
                sl[3] = 0xFF;
                sl[2] = 0xFF;
                sl[1] = 0xFF;
                if (sl[0] != 0xFF) {
                    if (pl->item[sl[0]].id == pl->item[pl->work88E].id) {
                        sl[0] = 0xFF;
                    } else {
                        flfntSetSize(0x14, 0xA);
                        if (disp_shell_name(sl[0], 0x12D) == 0) {
                            sl[0] = 0xFF;
                        } else {
                            u8 v = Pl_shell_set(pl, pl->work88E, 0);
                            if (pl->item[v].id != pl->item[sl[0]].id) {
                                flfntSetSize(0x14, 0x14);
                                if (disp_shell_name(v, 0xFE) == 1) {
                                    sl[2] = v;
                                    v = Pl_shell_set(pl, sl[2], 0);
                                    if (pl->item[v].id != pl->item[sl[0]].id) {
                                        flfntSetSize(0x14, 0xA);
                                        if (disp_shell_name(v, 0xF1) == 1) sl[3] = v;
                                    }
                                }
                            }
                        }
                    }
                }
                flfntSetSize(0x14, 0x14);
                if (disp_shell_name(pl->work88E, 0x116) == 1) {
                    sl[1] = pl->work88E;
                }
            }
            q.s[1] = 0xEE;
            {
                int k = 3;
                s16 *sy = sy_tbl_3417 + 3;
                u8 *p = &sl[3];
                do {
                    if (*p != 0xFF) {
                        q.col = item_col_tbl[Item_data[pl->item[*p].id][6]];
                    } else {
                        q.col = 0xFF707070;
                    }
                    q.s[0] = 0x14F;
                    q.s[2] = 0x94;
                    q.s[3] = *sy;
                    ((s16 *)&q.uv0)[0] = 0xE0;
                    ((s16 *)&q.uv1)[0] = 0x100;
                    PutSpriteDiv3(&q, 9, 0xC);
                    q.s[1] += *sy;
                    k--;
                    p--;
                    sy--;
                } while (k >= 0);
            }
        } else {
            sl[0] = Pl_shell_set(pl, pl->work88E, 1);
            sl[2] = 0xFF;
            sl[1] = 0xFF;
            if (sl[0] != 0xFF) {
                u8 v;
                flfntSetSize(0x14, 0x14);
                v = Pl_shell_set(pl, pl->work88E, 0);
                if (pl->item[v].id != pl->item[sl[0]].id) {
                    if (disp_shell_name(v, 0xF0) == 1) sl[2] = v;
                }
                if (pl->item[sl[0]].id == pl->item[pl->work88E].id) {
                    sl[0] = 0xFF;
                } else if (disp_shell_name(sl[0], 0x120) == 0) {
                    sl[0] = 0xFF;
                }
            }
            if (disp_shell_name(pl->work88E, 0x108) == 1) {
                sl[1] = pl->work88E;
            }
            q.s[1] = 0xEE;
            {
                int k = 2;
                u8 *p = &sl[2];
                do {
                    if (*p != 0xFF) {
                        q.col = item_col_tbl[Item_data[pl->item[*p].id][6]];
                    } else {
                        q.col = 0xFF707070;
                    }
                    q.s[0] = 0x14F;
                    q.s[2] = 0x94;
                    PutSpriteDiv3(&q, 9, 0xC);
                    q.s[1] += 0x18;
                    k--;
                    p--;
                } while (k >= 0);
            }
        }
        lpPit->x57 = 1;
    }
}
