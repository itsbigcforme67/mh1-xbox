/* lb_v08 - lobby player name tags 0x005CB980-0x005CBF30: lb_disp_name (name, job icon and status icon above each avatar). Rewritten from the asm. */
#include "lobby_f.h"
extern u8 lb_quest_color_tbl[];
extern u8 D_3C738C[];
extern u16 System_timer;
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
int Lb_Pl_stg_ck();
int Lb_get_pl_stat2();
int Get_weapon_job();
void Lb_put_job();
void Lb_put_icon_free();
void Lb_put_status();
int strlen();
int Online_ck();
void lb_disp_name(u8 *arg0) {
    f32 v[3];
    f32 scr[4];
    f32 mat[16];
    u8 *pe;
    u8 *p;
    u8 *pl;
    u8 *c;
    char *name;
    u8 *c2;
    s16 len;
    int half;
    int off;
    s16 i;
    int px;
    int y2;
    int x2;
    int py;
    int y4;
    int pal;
    u8 f;
    pe = (u8 *)lb_player;
    switch (lb_sys.x68) {
    case 0:
    case 0xF:
    case 8:
        break;
    default:
        return;
    }
    font_set_stack_no(*(s32 *)(arg0 + 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    p = (u8 *)lb_player;
    i = 0;
    off = 0;
    do {
        pl = *(u8 **)pe;
        if (*pl != 0 && pl[1] != 0 && (Lb_Pl_stg_ck(pl) & 0xFF) && Lb_get_pl_stat2((s8)i) == 0 && (*(f32 *)(pl + 0xAC) != 0.0f || *(f32 *)(pl + 0xB4) != 0.0f)) {
            name = (char *)p + 4;
            if (Online_ck() == 1 && *(u8 *)0x39DAD4 != 0) {
                name = (char *)p + 0x24;
            }
            len = strlen(name);
            flmatInit(mat);
            flSetRenderState(0x1A, mat);
            v[0] = *(f32 *)(pl + 0xAC);
            v[1] = 190.0f + *(f32 *)(pl + 0xB0);
            v[2] = *(f32 *)(pl + 0xB4);
            flvecrRotTransPers(scr, v);
            if (scr[0] < 700.0f && !(scr[0] <= -60.0f)) {
                if (scr[1] < 500.0f && !(scr[1] <= -20.0f) && !(scr[3] <= 0.0f)) {
                    half = len / 2;
                    scr[0] -= (f32)(half * 8);
                    flfntLocate((int)(1.25f * (f32)(int)scr[0]), (int)scr[1]);
                    px = (s16)(int)(1.25f * (f32)(int)scr[0]);
                    py = (s16)(int)scr[1];
                    flfntSetSize(0x10, 0x10);
                    c = cw + off;
                    c2 = c + 0x1346;
                    f = c[0x1347];
                    if (f == 0x14) {
                        pal = 2;
                    } else if (f >= 0xD) {
                        pal = 6;
                    } else {
                        pal = 5;
                    }
                    font_set_stack_no(0);
                    font_print_double((int)(1.25f * (f32)(int)scr[0]), (int)scr[1], 1, pal, name);
                    if (i == *(u8 *)0x3F34C1) {
                        s8 job = Get_weapon_job(D_3C738C);
                        if (job == 5) {
                            job = 1;
                        }
                        y2 = (s16)py;
                        x2 = (s16)px;
                        y4 = y2 - 4;
                        Lb_put_job((s16)(x2 - 0x16), (s16)y4, 0x16, -1, job, 0);
                    } else {
                        y2 = (s16)py;
                        x2 = (s16)px;
                        y4 = y2 - 4;
                        Lb_put_job((s16)(x2 - 0x16), (s16)y4, 0x16, -1, *c2, 0);
                    }
                    f = c2[0x15];
                    if (f & 0xC0) {
                        if (!(f & 0x40)) {
                            if (!(System_timer & 0x10)) {
                                goto next;
                            }
                        }
                        Lb_put_icon_free((s16)(x2 + len * 4 - 0xC), (s16)(y2 - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (f & 0xF) * 4), 0xF);
                    } else if (f & 0x30) {
                        if ((f & 0x10) || (System_timer & 0x10)) {
                            Lb_put_icon_free((s16)(x2 + len * 4 - 0xC), (s16)(y2 - 0x1A), 0x16, *(s32 *)(lb_quest_color_tbl + (f & 0xF) * 4), 0xE);
                        }
                    } else {
                        Lb_put_status((s16)(x2 + len * 8 + 6), (s16)y4, 0x14, -1, c2[2]);
                    }
                }
            }
        }
next:
        p += 0x38;
        i = (s16)(i + 1);
        pe += 0x38;
        off += 0x2FC;
    } while (i < 8);
}
