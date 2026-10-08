/*
 * rt_pick.h - what is drawn: tags for the in-game bug reporter (F8). Portable part (rt_pick.c); the
 * interactive part is src/pc/pick.c (PC front-end, not in the Xbox build).
 *
 * Normal frames pay one branch per tag call (gfx_pick_pass is 0). While the frozen frame is drawn again for
 * the id buffer, the draw sites tag what they draw with PICK(kind, a, b, c, d) (+ pick_tag_ext for a code
 * pointer, an object and a world position); the backend then asks pick_register for an id per draw call.
 */
#ifndef RT_PICK_H
#define RT_PICK_H
#include <stdint.h>
#include "../gfx/gfx.h"

enum { PK_OTHER, PK_STAGE, PK_SKY, PK_SET, PK_MONSTER, PK_HUNTER, PK_NPC, PK_WEAPON, PK_EFT, PK_SHELL,
       PK_PRIM, PK_HUD, PK_TEXT, PK_MOVIE, PK_FADE, PK_N };

typedef struct {
    int kind;
    int a, b, c, d;           /* kind-specific: see rt_pick.c pick_describe */
    const void *fn;           /* code pointer (the prim's trans function) for the symbol name */
    const void *obj;          /* fl_skel * of a skinned model (nearest bone) */
    float pos[3];
    int has_pos;
    int clay_handle;          /* the game's clay handle (flExecuteClay), -1 none */
} pick_tag_t;

typedef struct {
    pick_tag_t tag;
    gfx_pick_info gi;
} pick_entry_t;

void pick_tag_full(int kind, int a, int b, int c, int d, const void *fn, const void *obj, const float *pos);
#define PICK(kind, a, b, c, d) do { if (gfx_pick_pass) pick_tag_full((kind), (a), (b), (c), (d), 0, 0, 0); } while (0)
#define PICK_FN(kind, a, b, c, d, fn, obj, pos) do { if (gfx_pick_pass) pick_tag_full((kind), (a), (b), (c), (d), (fn), (obj), (pos)); } while (0)
void pick_tag_handle(int handle);                 /* flExecuteClay: the game's clay handle */
#define PICK_HANDLE(h) do { if (gfx_pick_pass) pick_tag_handle(h); } while (0)
/* the game's clay handles of the area and set models (rt_bind_*_model): kind, a, b */
void pick_note_handle(int handle, int kind, int a, int b, int c);

/* session recording for the report: the pad state of every tick (one call per tick) as a --input script, and the
 * command line */
void rt_pick_record_pad(unsigned bits, int lx, int ly, int rx, int ry);
char *rt_pick_input_script(int *nticks);         /* malloc'ed script text: "xBITS:lx:ly:rx:ry*n,..." */
void rt_pick_set_args(int argc, char **argv);
const char *rt_pick_args(void);

void pick_pass_begin(void);                        /* forget the entries */
int pick_count(void);
const pick_entry_t *pick_entry(uint32_t id);       /* 1-based */
const char *pick_kind_name(int kind);
/* one line text for the panel, and a JSON object (appended to out, cap bytes) for the report */
void pick_describe(const pick_entry_t *e, char *out, int cap);
int pick_json(const pick_entry_t *e, char *out, int cap);
/* same object? (the panel lists one line per distinct object) */
int pick_same(const pick_entry_t *a, const pick_entry_t *b);
/* game state for the report: JSON object text, malloc'ed */
char *pick_game_json(void);
#endif
