/* menu_ftime - f_menu disp (SLPM_654.95 0x0012BBC0-0x0012BC98): font_print_quest_time, the quest timer as zenkaku "mm:ss". The `& 0xFFFFFFFF` and the
 * int temps are what the permuter found. Whole file in menu_disp_nm.c. */
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
