/*
 * rt_abi.c - argument-order adaptors for game C that declares a callee
 * with its float arguments in another position than the definition.
 *
 * On the PS2 floats travel in f12.., integers and pointers in a0.., so
 * the order of float and int parameters in a declaration does not change
 * the code (the matching files are right as they are). On x86 every
 * argument is on the stack in order, so such calls must be re-ordered.
 * tools/build_pc.sh compiles the affected files with -DNAME=rtabi_NAME
 * (only in the PC build); each rtabi_NAME here takes the arguments in the
 * caller's declared order and calls the definition. Found with a -flto
 * build (-Wlto-type-mismatch), see docs/pc.md.
 */
#include "types.h"

typedef struct PLW PLW;

/* frame_check family: plf.h declares (f32, PLW *, int); the definition
 * (src/main/frame/f_frame_nm.c) is (FRW *, int n, f32) */
int frame_check(void *w, int n, f32 f);
int frame_check2(void *w, int n, f32 f);
int frame_check3(void *w, int n, f32 a, f32 b);
/* pl_snd01.c (matched) calls the real (work, n, f32) order under the alias frame_check_001263F0 */
int rtabi_frame_check_real(void *w, int n, f32 f) { return frame_check(w, n, f); }
int rtabi_frame_check(f32 f, PLW *pl, int n) { return frame_check(pl, n, f); }
int rtabi_frame_check2(f32 f, PLW *pl, int n) { return frame_check2(pl, n, f); }
/* include/lbnpc.h (EMW *, f32, int): the lobby NPC scripts */
int rtabi_frame_check2_em(void *w, f32 f, int n) { return frame_check2(w, n, f); }
int rtabi_frame_check3(f32 a, f32 b, PLW *pl, int n) { return frame_check3(pl, n, a, b); }

/* Eft06_set: plf.h (f32 scale, PLW *, int arg, int x05, int joint);
 * definition (src/main/eft/eft06b.c) (chr, s16 arg, x05, joint, f32 scale) */
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);
void rtabi_Eft06_set(f32 scale, PLW *pl, int arg, int x05, int joint) { Eft06_set(pl, (s16)arg, x05, joint, scale); }

/* Eft02_set6: plf.h (f32 scale, PLW *, int, int); definition
 * (src/main/eft/eft02b.c) (EMW *, int arg, int joint, f32 scale) */
void Eft02_set6(void *em, int arg, int joint, f32 scale);
void rtabi_Eft02_set6(f32 scale, PLW *pl, int arg, int joint) { Eft02_set6(pl, arg, joint, scale); }

/* hit_point_cbd: src/main/stage/f_stage.c declares (f32 h, f32 w, f32 *p,
 * f32 *a, f32 *b); definition (src/main/hit/hit3.c) (p, a, b, h, w) */
u8 hit_point_cbd(f32 *p, f32 *a, f32 *b, f32 h, f32 w);
int rtabi_hit_point_cbd(f32 h, f32 w, f32 *p, f32 *a, f32 *b) { return hit_point_cbd(p, a, b, h, w); }

/* GetGroundHitStatusAreaPl: pl_move_sub (src/main/pl/pl_nm.c) passes four
 * arguments; the definition (src/main/hit/shit8_nm.c) has a fifth, f32
 * *flag, which it writes when off the ground (on the PS2 t0 holds
 * whatever the caller left there). The adaptor gives it a dummy. */
int GetGroundHitStatusAreaPl(void *ent, f32 *pos, void *at, f32 *out, f32 *flag);
int rtabi_GetGroundHitStatusAreaPl(void *ent, f32 *pos, void *at, f32 *out)
{
    f32 dummy;
    return GetGroundHitStatusAreaPl(ent, pos, at, out, &dummy);
}

/* Monster code (game.bin em_master_nm.c, agent B's em01_ai_nm.c): */
/* em_frame_check: em01_ai_nm.c (EMW *, f32 f, int n); definition (w, n, f) */
int em_frame_check(void *w, int n, f32 f);
int rtabi_em_frame_check(void *w, f32 f, int n) { return em_frame_check(w, n, f); }
/* em_frame_check2: em03.c (EMW *, f32 f, int n); definition (w, n, f) */
int em_frame_check2(void *w, int n, f32 f);
int rtabi_em_frame_check2(void *w, f32 f, int n) { return em_frame_check2(w, n, f); }
/* Eft13_set_em_scl: em01_ai_nm.c (EMW *, int j, f32 scale, int arg);
 * definition (src/main/eft/eft13e.c) (chr, s16 j, int arg, f32 scale) */
void Eft13_set_em_scl(void *chr, s16 j, int arg, f32 scale);
void rtabi_Eft13_set_em_scl(void *em, int j, f32 scale, int arg) { Eft13_set_em_scl(em, (s16)j, arg, scale); }
/* Eft15_set3: em01_ai_nm.c (EMW *, int arg, f32 scale, int x07);
 * definition (src/game/eft/eft15.c) (em, arg, x07, scale) */
void Eft15_set3(void *em, int arg, int x07, f32 scale);
void rtabi_Eft15_set3(void *em, int arg, f32 scale, int x07) { Eft15_set3(em, arg, x07, scale); }
/* Eft02_set3: em_master_nm.c (f32 scale, EMW *, ang, arg, joint, f32 *pos);
 * definition (src/main/eft/eft02b.c) (em, ang, arg, joint, pos, scale) */
void Eft02_set3(void *em, int ang, int arg, int joint, f32 *pos, f32 scale);
void rtabi_Eft02_set3(f32 scale, void *em, int ang, int arg, int joint, f32 *pos) { Eft02_set3(em, ang, arg, joint, pos, scale); }

/* NextStage_No_Set: em_core_nm.c calls it with no arguments (a0 = em left
 * over in the asm); the definition (agent D's em_cmd_nm.c) takes em. */
void NextStage_No_Set(void *em);
void rtabi_NextStage_No_Set(void *em) { NextStage_No_Set(em); }

/* Eft10_set: em07_ai_nm.c / em08_ai_nm.c call it as (f32 scale, EMW *, arg,
 * x07) (PS2: scale in f12, em in a0); the definition (src/game/eft/eft10.c)
 * is (em, arg, x07, scale). Without this the Lao-Shan's dying dust crashed. */
void Eft10_set(void *em, int arg, int x07, f32 scale);
void rtabi_Eft10_set(f32 scale, void *em, int arg, int x07) { Eft10_set(em, arg, x07, scale); }

/* em12_nm.c (em12_blood_req) calls Eft02_set4(scale, a, ang, 3, pos) as m2c read the asm (scale in f12 on the PS2); the
 * definition (src/main/eft/eft02b.c) is (a, ang, arg, pos, scale). Without this the Aptonoth's blood spray, and so a
 * Rathalos hunt that passed one, crashed on garbage arguments (quest 139). */
void Eft02_set4(unsigned short a, int ang, int arg, float *pos, float scale);
void rtabi_Eft02_set4(float scale, int a, int ang, int arg, float *pos) { Eft02_set4((unsigned short)a, ang, arg, pos, scale); }

/* ItemboxWindowX: the shop lists (lb_by139.c, lb_shp.c) declare (f32 x, s16 cur, int flags); the definition
 * (src/lobby/f/lb_ib.c) is (int cur, int flags, f32 base). Without this the weapon shop's sell list
 * (the item-box style slot grid) crashed with garbage arguments. */
void ItemboxWindowX(int cur, int flags, f32 base);
void rtabi_ItemboxWindowX(f32 x, s16 cur, int flags) { ItemboxWindowX(cur, flags, x); }
