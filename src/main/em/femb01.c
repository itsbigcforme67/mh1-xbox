/* SLPM_654.95 0x0010EC00-0x0010EC18: common_local_init .. dummy_em_prog. See f_em_nm.c. */
#include "em.h"
#include "game.h"

#define EB(o) (*(u8 *)((u8 *)em + (o)))
#define ESB(o) (*(s8 *)((u8 *)em + (o)))
#define EH(o) (*(s16 *)((u8 *)em + (o)))
#define EW(o) (*(s32 *)((u8 *)em + (o)))
#define EF(o) (*(f32 *)((u8 *)em + (o)))
#define EP(o) (*(void **)((u8 *)em + (o)))

typedef void (*EM_PROG)(EMW *);

extern EM_PROG *em_prog_tbl[];
extern void enemy_trans();
extern u8 enemy_size[];
extern f32 enemy_scale[];
extern struct EM_AREA *em_pos_tbl[];
extern s32 *em_hungry_tbl[];
extern s32 *em_thirst_tbl[];
extern s32 *em_suimin_tbl[];
extern f32 *em_ikari_data_tbl[];
extern s16 em_atk_mode_timer_tbl[];
extern u8 ot0[];
extern u8 ot1[];
extern u8 player_work[];

typedef struct QUEST_W_EM {
    u8 _pad00[8];
    s16 no;             /* 0x08 quest number (0: free hunt) */
} QUEST_W_EM;
extern QUEST_W_EM quest_w;

s16 get_prim(void);
void *get_prim_ptr(s16);
void add_prim(void *ot, void *prim, int pri, int sub);
void em_work_set(EMW *);
void em_cmd_init(EMW *);
void Em_hagi_point_clr(EMW *);
void Em_Taisei_Set(EMW *);
f32 em_def_attack_set(EMW *);
f32 em_def_defence_set(EMW *);
void em_wall_bit_set(EMW *);
void em_search_data_set(EMW *, u8);
void em_range_set(EMW *, s8);
void Ikari_Data_Set(EMW *);
void em_tsuushin_set(EMW *);
int Em_max_parts_get(int);
void em_dur_init(EMW *);
void World_calc(void *);
void eft01_set(void *, int);
void Quest_enemy_die(EMW *);
void push_em_work(EMW *);

void Em_Master_Change(EMW *);   /* a0 = em still (func_5395F0 in the asm) */
int Online_ck(void);
void net_receive_em_act(EMW *);
void pl_timer_calc(void *);
void hit_stop_calc(void *);
int GetGroundHitStatusAreaEm(EMW *, f32 *, void *, f32 *, f32 *);
void kehai_set(EMW *);
void Stage_Hate_Add(EMW *);
void kehai_ck(EMW *);
s8 em_eye_search_set(EMW *);
int em_cancel_act_ck(EMW *, u8);
int smell_ck(EMW *, int);
void ikari_flag_set(EMW *);
void em_escape_action_ck(EMW *);
u8 pl_ninshiki_ck(EMW *);
int em_pl_pos_set(EMW *, u8, f32 *);
void SetVector(f32 *, f32, f32, f32);
int Em_Unko_Smoke_Ck(EMW *);
void em_eye_dmg_reset_act_set(EMW *);
void Em_Taisei_Ck(EMW *);
void cpRotMatrixYXZ2(s32 *, f32 *);
void em_neck_move(EMW *);
void Em_Damage_Hate_Set(EMW *);
void Em_Hate_Ck(EMW *);
void pl_flag_set(void *, int);
void pl_flag_clr(void *, int);
void frame_move(void *);
void HitWallPlayer(void *, int);
void GetEmMaterialData(EMW *);
f32 flArcTan2(f32, f32);
int flConvertRtoS(f32);

void em01_init(EMW *); void em01_main(EMW *);
void em02_init(EMW *); void em02_main(EMW *);
void em03_init(EMW *); void em03_main(EMW *);
void em04_init(EMW *); void em04_main(EMW *);
void em07_init(EMW *); void em07_main(EMW *);
void em08_init(EMW *); void em08_main(EMW *);
void em09_init(EMW *); void em09_main(EMW *);
void em10_init(EMW *); void em10_main(EMW *);
void em12_init(EMW *); void em12_main(EMW *);
void em14_init(EMW *); void em14_main(EMW *);
void em15_init(EMW *); void em15_main(EMW *);
void em16_init(EMW *); void em16_main(EMW *);
void em17_init(EMW *); void em17_main(EMW *);
void em18_init(EMW *); void em18_main(EMW *);
void em19_init(EMW *); void em19_main(EMW *);
void em20_init(EMW *); void em20_main(EMW *);
void em21_init(EMW *); void em21_main(EMW *);
void em27_init(EMW *); void em27_main(EMW *);
void em29_init(EMW *); void em29_main(EMW *);
void em33_init(EMW *); void em33_main(EMW *);

void em_die(EMW *em);
void em_erase(EMW *em);
void em_effect_move(EMW *em);











/* 0x10EC00 */
void common_local_init(EMW *em) {
    eft01_set(em, 0);
}

/* 0x10EC10 */
void dummy_em_prog(EMW *em) {
}
