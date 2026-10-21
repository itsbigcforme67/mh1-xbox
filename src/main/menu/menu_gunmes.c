/* menu_gunmes - f_menu disp (SLPM_654.95 0x001319D0-0x00131FA4): disp_cannon (the bowgun clip icons; PLW field names,
 * not byte offsets, so the loop does not keep field addresses), disp_gun_load_mess, the blinking "loading" text of
 * the bowguns, and disp_menu, the pit menu frame (page title, blinking arrow). Matched by agent B (12 Oct 2026): the
 * blink phases are u32, the text position before the colour; disp_menu keeps the page in s16 n / base / k. */
#include "g_alias.h"
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
#ifdef __MWERKS__
int Pl_item_num_ck(PLW *, int);
#else
short Pl_item_num_ck(PLW *, int);   /* the real return type (tools/check_kr_returns.py) */
#endif
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





/* ===== item window and gauges (0x12E3E0-0x1306A0) ===== */
#include "version.h"
#ifndef __MWERKS__
short Get_Use_itemnum(PLW *);   /* (declared before its first use) */
short Pl_item_num_ck(PLW *, int);   /* (declared before its first use) */
signed char Item_preparation_rate_0(void *, int);   /* (declared before its first use) */
unsigned char Pl_slash_lv_ck(PLW *);   /* (declared before its first use) */
unsigned short Pl_shell_set(PLW *, u16, int);   /* (declared before its first use) */
#endif
extern u8 Item_data[][ITEM_ROW];
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








extern u8 pl_type_uv[];
extern u16 uv_tbl_3869[][4];
void flps0002(void *);
typedef struct PFLPS1 {
    s16 s[4];
    u32 col;
} PFLPS1;


/* ===== timer, gauges (0x1306A0-0x131580) ===== */
extern u16 System_timer;
extern u8 needle_data[][0x24];
void flSinCos(f32, f32 *, f32 *);
f32 flSin(f32);
void gage_disp(void *, int);
void bar_disp(void *, int);
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
#ifdef __MWERKS__
int Pl_slash_lv_ck(PLW *);
#else
unsigned char Pl_slash_lv_ck(PLW *);   /* the real return type (tools/check_kr_returns.py) */
#endif
int PachingerCamChk(PLW *);
s8 GetPachingerInfo(PLW *, u8 *, u8 *, f32 *);
void Put_sprite_rotate(void *, s8, ...);
f32 flCos(f32);
#ifdef __MWERKS__
int Pl_shell_set(PLW *, u16, int);
#else
unsigned short Pl_shell_set(PLW *, u16, int);   /* the real return type (tools/check_kr_returns.py) */
#endif
void disp_gun_load_mess(int);
void func_5B4B20();
int Game_clear_ck(int);
void DispFrameListOptionArrowC(void *, u32);
void Disp_name_or_id(int);
void Reibun_print(int, int);
void lb_disp_chat_cnfg_sendpl(int, PIT_W *);
void font_print_strings(s16, s16, void *, int);
void disp_menu(int, PIT_W *);








/* ===== wyvern ripple, data/option windows (0x132510-0x133A00) ===== */
extern u8 pfl_menu_data[];
extern u8 pf_mix_list_base[], pf_monster_list_base[];
extern u8 lit_4947[], lit_4948[], lit_4949[], lit_4985[], lit_4986[], lit_4987[], lit_5012[], lit_5054[];
extern u8 mix_level_color[];
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
void EquipmentDescriptionWindow(int, int, int, u8);
void PlayerEquipmentWindow(PLW *);
void flps0004(void *);
int menu_equip_get_equip(u8);
void efct_circle(int, int, int, f32, f32);







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
    s8 on;          /* 0x00 */
    s8 show;        /* 0x01 */
    u8 alpha;       /* 0x02 */
    s8 delay;       /* 0x03 */
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


/* ===== item window, select mode (0x12E910-0x12F830) ===== */
extern u8 item_select_base[];
extern u8 btn_item_sel_3360[];
extern s16 sy_tbl_3417[];
#ifdef __MWERKS__
int Get_Use_itemnum(PLW *);
#else
short Get_Use_itemnum(PLW *);   /* the real return type (tools/check_kr_returns.py) */
#endif
u16 item_sel_sub(PLW *, u16, int);
#ifdef __MWERKS__
int Pl_shell_set(PLW *, u16, int);
#else
unsigned short Pl_shell_set(PLW *, u16, int);   /* the real return type (tools/check_kr_returns.py) */
#endif


