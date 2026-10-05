#ifndef PLF_H
#define PLF_H
/* Raw field access for the player work (PLW, 0xA00 bytes) used by the
 * decompiled player code (src/main/pl/pl*.c). Fields whose meaning is not
 * known yet are read by offset here instead of editing include/pl.h
 * (several people edit that header). PS16(pl, 0x2F4) = s16 at PLW+0x2F4. */
#include "types.h"
#define PS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define PU8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PU16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define PS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PU32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define PF32(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define PPTR(p, o) (*(void **)((u8 *)(p) + (o)))
#define GWU8(o) (*(u8 *)((u8 *)&game_w + (o)))

#include "pl.h"
#include "game.h"
#include "em.h"
extern s8 pl_cnt_w[16];
extern PLW player_work[];
extern s16 *pl_supp_tbl[36];
extern s8 equip_set[0x48];
extern u8 parts_max_tbl[0xC];
extern u32 test_hair_col[0xC];
extern f32 start_ofs[0x10];
void trans_pl_sub(void);
extern PLPROG *pl_prog_tbl[11];

void Pl_item_supply(PLW *, int, u16, s16);
int Pl_master_ck(PLW *);
int ran_suu(int);
int softdip_ck(int);
void weapon_create_model(u8, u16, int);
void armor_create_model(PLW *);
void yure_init(PLW *);
void *memset(void *, int, int);
void pl_move(void);
void pl_init(int);
int get_prim(void);
void *get_prim_ptr(s16);
void hit_chk_init(void);
void net_send_pl(PLW *, int, int);
void pl_init_sub(PLW *);
void eft01_set(PLW *, int);
void Eft06_set(f32, PLW *, int, int, int);
void cpRotMatrix(s32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void pl_work_clr(PLW *, u8, PLPROG *);
void Pl_act_set(PLW *, int, int, int);
void Pl_item_charge(PLW *);
void Pl_item_idx_calc(PLW *);
void Pl_light_init(PLW *);
void Pl_max_stamina_calc(PLW *, s16);
void Pl_ofs_set(PLW *, f32 *, int);
void Pl_reg_calc(PLW *);
u16 Pl_shell_set(PLW *, int, int);
s8 Pl_slash_lv_ck(PLW *, u16);
void Pl_view_reset(PLW *, int, int, int);
void Shell_type_set(PLW *, int);
void World_calc(PLW *);
void flvecCopy(f32 *, f32 *);
void frame_init(PLW *, u16, s16, int);
void normal_char_set(PLW *, int, int);
void parts_init(PLW *);
void pl_to_normal_clr(PLW *);
s32 skill_hp_calc_00134FF0(PLW *);
extern u8 Ken_data[234][0x18];
extern f32 stage_start_pos[88][3];
extern u16 stage_start_ang[88];
s32 Game_clear_ck(int);
s32 Pl_Skill_ck(PLW *, int);
s32 Pl_hold_item_ck(PLW *);
void Pl_item_stack(PLW *, int, int);
void Pl_stamina_calc(PLW *, int);
void Pl_stamina_reduce(PLW *);
void Pl_vital_calc(PLW *, s16);
s16 Stage_env_ck(u8);
s16 act_ck(PLW *, int, int);
s32 pl_flag_ck(PLW *, int);
void set01_set(int, int, int);
void unmei_se(PLW *);
void pl_flag_clr(PLW *, int);
void pl_flag_set(PLW *, int);
void timer_calc_sub_em(PLW *);
void timer_calc_sub_pl(PLW *);
void Eft06_set2(f32, PLW *, int, int, f32 *);
void func_639F20(PLW *);
s32 item_blank_ck(PLW *);
u16 item_sel_sub(PLW *, u16, int);
void se_req(int, int, int);
s32 Get_view_dir();
extern u8 Item_data[327][16];
int calc_vec_ang2(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void Pl_act_set2(PLW *, int, int, int);
void Pl_adj_calc(PLW *, int);
s32 Sansai_talk_ck(PLW *);
void SetVector(f32, f32, f32, f32 *);
extern f32 bed_ofs[];
void Share_item_conv(PLW *);
s32 St_unique_ck(PLW *, f32 *, u16 *, u8 *);
void func_549200(PLW *, int);
s32 Ext_pick_point_ck(PLW *);
s32 St_pick_ck(PLW *, u16 *, f32 *);
s32 Online_ck(void);
void Pl_chat_act_set(PLW *);
void item_action_set(PLW *, int);
void job_special_com_ck(PLW *, u8);
void search_act_set(PLW *, u8);
s16 stick_dir_set(PLW *, int);
s32 stick_pow_get(PLW *, int);
s32 trade_get_ck_00139680(PLW *);
void unique_act_set(PLW *);
typedef struct { u8 _00; u8 se_kind; u8 _02[2]; } PL_SHELL_DATA; /* see SHELL_DATA in shell06.h */
extern PL_SHELL_DATA Shell_data[];
#endif
