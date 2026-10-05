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
