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
int rtabi_frame_check(f32 f, PLW *pl, int n) { return frame_check(pl, n, f); }
int rtabi_frame_check2(f32 f, PLW *pl, int n) { return frame_check2(pl, n, f); }
int rtabi_frame_check3(f32 a, f32 b, PLW *pl, int n) { return frame_check3(pl, n, a, b); }

/* Eft06_set: plf.h (f32 scale, PLW *, int arg, int x05, int joint);
 * definition (src/main/eft/eft06b.c) (chr, s16 arg, x05, joint, f32 scale) */
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);
void rtabi_Eft06_set(f32 scale, PLW *pl, int arg, int x05, int joint) { Eft06_set(pl, (s16)arg, x05, joint, scale); }

/* Eft02_set6: plf.h (f32 scale, PLW *, int, int); definition
 * (src/main/eft/eft02_nm.c) (EMW *, int arg, int joint, f32 scale) */
void Eft02_set6(void *em, int arg, int joint, f32 scale);
void rtabi_Eft02_set6(f32 scale, PLW *pl, int arg, int joint) { Eft02_set6(pl, arg, joint, scale); }

/* hit_point_cbd: src/main/stage/f_stage.c declares (f32 h, f32 w, f32 *p,
 * f32 *a, f32 *b); definition (src/main/hit/hit3_nm.c) (p, a, b, h, w) */
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
