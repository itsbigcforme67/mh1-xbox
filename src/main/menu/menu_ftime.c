/* menu_ftime - f_menu disp (SLPM_654.95 0x0012B690-0x0012BC98): disp_name (the other hunters' names; matched by
 * agent B: field names, an s16 counter, the projected vector is f32[4] and its w is tested), Pit_disp_quest (the
 * quest page of the pit menu) and
 * font_print_quest_time, the quest timer as zenkaku "mm:ss". The `& 0xFFFFFFFF` and the int temps are what the
 * permuter found. Pit_disp_quest matched by agent B (12 Oct 2026): the page number read once into n, `return` in the
 * then-block of case 0, `break` after case 1; its buf is 40 bytes on the PS2 (frame size) and 64 on the PC. */
#include "g_alias.h"
#ifdef MHG_PS2 /* G calls another function here (tools/check.py VERSION=mhg) */
#define strlen func_001D5970
#endif
#include "menu.h"
#include "em.h"
#include "pl.h"
#include "fl.h"
#ifndef __MWERKS__
signed char Item_preparation_rate_0(void *, int);   /* (declared before its first use) */
#endif

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
#ifdef __MWERKS__
int Item_preparation_rate_0(void *, int);
#else
signed char Item_preparation_rate_0(void *, int);   /* the real return type (tools/check_kr_returns.py) */
#endif
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

/* The other hunters' names above their heads (online: the room member id). */
#if !defined(G_PART) || G_PART == 1
void disp_name(void) {
    PLW *pl;
    f32 pos[3];
    f32 scr[4];
    char *name;
    u8 mat[0x40];
    s16 i;

    flfntSetSize(0x10, 0x10);
    font_set_palette(0);
    pl = player_work;
    for (i = 0; i < 4; i++, pl++) {
        if (pl->id != game_w.master && game_w.pl_state[i] == 1 &&
            pl->be_flag != 0 && pl->x01 != 0 && (u8)Pl_stg_ck(pl)) {
            flmatInit((void *)mat);
            flSetRenderState(0x1A, (u32)mat);
            pos[0] = pl->pos[0];
            pos[1] = pl->pos[1] + 200.0f;
            pos[2] = pl->pos[2];
            flvecrRotTransPers(scr, pos);
            if (scr[0] < 700.0f && scr[0] > -60.0f) {
                if (scr[1] < 500.0f && scr[1] > -20.0f) {
                    name = (char *)pl + 0x8D4;
                    if (scr[3] > 0.0f) {
                        if (Online_ck() == 1 && PitMenu.x14 != 0) {
                            name = (char *)room_member_id + pl->id * 8;
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
#endif

/* The quest page of the pit menu: frame, title and the clear conditions, reward, time and map. */
#if !defined(G_PART)
void Pit_disp_quest(void) {
#ifdef __MWERKS__
    char buf[40];
#else
    char buf[64];   /* PC: room for the longest format string the game prints here */
#endif
    int n;

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
    n = lpPit->x43;
    *(void **)(pfl_quest + 0xC) = quest_str[n];
    DispFrameList(pfl_quest, buf, -1, n);
    DispFrameListOptionArrow(pfl_quest);
    font_set_palette(0);
    flfntLocate(0x167, 0x66);
    font_print_uf(Quest_str_get(0));
    switch (lpPit->x43) {
    case 0:
        if (game_w.x1DC == 0) {
            quest_condition_print(lpPit->x43);
            return;
        }
        func_5CB310(lpPit->x43);
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
        break;
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
#endif

#if !defined(G_PART) || G_PART == 2
void font_print_quest_time(int n) {
    char buf[8];
    u16 out[12];
    u16 *o;
    char *p;
    int t;
    int m;
    int s;

    t = Quest_time_get((u8)n);
    t = (t / 30) & 0xFFFFFFFF;
    m = t / 60;
    t = t - (m * 60);
    s = t;
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
#endif
