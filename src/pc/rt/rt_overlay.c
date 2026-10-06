/*
 * rt_overlay.c - main -> game.bin overlay calls for the port runtime.
 *
 * Main-program C calls overlay functions by address (func_XXXXXX, see
 * src/main/stage/stage_set.c). Natively there is no overlay, so each such
 * name is routed here to the decompiled function.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>

/* ported (decompiled C in src/game/set) */
void Set03_set(void); void Set04_set(void); void Set05_set(u8 arg); void set07_set(void);
void set08_set(void); void set00_set(void); void set09_set(void); void Set10_set(void);
void set11_set(void); void set14_set(void); void set15_set(void); void set16_set(void);
void set17_set(void); void Set18_set(void); void Set19_set(void); void Set20_set(int arg);
void Set22_set(void);
void func_6213F0(void) { set00_set(); }
void func_61ED10(void) { Set03_set(); }
void func_61F2C0(void) { Set04_set(); }
void func_61F7E0(int stage) { Set05_set((u8)stage); }
void func_6206D0(void) { set07_set(); }
void func_620B70(void) { set08_set(); }
void func_618CA0(void) { set09_set(); }
void func_621C70(void) { Set10_set(); }
void func_622180(void) { set11_set(); }
void func_6229B0(void) { set14_set(); }
void func_624050(void) { set15_set(); }
void func_625120(void) { set16_set(); }
void func_625630(void) { set17_set(); }
void func_625BB0(void) { Set18_set(); }
void func_6263B0(void) { Set19_set(); }
void func_626E70(int kind) { Set20_set(kind); }
void func_627840(void) { Set22_set(); }
/* Set13_set (main, set13.c: sun glare) is linked directly. */

/* effects and shells (src/game/eft, src/game/shell) called from main C by
 * address; each forwards to the definition's own argument order. */
struct PLW;
void Shell10_set(f32 *pos, int arg, int stg, struct PLW *pl);
void Eft14_set2(f32 *pos, s16 arg);
void Eft17_set_ex(f32 *pos, int ang, int arg, f32 scale);
void Eft08_set(f32 *pos, int arg, int x07, f32 scale);
void Eft08_set2(struct PLW *pl, int arg, int x07, f32 scale, f32 y);
void shell01_set2(void *src, int arg);
void shell05_set3(void *src, int arg);
void func_633B50(f32 *pos, int a, int stage, int b) { Shell10_set(pos, a, stage, (struct PLW *)(uintptr_t)b); }
void func_54B8C0(f32 *pos, int kind) { Eft14_set2(pos, (s16)kind); }
void func_53FDF0(f32 *pos, u16 ang, int arg, f32 scale) { Eft17_set_ex(pos, ang, arg, scale); }
void func_544C90(f32 *pos, int arg, int x07, f32 scale) { Eft08_set(pos, arg, x07, scale); }
void func_544D20(void *pl, int arg, int x07, f32 scale, f32 y) { Eft08_set2(pl, arg, x07, scale, y); }
void func_628690(void *src, int arg) { shell01_set2(src, arg); }
void func_62A2C0(void *src, int arg) { shell05_set3(src, arg); }

/* Player code (src/main/pl, src/main/hit/hit_nm.c) -> game.bin effects,
 * shells and the player damage file (src/game/pl). */
struct EMW;
void shell00_set(struct PLW *pl, int arg);
void eft12_set(struct PLW *pl, s16 arg);
void shell03_set(struct PLW *pl, int arg, int x07);
void Eft18_set2(f32 *pos, s16 arg, int x07);
void Eft16_set(struct PLW *pl, int arg, s16 hit, f32 *pos, f32 scale);
void Eft05_set(struct PLW *pl, int timer, int arg);
void Eft15_set(f32 *pos, int arg, int ang, struct PLW *pl, f32 scale);
void Pl_die_set(struct PLW *pl);
int Guard_dir_ck(u16 ang, u16 dm_ang);
void Shell22_set(struct PLW *pl, u8 arg);
void Eft22_set(struct PLW *pl, int arg);
void eft00_set(struct EMW *em, int arg, f32 *pos, int x07);
void Eft16_set_impact(struct PLW *pl, f32 *pos, int arg, s16 hit, s16 wpn, f32 scale);
void eft11_set(struct EMW *em, f32 *pos, int arg);
void Pl_damage_sub(struct PLW *pl);
void shell06_set(struct PLW *pl, int unused, int joint);
void Eft24_set(struct PLW *pl, int arg);
void eft14_set(f32 *pos, s16 arg, f32 scale);
void func_6362B0(struct PLW *pl, int a) { shell00_set(pl, a); }
void func_549200(struct PLW *pl, int a) { eft12_set(pl, (s16)a); }
void func_628FB0(struct PLW *pl, int a, int b) { shell03_set(pl, a, b); }
void func_5547B0(f32 *pos, int a, int b) { Eft18_set2(pos, (s16)a, b); }
void func_5508F0(struct PLW *pl, int a, int hit, f32 *pos, f32 scale) { Eft16_set(pl, a, (s16)hit, pos, scale); }
void func_543690(struct PLW *pl, int t, int a) { Eft05_set(pl, t, a); }
void func_54D480(f32 *pos, int a, int ang, struct PLW *pl, f32 scale) { Eft15_set(pos, a, ang, pl, scale); }
void func_639F20(struct PLW *pl) { Pl_die_set(pl); }
int func_639FC0(u16 ang, u16 dm) { return Guard_dir_ck(ang, dm); }
void func_637F60(struct PLW *pl, int a) { Shell22_set(pl, (u8)a); }
void func_555A90(struct PLW *pl, int a) { Eft22_set(pl, a); }
void func_551790(struct PLW *pl, int a, f32 *pos, int b) { eft00_set((struct EMW *)pl, a, pos, b); }
void func_550DD0(struct PLW *pl, f32 *pos, int a, int hit, int wpn, f32 scale) { Eft16_set_impact(pl, pos, a, (s16)hit, (s16)wpn, scale); }
void func_546860(struct PLW *pl, f32 *pos, int a) { eft11_set((struct EMW *)pl, pos, a); }
void func_63A260(struct PLW *pl) { Pl_damage_sub(pl); }
void func_62A6C0(struct PLW *pl, int a, int joint) { shell06_set(pl, a, joint); }
void func_558A80(struct PLW *pl, int a) { Eft24_set(pl, a); }
void func_54B7E0(f32 *pos, int a, f32 scale) { eft14_set(pos, (s16)a, scale); }
/* Pl_piyo_ck (game 0x639DD0, 5 instructions): stun gauge +0x7AA >= 50 */
int Pl_piyo_ck(struct PLW *pl) { return *(s16 *)((u8 *)pl + 0x7AA) >= 50; }
int func_639DD0(struct PLW *pl) { return Pl_piyo_ck(pl); }
void Eft14_set4(struct PLW *pl, int arg);
void Eft21_set(struct PLW *pl, int arg);
void func_54BA40(struct PLW *pl, int a) { Eft14_set4(pl, a); }
void func_555020(struct PLW *pl, int a) { Eft21_set(pl, a); }

/* select.bin (character screen): CardCmsv08 (mccomb.c, the save of a new
 * hunter) copies the edited character into its card slot with
 * user_data_copy (0x534650, src/select/edit_nm.c) */
void user_data_copy(void *src, u8 slot);
void func_534650(void *src, u8 slot) { user_data_copy(src, slot); }
/* Init_task (0x533A00, the select overlay's entry task: a soft reset
 * starts the boot again with it) */
void Init_task(void *t);
void func_533A00(void *t) { Init_task(t); }
