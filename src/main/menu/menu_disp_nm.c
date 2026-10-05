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
    char buf[32];

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
    char buf[8];
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
    char buf[16];
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
    char buf[8];
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
    char buf[8];
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
    char buf[8];
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
    char buf[32];
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
    if (Pl_Skill_ck(me, 0x2E) == 1 || Pl_item_num_ck(me, 0x8E) == 0) {
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
            if (*d >= 2) {
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
                q.col = item_col_tbl[Item_data[pl->item[(u8)pl->work88E].id][6]];
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

    if (m >= 2) {
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
    int y;
    u32 i;
    PLW *pl;

    player_info_sub(20.0f, lpPit->pl, 0x50);
    y = 0x6A;
    pl = player_work;
    for (i = 0; i < 4; i++, pl++) {
        if (*(u8 *)pl != 0 && i != game_w.master && game_w.pl_state[i] == 1) {
            player_info_sub(20.0f, pl, y);
            y = (s16)(y + 0x1A);
        }
    }
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
