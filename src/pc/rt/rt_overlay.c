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

/* ported */
void set00_set(void);
void set14_set(void);
void func_6213F0(void) { set00_set(); }
void func_6229B0(void) { set14_set(); }
/* Set13_set (main, set13.c: sun glare) is linked directly. */

/* not ported yet */
STUB(func_61ED10, "Set03_set", (void))
STUB(func_61F2C0, "Set04_set", (void))
STUB(func_61F7E0, "Set05_set", (int stage))
STUB(func_6206D0, "set07_set", (void))
STUB(func_620B70, "set08_set", (void))
void set09_set(void);
void func_618CA0(void) { set09_set(); }
STUB(func_621C70, "Set10_set", (void))
STUB(func_622180, "set11_set", (void))
STUB(func_624050, "set15_set", (void))
STUB(func_625120, "set16_set", (void))

STUB(func_625630, "set17_set", (void))
STUB(func_625BB0, "Set18_set", (void))
STUB(func_6263B0, "Set19_set", (void))
STUB(func_626E70, "Set20_set", (int kind))
STUB(func_627840, "Set22_set", (void))
STUB(func_633B50, "Shell10_set", (f32 *pos, int a, int stage, int b))
STUB(func_54B8C0, "Eft14_set2", (f32 *pos, int kind))
