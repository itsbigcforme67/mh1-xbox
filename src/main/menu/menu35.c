/* menu35 - f_menu 0x001333B0-0x0013354C: mix list */
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
int Get_Use_itemnum(PLW *);
u16 item_sel_sub(PLW *, u16, int);
int Pl_shell_set(PLW *, u16, int);


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
