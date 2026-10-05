/*
 * rt_overlay.c - main -> game.bin overlay calls for the port runtime.
 *
 * Main-program C calls overlay functions by address (func_XXXXXX, see
 * src/main/stage/stage_set.c). Natively there is no overlay, so each such
 * name is routed here to the decompiled function, or to a stub that says
 * once that the code is not ported yet.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>

static void not_ported(const char *name)
{
    fprintf(stderr, "rt: %s not ported yet (skipped)\n", name);
}

#define STUB(fn, name, args) void fn args { static int once; if (!once++) not_ported(name); }

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

/* not ported yet */
STUB(func_633B50, "Shell10_set", (f32 *pos, int a, int stage, int b))
STUB(func_54B8C0, "Eft14_set2", (f32 *pos, int kind))