/* 0x12BCA0 */

/* The bowgun ammo display: one icon per shot in the clip (loaded / empty), the loading messages. */
#if !defined(G_PART) || G_PART == 1
void disp_cannon(void) {
    PFLPS2 q;
    PLW *pl;
    f32 x;
    u32 i;

    pl = lpPit->pl;
    if (pl->kind == 1 || pl->kind == 5) {
        SetFilterMode(0);
        reload_tex(1, GSLOT(0x11A, 0x141));
        SetTextureStage(GSLOT(0x11A, 0x141));
        if (pl->work8BC == 0) {
            if (pl->flag12 != 0 && (u16)Pl_shell_set(pl, pl->work88E, 1) != 0xFF) {
                disp_gun_load_mess(2);
            }
            return;
        }
        if (lpPit->x2A != 0) {
            x = 120.0f;
            q.s[1] = 0x32;
        } else {
            x = 320.0f - 6.0f * (f32)pl->work01D;
            q.s[1] = 0x123;
        }
        q.s[2] = 9;
        q.s[3] = 0x14;
        q.col = -1;
        q.uv[1] = 0xEC;
        q.uv[3] = 0xFF;
        for (i = 0; i < pl->work01D; i++, x += 12.0f) {
            if (i < pl->work01C) {
                q.uv[0] = 0xCC;
                q.uv[2] = 0xD8;
            } else {
                q.uv[0] = 0xE0;
                q.uv[2] = 0xEC;
            }
            q.s[0] = 0.8f * x;
            flps0008(&q);
        }
        if (pl->work01C <= 0 && pl->flag12 != 0) {
            disp_gun_load_mess(0);
        }
        if (pl->char0 == 0x57C) {
            disp_gun_load_mess(1);
        }
    }
}
#endif

/* The bowgun's "loading" message (row n of gun_load_mess), blinking with the system timer. */
#if !defined(G_PART) || G_PART == 1
void disp_gun_load_mess(int n) {
    PFLPS2 q;
    s16 *m = gun_load_mess[n];
    u32 v = ((System_timer & 0x1F) << 11) & 0xFFFF;
    int a = ((s8)(96.0f * flSin(0.0000958738f * (f32)v)) + 0x9F) & 0xFF;

    if (lpPit->x2A != 0) {
        q.s[0] = m[0];
        q.s[1] = 0x3A;
        q.col = ((a & 0xFF) << 24) | 0xFFFFFF;
    } else {
        int b = a & 0xFF;
        q.s[0] = m[7];
        q.s[1] = 0xD5;
        q.col = b | ((b << 16) | 0xFF000000 | (b << 8));
    }
    q.s[2] = m[1];
    q.s[3] = m[2];
    q.uv[0] = m[3];
    q.uv[1] = m[4];
    q.uv[2] = m[5];
    q.uv[3] = m[6];
    flps0008(&q);
}
#endif

/* The pit menu frame: page title (n of the five-item pages), the arrow blinking while the menu has the pad. */
#if !defined(G_PART)
void disp_menu(int sw, PIT_W *p) {
#ifdef __MWERKS__
    char buf[32];
#else
    char buf[48];   /* PC: the title " ~C05menu  ~C00%01d/..." with its colour codes */
#endif
    s16 n;
    s16 k;
    u32 col;
    s16 base;
    u32 v;

    if (game_w.x1DC != 0) {
        func_5B4B20(sw);
        return;
    }
    n = lpPit->x41 / 5;
    base = n * 5;
    SetTrnslMode(4, 5);
    if (n == 0) {
        *(void **)(pfl_menu + 0xC) = menu_str_002EF860;
    } else {
        k = 1;
        if (Online_ck() == 0) {
            k++;
        }
        if (Game_clear_ck(0) == 1) {
            k += 2;
        }
        *(void **)(pfl_menu + 0xC) = menu_str_002EF860 + k * 0x14;
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
    DispFrameList(pfl_menu, buf, lpPit->x41 - base);
    DispFrameListOptionArrowC(pfl_menu, col);
    if (Online_ck() == 1 && (s8)sw == 0) {
        Disp_name_or_id(0xD1);
    }
}
#endif
