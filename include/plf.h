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
#endif
