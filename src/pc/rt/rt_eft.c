/*
 * rt_eft.c - effect and shell services for the port runtime: the effect
 * work list (pull_eft_work, move_eft, trans_eft), the shell work list
 * (pull_shell_work, move_shell, trans_shell), the second prim pool
 * (get_prim2), the senko/smoke/smell stacks, the effect models (eft_mdlw)
 * and the small main-program helpers the eft and shell game C calls
 * (eft_vec_linear, eft_trans_sub, Eft_rendope_set, ...).
 *
 * Native re-implementations written from the PS2 asm (addresses given per
 * function). What is a guess or a simplification is marked as such.
 */
#include "rt.h"
#include "types.h"
#include "eft.h"
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(offsetof(EFTW, move) == 0x20 && offsetof(EFTW, prim) == 0x38, "EFTW layout");
_Static_assert(offsetof(SHLW, trans) == 0x14 && offsetof(SHLW, move) == 0x20 && offsetof(SHLW, prim) == 0xBC,
               "SHLW layout");

void flSetRenderState(int state, u32 value);
void flExecuteClay(int handle, int arg);
void clay_attr_set(s32 attr);
void clay_attr_reset(void);
void SetTrnslMode(int a, int b);
void SetOpeMode(int a);
void SetFilterMode(int a);
void flmatMakeScale(FLMAT *m, f32 x, f32 y, f32 z);
void flmatRotXYZ33(FLMAT *m, f32 x, f32 y, f32 z);
void flmatRotX33(FLMAT *m, f32 a);
void flmatRotY33(FLMAT *m, f32 a);
void flmatRotZ33(FLMAT *m, f32 a);
void flmatSetTrans(FLMAT *m, f32 x, f32 y, f32 z);
f32 flSin(f32);
f32 flCos(f32);
f32 flArcTan2(f32, f32);

/* ------------------------------------------------------------ work heap */
/* get_start_heap/get_heap_ptr (0x154F20): n consecutive 512-byte blocks of
 * a shared 256-block heap. Here each work entry gets its own zeroed
 * buffer (at least one block). */
#define HEAP_BLOCK 0x200
static void *heap_alloc(int n)
{
    return calloc((size_t)(n > 0 ? n : 1), HEAP_BLOCK);
}

/* ------------------------------------------------------------ effect list */
/* eft_work (0x390B10): 128 entries of 0x40 bytes, handed out from a free
 * stack (init_eft_work 0x100E98) and linked at +0x0C (prev) / +0x10 (next)
 * from eft_w_top. pull_eft_work (0x100FF0) links at the head,
 * pull_eft_work2 (0x1010C0) at the tail. */
#define EFT_N 128
#define EFT_SIZE 0x40
_Static_assert(sizeof(EFTW) == EFT_SIZE, "EFTW size");
EFTW eft_work[EFT_N];                /* eft23 walks it directly */
#define EFT_RAW(i) ((u8 *)&eft_work[i])
static u8 *eft_free[EFT_N];
static int eft_ctr;
static u8 *eft_w_top;
static void *eft_heap[EFT_N];

#define LNK_PREV(p) (*(u8 **)((p) + 0x0C))
#define LNK_NEXT(p) (*(u8 **)((p) + 0x10))

static void eft_init(void)
{
    int i;
    memset(eft_work, 0, sizeof eft_work);
    eft_w_top = NULL;
    for (i = 0; i < EFT_N; i++) {
        free(eft_heap[i]);
        eft_heap[i] = NULL;
        eft_free[i] = EFT_RAW(EFT_N - 1 - i);   /* pops the first entry first */
    }
    eft_ctr = EFT_N;
}

static u8 *eft_take(int n)
{
    u8 *p;
    int i;
    if (eft_ctr == 0)
        return NULL;
    p = eft_free[--eft_ctr];
    i = (int)(p - EFT_RAW(0)) / EFT_SIZE;
    memset(p, 0, EFT_SIZE);       /* the PS2 keeps old bytes; zero is the first-use state */
    free(eft_heap[i]);
    eft_heap[i] = heap_alloc(n);
    p[0] = 1;
    *(void **)(p + 0x18) = eft_heap[i];
    p[0x1D] = (u8)n;
    return p;
}

EFTW *pull_eft_work(int n)
{
    u8 *p = eft_take(n);
    if (!p)
        return NULL;
    LNK_PREV(p) = NULL;
    LNK_NEXT(p) = eft_w_top;
    if (eft_w_top)
        LNK_PREV(eft_w_top) = p;
    eft_w_top = p;
    return (EFTW *)p;
}

EFTW *pull_eft_work2(int n)
{
    u8 *p = eft_take(n), *q;
    if (!p)
        return NULL;
    LNK_NEXT(p) = NULL;
    if (!eft_w_top) {
        eft_w_top = p;
        LNK_PREV(p) = NULL;
    } else {
        for (q = eft_w_top; LNK_NEXT(q); q = LNK_NEXT(q))
            ;
        LNK_NEXT(q) = p;
        LNK_PREV(p) = q;
    }
    return (EFTW *)p;
}

/* push_eft_work (0x1011C0): unlink, back on the free stack, clear the
 * first 8 bytes and the prim fields. The next link stays, so the move
 * loop can go on after an effect removed itself. */
void push_eft_work(EFTW *ew)
{
    u8 *p = (u8 *)ew;
    if (!LNK_PREV(p))
        eft_w_top = LNK_NEXT(p);
    else
        LNK_NEXT(LNK_PREV(p)) = LNK_NEXT(p);
    if (LNK_NEXT(p))
        LNK_PREV(LNK_NEXT(p)) = LNK_PREV(p);
    if (eft_ctr < EFT_N)
        eft_free[eft_ctr++] = p;
    memset(p, 0, 8);
    *(s16 *)(p + 0x3C) = 0;
    *(void **)(p + 0x38) = NULL;
}

/* move_eft (0x101260) */
static void move_eft(void)
{
    u8 *p;
    for (p = eft_w_top; p; p = LNK_NEXT(p))
        if (p[0])
            ((EFTW *)p)->move((EFTW *)p);
}

/* trans_eft (0x1012B0) / trans_eft_up (0x101330): effects that draw
 * themselves through the callback at +0x14 (not through prims). Type 4
 * arg 0 is drawn by trans_eft_up, the others by trans_eft. */
static void trans_eft(int up)
{
    u8 *p;
    for (p = eft_w_top; p; p = LNK_NEXT(p)) {
        void (*fn)(EFTW *) = *(void (**)(EFTW *))(p + 0x14);
        if (!fn || !p[0] || !p[1])
            continue;
        if ((p[2] == 4 && p[3] == 0) == (up != 0))
            fn((EFTW *)p);
    }
}

/* ------------------------------------------------------------ shell list */
/* shell_work (0x398E70): 64 entries of 0xD4 bytes (init_shell_work), same
 * list scheme as effects (pull_shell_work 0x159090, push 0x159160). */
#define SHL_N 64
#define SHL_SIZE 0xD4
static union { SHLW w; u8 raw[SHL_SIZE]; } shl_pool[SHL_N];
static u8 *shl_free[SHL_N];
static int shl_ctr;
u8 *shell_w_top;          /* also read by hit_nm.c (shell hit checks) */
static void *shl_heap[SHL_N];

static void shl_init(void)
{
    int i;
    memset(shl_pool, 0, sizeof shl_pool);
    shell_w_top = NULL;
    for (i = 0; i < SHL_N; i++) {
        free(shl_heap[i]);
        shl_heap[i] = NULL;
        shl_free[i] = shl_pool[SHL_N - 1 - i].raw;
    }
    shl_ctr = SHL_N;
}

SHLW *pull_shell_work(int n)
{
    u8 *p;
    int i;
    if (shl_ctr == 0)
        return NULL;
    p = shl_free[--shl_ctr];
    i = (int)(p - shl_pool[0].raw) / SHL_SIZE;
    memset(p, 0, SHL_SIZE);
    free(shl_heap[i]);
    shl_heap[i] = heap_alloc(n);
    LNK_PREV(p) = NULL;
    LNK_NEXT(p) = shell_w_top;
    if (shell_w_top)
        LNK_PREV(shell_w_top) = p;
    shell_w_top = p;
    p[0] = 1;
    *(void **)(p + 0x18) = shl_heap[i];
    p[0x1C] = (u8)i;
    p[0x1D] = (u8)n;
    return (SHLW *)p;
}

void push_shell_work(SHLW *sh)
{
    u8 *p = (u8 *)sh;
    if (!LNK_PREV(p))
        shell_w_top = LNK_NEXT(p);
    else
        LNK_NEXT(LNK_PREV(p)) = LNK_NEXT(p);
    if (LNK_NEXT(p))
        LNK_PREV(LNK_NEXT(p)) = LNK_PREV(p);
    if (shl_ctr < SHL_N)
        shl_free[shl_ctr++] = p;
    memset(p, 0, 8);
}

/* move_shell (0x159210): +0x7B is a hit-stop counter that skips moves */
static void move_shell(void)
{
    u8 *p;
    for (p = shell_w_top; p; p = LNK_NEXT(p)) {
        if (!p[0])
            continue;
        if (p[0x7B])
            p[0x7B]--;
        else
            ((SHLW *)p)->move((SHLW *)p);
    }
}

/* trans_shell (0x159310): the draw callback at +0x14. The PS2 also checks
 * that the owner (player_work / em_work[+0x0A]) is in use; here only the
 * shell's own flags are checked (simplification). */
static void trans_shell(void)
{
    u8 *p;
    for (p = shell_w_top; p; p = LNK_NEXT(p)) {
        SHLW *sh = (SHLW *)p;
        if (p[0] && p[1] && sh->trans)
            sh->trans(sh);
    }
}

/* ------------------------------------------------------------ prim pool 2 */
/* get_prim2 (0x1693D0): 256 prims of 0x20, a slot is free while its trans
 * is 0. release_prim2 keeps +0x18/+0x1C (owner and index). */
#define PRIM2_N 256
static union { PRIM p; u8 raw[0x20]; } prim2[PRIM2_N];
static int prim2_top;

PRIM *get_prim_ptr2(s16 no)
{
    return no >= 0 && no < PRIM2_N ? &prim2[no].p : NULL;
}

int get_prim2(void)
{
    int i;
    for (i = prim2_top; i < PRIM2_N; i++)
        if (!prim2[i].p.trans) {
            prim2_top = i + 1;
            return i;
        }
    return -1;
}

void release_prim2(s16 no)
{
    PRIM *p = get_prim_ptr2(no);
    void *o;
    s32 k;
    if (!p)
        return;
    o = p->owner;
    k = p->no;
    memset(p, 0, 0x20);
    p->owner = o;
    p->no = k;
    if (no < prim2_top)
        prim2_top = no;
}

/* ------------------------------------------------------------ stacks */
/* push_senko/smoke/smell (0x16A570, 0x16A3D0, 0x16A2B0): up to 32 pointers
 * each, for the screen-flash, smoke and smell renderers. Those renderers are
 * not ported: the entries are kept, nothing draws them yet. */
void *senko_stack[32], *smoke_stack[32], *smell_stack[32];   /* the game's (em_core reads them) */
s8 senko_cnt, smoke_cnt, smell_cnt;

/* as 0x16A2B0: full at 32 (count), first free slot, count up */
static int stack_push(void **st, s8 *cnt, void *p)
{
    int i;
    if (*cnt >= 32)
        return 0;
    for (i = 0; i < 32; i++)
        if (!st[i]) {
            st[i] = p;
            (*cnt)++;
            return 1;
        }
    return 0;
}

static void stack_pull(void **st, s8 *cnt, void *p)
{
    int i;
    for (i = 0; i < 32; i++)
        if (st[i] == p) {
            st[i] = NULL;
            (*cnt)--;
        }
}

int push_senko(void *p) { return stack_push(senko_stack, &senko_cnt, p); }
void pull_senko(void *p) { stack_pull(senko_stack, &senko_cnt, p); }
int push_smoke(void *p) { return stack_push(smoke_stack, &smoke_cnt, p); }
void pull_smoke(void *p) { stack_pull(smoke_stack, &smoke_cnt, p); }
int push_smell(void *p) { return stack_push(smell_stack, &smell_cnt, p); }
void pull_smell(void *p) { stack_pull(smell_stack, &smell_cnt, p); }

/* ------------------------------------------------------------ eft helpers */
/* eft_vec_linear (0x1013B0): keyframes {t, x, y, z} ending with t = -1;
 * holds the value of an exact key, interpolates between keys. */
void eft_vec_linear(f32 t, f32 *key, f32 *out)
{
    for (; key[0] != -1.0f; key += 4) {
        if (t == key[0]) {
            out[0] = key[1];
            out[1] = key[2];
            out[2] = key[3];
            return;
        }
        if (!(t <= key[0]) && t < key[4]) {
            f32 r = (t - key[0]) / (key[4] - key[0]);
            out[0] = key[1] + r * (key[5] - key[1]);
            out[1] = key[2] + r * (key[6] - key[2]);
            out[2] = key[3] + r * (key[7] - key[3]);
            return;
        }
    }
}

/* eft_alpha_linear (0x101480): keyframes {t, a} ending with t = -1 */
void eft_alpha_linear(f32 t, f32 *key, f32 *out)
{
    for (; key[0] != -1.0f; key += 2) {
        if (t == key[0]) {
            out[0] = key[1];
            return;
        }
        if (!(t <= key[0]) && t < key[2]) {
            f32 r = (t - key[0]) / (key[2] - key[0]);
            out[0] = key[1] + r * (key[3] - key[1]);
            return;
        }
    }
}

/* eft_rgba_linear (0x101510): keyframes {s32 t, u32 rgba} ending with
 * t = -1; each byte channel interpolated (a + r * (b - a), truncated). */
void eft_rgba_linear(s32 *key, s32 t, u32 *out)
{
    for (; key[0] != -1; key += 2) {
        if (t == key[0]) {
            out[0] = (u32)key[1];
            return;
        }
        if (key[0] < t && t < key[2]) {
            f32 r = (f32)(t - key[0]) / (f32)(key[2] - key[0]);
            u32 a = (u32)key[1], b = (u32)key[3], c = 0;
            int sh;
            for (sh = 0; sh < 32; sh += 8) {
                u32 ca = (a >> sh) & 0xFF, cb = (b >> sh) & 0xFF;
                u32 d = (u32)(s32)(r * ((f32)cb - (f32)ca));
                c |= ((ca + d) & 0xFF) << sh;
            }
            out[0] = c;
            return;
        }
    }
}

/* make_mat_srt (0x1018C0): scale, then rotation picked by the flag bits
 * (2 X, 4 Y, 8 Z, 0xE XYZ), then translation. */
void make_mat_srt(f32 *scale, f32 *rot, f32 *trans, u16 flag, FLMAT *m)
{
    f32 tx = trans[0], ty = trans[1], tz = trans[2];
    flmatMakeScale(m, scale[0], scale[1], scale[2]);
    switch (flag & 0xE) {
    case 0xE:
        flmatRotXYZ33(m, rot[0], rot[1], rot[2]);
        break;
    case 2:
        flmatRotX33(m, rot[0]);
        break;
    case 4:
        flmatRotY33(m, rot[1]);
        break;
    case 8:
        flmatRotZ33(m, rot[2]);
        break;
    }
    flmatSetTrans(m, tx, ty, tz);
}

/* Eft_rendope_set (0x101D10): blend mode from an effect's flag word */
void Eft_rendope_set(int flag)
{
    flag &= 0xFFFF;
    if (flag & 0x10)
        SetFilterMode(0);
    switch (flag & 0xE) {
    case 2:
        SetTrnslMode(4, 1);
        break;
    case 4:
        SetTrnslMode(4, 5);
        break;
    case 8:
        SetTrnslMode(1, 1);
        SetOpeMode(2);
        break;
    }
}

/* Material_set_sub (0x169120) binds the clay's materials (fl states
 * 0x3A+n). Port clays carry their own texture, so nothing to do. */
void Material_set_sub(void *mat, CLAY *clay)
{
    (void)mat;
    (void)clay;
}

static u32 alpha_byte(f32 a)
{
    f32 v = 255.0f * a;
    return v >= 2147483648.0f ? ((u32)(s32)(v - 2147483648.0f) | 0x80000000u) & 0xFF : (u32)(s32)v & 0xFF;
}

/* eft_trans_sub (0x1019E0): draw one effect clay with world matrix m,
 * alpha a (fade colour alpha; flag bit 1 keeps the colour black, else
 * white) and the effect's blend flag. */
int eft_trans_sub(CLAY *clay, FLMAT *m, u16 flag, f32 a, void *mat)
{
    if (!clay || clay->handle == -1)
        return 0;
    flSetRenderState(0x60, 0);
    flSetRenderState(0x1A, (u32)m);
    if (flag & 1)
        flSetRenderState(0x67, alpha_byte(a) << 24);
    else
        flSetRenderState(0x67, alpha_byte(a) << 24 | 0xFFFFFF);
    Material_set_sub(mat, clay);
    clay_attr_set(clay->attr);
    Eft_rendope_set(flag);
    flExecuteClay(clay->handle, 0);
    clay_attr_reset();
    return 1;
}

/* eft_trans_sub_col (0x101B90): same with a full RGBA fade colour */
int eft_trans_sub_col(CLAY *clay, FLMAT *m, u32 col, u16 flag, void *mat)
{
    if (!clay || clay->handle == -1)
        return 0;
    flSetRenderState(0x60, 0);
    flSetRenderState(0x1A, (u32)m);
    flSetRenderState(0x67, col);
    Material_set_sub(mat, clay);
    clay_attr_set(clay->attr);
    Eft_rendope_set(flag);
    flExecuteClay(clay->handle, 0);
    clay_attr_reset();
    return 1;
}

/* eft_trans_sub_opa (0x101C60): opaque, alpha reference 0x80 */
int eft_trans_sub_opa(CLAY *clay, FLMAT *m, void *mat)
{
    if (!clay || clay->handle == -1)
        return 0;
    flSetRenderState(0x60, 0x80);
    flSetRenderState(0x1A, (u32)m);
    Material_set_sub(mat, clay);
    clay_attr_set(clay->attr);
    flExecuteClay(clay->handle, 0);
    clay_attr_reset();
    return 1;
}

/* ------------------------------------------------------------ vectors */
void SetVector(f32 *v, f32 x, f32 y, f32 z) { v[0] = x; v[1] = y; v[2] = z; }
void AddVector(f32 *d, f32 *a, f32 *b) { d[0] = a[0] + b[0]; d[1] = a[1] + b[1]; d[2] = a[2] + b[2]; }
void ScaleVector(f32 *d, f32 *a, f32 s) { d[0] = a[0] * s; d[1] = a[1] * s; d[2] = a[2] * s; }
/* PointToPoint (g_cpAng2Rad): d = a - b (checked against the asm; was b - a) */
void PointToPoint(f32 *d, f32 *a, f32 *b) { d[0] = a[0] - b[0]; d[1] = a[1] - b[1]; d[2] = a[2] - b[2]; }

/* flvecRotX (0x172FF0): rotate v about X by a radians */
void flvecRotX(f32 *v, f32 a)
{
    f32 s = flSin(a), c = flCos(a), y = v[1], z = v[2];
    v[1] = y * c - z * s;
    v[2] = y * s + z * c;
}

/* ------------------------------------------------------------ actors */
/* Joints: on the PS2 get_joint_pos(chr, j) reads the world matrix of node
 * j of the actor's skeleton (chr+0x50C -> model -> +0x24, 0x190 bytes a
 * node). The host skeletons live in the viewer, which hands their world
 * matrices over each frame (rt_actor_joints); actors without them use
 * their position (stand-in). */
static FLMAT joint_m;
static struct { const void *chr; const f32 *m; int n; } joints[8];

void rt_actor_joints(const void *chr, const float *mats, int n)
{
    int i, f = -1;
    for (i = 0; i < 8; i++) {
        if (joints[i].chr == chr) { f = i; break; }
        if (f < 0 && !joints[i].chr) f = i;
    }
    if (f < 0) return;
    joints[f].chr = chr;
    joints[f].m = mats;
    joints[f].n = n;
}

static const f32 *joint_mat(const void *chr, int j)
{
    int i;
    for (i = 0; i < 8; i++)
        if (joints[i].chr == chr && joints[i].m && j >= 0 && j < joints[i].n)
            return joints[i].m + 16 * j;
    return NULL;
}

void get_joint_pos(void *chr, int joint, f32 *out)
{
    const f32 *m = joint_mat(chr, (s16)joint);
    const f32 *p = m ? m + 12 : (const f32 *)((u8 *)chr + 0xAC);
    out[0] = p[0];
    out[1] = p[1];
    out[2] = p[2];
}

void get_joint_pos_em(void *chr, int joint, f32 *out) { get_joint_pos(chr, joint, out); }

FLMAT *get_joint_wmat(void *chr, int joint)
{
    const f32 *m = joint_mat(chr, (s16)joint);
    const f32 *p = (const f32 *)((u8 *)chr + 0xAC);
    if (m) {
        memcpy(joint_m, m, sizeof joint_m);
        return &joint_m;
    }
    memset(joint_m, 0, sizeof joint_m);
    joint_m[0][0] = joint_m[1][1] = joint_m[2][2] = joint_m[3][3] = 1.0f;
    joint_m[3][0] = p[0];
    joint_m[3][1] = p[1];
    joint_m[3][2] = p[2];
    return &joint_m;
}

void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m);
/* hit_data_expand (0x151A60): one body entry {s16 joint, s16 type, ..,
 * f32 r at +0xC, offsets at +0x10 / +0x1C} around the joint's world
 * matrix (on the PS2 node j of chr+0x50C -> +0x24, 0x190 bytes a node):
 * type 0 sphere, type 1 capsule; radius times the actor scale (+0xB8).
 * Joint 0x7F (and actors without host joints) give -1, no part. */
int hit_data_expand(void *chr, void *body, f32 *cap, f32 *sph)
{
    const u8 *e = body;
    s16 j = *(const s16 *)e;
    const f32 *m;
    f32 o[3];
    if (j == 0x7F || !(m = joint_mat(chr, j)))
        return -1;
    switch (*(const s16 *)(e + 2)) {
    case 0:
        sph[3] = *(const f32 *)(e + 0xC) * *(const f32 *)((u8 *)chr + 0xB8);
        flvecApplyMat33(o, (f32 *)(e + 0x10), (FLMAT *)m);
        sph[0] = m[12] + o[0];
        sph[1] = m[13] + o[1];
        sph[2] = m[14] + o[2];
        return 0;
    case 1:
        cap[6] = *(const f32 *)(e + 0xC) * *(const f32 *)((u8 *)chr + 0xB8);
        flvecApplyMat33(o, (f32 *)(e + 0x10), (FLMAT *)m);
        cap[0] = m[12] + o[0];
        cap[1] = m[13] + o[1];
        cap[2] = m[14] + o[2];
        flvecApplyMat33(o, (f32 *)(e + 0x1C), (FLMAT *)m);
        cap[3] = m[12] + o[0];
        cap[4] = m[13] + o[1];
        cap[5] = m[14] + o[2];
        return 1;
    }
    return -1;
}

FLMAT *get_joint_wmat_em(void *chr, int joint) { return get_joint_wmat(chr, joint); }

/* ------------------------------------------------------------ ground */
/* GetGroundHit, GetGroundShellHit, GetWaterHit: the game's own C now
 * (src/main/hit/shit3_nm.c on the stage's HITS files, rt_hit.c). */

/* ------------------------------------------------------------ models */
typedef struct {                     /* MDLW (get_mdlw_ptr) as the eft code sees it */
    u8 flag;                         /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;                       /* 0x10 material table (unused by the port) */
    u8 _pad14[0x10];
    u8 *skin;                        /* 0x24 skeleton nodes, 0x190 bytes each, world
                                        matrix first; +0xC2 s16 node count (eft05_t) */
    u8 _pad28[0x8];
    CLAY *clay;                      /* 0x30 */
} RT_MDLW;
_Static_assert(offsetof(RT_MDLW, clay) == 0x30, "MDLW layout");
_Static_assert(offsetof(RT_MDLW, skin) == 0x24, "MDLW layout");

extern RT_MDLW *eft_mdlw[5];         /* 0x3C8DC0, data table work area */
static RT_MDLW eft_mdl[5];

/* eft_mdlw[k] (load_eft / load_shadow, 0x111110): k 0 ef_00, 1-3 kage04-06,
 * 4 ef_01 */
static void (*eft_skin_cb)(int k, const float *mats, int n);

/* skinned effect models (ef_01: eft05 slash trails): the game writes the
 * node matrices into mdlw->skin and calls flSetSkinTrans(skin); the host
 * then re-skins the model's clays with them (callback from the viewer)
 * before the flExecuteClay that follows. */
void rt_bind_eft_skin(int k, int nbone, void (*cb)(int k, const float *mats, int n))
{
    if (k < 0 || k >= 5 || nbone <= 0)
        return;
    eft_mdl[k].skin = calloc((size_t)nbone, 0x190);
    *(s16 *)(eft_mdl[k].skin + 0xC2) = (s16)nbone;
    eft_skin_cb = cb;
}

void flSetSkinTrans(void *skin)
{
    int k, i, n;
    static f32 mats[64][16];
    for (k = 0; k < 5; k++)
        if (eft_mdl[k].skin && eft_mdl[k].skin == skin)
            break;
    if (k == 5 || !eft_skin_cb)
        return;             /* player/weapon hierarchies: drawn by the host */
    n = *(s16 *)(eft_mdl[k].skin + 0xC2);
    if (n > 64) n = 64;
    for (i = 0; i < n; i++)
        memcpy(mats[i], eft_mdl[k].skin + 0x190 * i, 64);
    eft_skin_cb(k, &mats[0][0], n);
}

void rt_bind_eft_model(int k, gfx_clay *const *c, const uint32_t *attr, int n)
{
    CLAY *cl;
    int i;
    if (k < 0 || k >= 5)
        return;
    cl = calloc((size_t)(n > 0 ? n : 1), sizeof *cl);
    for (i = 0; i < n; i++) {
        cl[i].handle = rt_register_clay(c[i]);
        cl[i].attr = attr ? (s32)attr[i] : 0;
    }
    eft_mdl[k].flag = 1;
    eft_mdl[k].clay = cl;
    eft_mdlw[k] = &eft_mdl[k];
}

/* ------------------------------------------------------------ loop */
void rt_eft_init(void)
{
    eft_init();
    shl_init();
    memset(prim2, 0, sizeof prim2);
    prim2_top = 0;
    memset(senko_stack, 0, sizeof senko_stack);
    memset(smoke_stack, 0, sizeof smoke_stack);
    memset(smell_stack, 0, sizeof smell_stack);
    senko_cnt = smoke_cnt = smell_cnt = 0;
}

void rt_eft_move(void)
{
    move_shell();
    move_eft();
}

void rt_eft_draw(void)
{
    trans_shell();
    trans_eft(0);
    trans_eft(1);
}

/* RT_TRACE: list live effects and shells once */
void rt_eft_trace(void)
{
    u8 *p;
    for (p = eft_w_top; p; p = LNK_NEXT(p))
        fprintf(stderr, "rt: eft type %d arg %d mode %d\n", p[2], p[3], p[4]);
    for (p = shell_w_top; p; p = LNK_NEXT(p))
        fprintf(stderr, "rt: shell type %d arg %d mode %d\n", p[2], p[3], p[4]);
}

/* ------------------------------------------------------------ stand-ins
 * Main-program and monster/player code the effects call. Small ones are
 * ported from the asm; the rest need players, monsters, sound, pad
 * vibration or skinned-model drawing, which the port does not run yet:
 * they do nothing (marked "stub"). */
static void once(const char *name)
{
    if (getenv("RT_TRACE"))
        fprintf(stderr, "rt: %s is a stub\n", name);
}
#define STUB_V(name, args) void name args { static int o; if (!o++) once(#name); }
#define STUB_I(name, args) int name args { static int o; if (!o++) once(#name); return 0; }

/* act_ck (0x14EF20): the object's action pair (+0x14, +0x15) is (a, b) */
int act_ck(void *chr, int a, int b)
{
    const u8 *p = chr;
    return p[0x14] == (u8)a && p[0x15] == (u8)b;
}

/* pl_flag_ck (0x14EF60): bit test in the flag words +0x390 / +0x394 (bit
 * 31 of the argument picks the second) */
int pl_flag_ck(void *pl, u32 flag)
{
    const u8 *p = pl;
    u32 w;
    if (flag & 0x80000000u) {
        memcpy(&w, p + 0x394, 4);
        return (int)(w & (flag & 0x7FFFFFFFu));
    }
    memcpy(&w, p + 0x390, 4);
    return (int)(w & flag);
}

/* Pl_silencer_ck (0x154D30): gun type 7 with option bit 0x10 */
int Pl_silencer_ck(void *pl)
{
    const u8 *p = pl;
    u16 o;
    if (p[0x35F] != 7)
        return 0;
    memcpy(&o, p + 0x362, 2);
    return (o & 0x10) != 0;
}

/* Em_area_ck (0x10B790): index (0-3) of area a in game_w+0x28, else -1 */
int Em_area_ck(int a)
{
    const u8 *g = (const u8 *)&game_w;
    int i;
    for (i = 0; i < 4; i++)
        if (g[0x28 + i] == (u8)(s16)a)
            return i;
    return -1;
}

/* Em_Calc_angY: game.bin em_core (src/game/em/em_core_nm.c, built). */

/* frame_check* come from the decompiled src/main/frame/f_frame_nm.c */

/* atck_data_set_shl / pl_atck_data_set_shl: src/pc/rt/rt_pl.c */

/* shell_flag_set: src/main/pl/pl_normal.c (built). shell_rate_add/_g
 * (0x151660, 0x1516A0): velocity integration */

void shell_rate_add(SHLW *sh)
{
    sh->pos2.x += sh->rate[0];
    sh->pos2.y += sh->rate[1];
    sh->pos2.z += sh->rate[2];
}

void shell_rate_add_g(SHLW *sh)
{
    sh->rate[0] += sh->rate_g[0];
    sh->rate[1] += sh->rate_g[1];
    sh->rate[2] += sh->rate_g[2];
    shell_rate_add(sh);
}

int softdip_ck(void) { return 0; }   /* 0x1593D0: returns 0 */

STUB_V(vib_set_pl, (void *pl, int a))
STUB_V(pl_light_change, (void *em, int a))
STUB_V(Pl_light_set, (void *em))
STUB_I(Get_atk_value, (void *pl, int a))
STUB_I(em09_status_ck, (void *em))
STUB_V(em09_dir_calc, (s32 *a, s32 *b, s32 c))
STUB_V(em_material_sub, (void *em, int a, CLAY *c))
STUB_I(Em_tail_hagi_point_set, (EFTW *ew, int a))
STUB_V(Ext_pick_point_clr, (void))
STUB_I(Ext_pick_point_cnt_ck, (int a))
STUB_V(Ext_pick_point_pos, (int a, f32 *pos))
/* skinned-model drawing (fl hierarchy), used by eft01/eft05/eft09 */
STUB_V(flCalcTrans, (void *h, FLMAT *m))
STUB_V(flCalcTransSI, (void *h, FLMAT *m))
STUB_V(flSetMatrixList, (void *a, void *b))
STUB_V(flSetSkinTransMatrixList, (void *a, void *b))
/* shell08_trans: src/game/shell/shell08_nm.c (near-match C, built). */

/* ------------------------------------------------------------ test spawns
 * RT_SPAWN="eft13:N,eft17:N,shell22:N,eft14:N,eft08:N" spawns those effects
 * (N = arg) at pos, for checking the eft/shell C with screenshots. */
void Eft13_set_pos(f32 scale, f32 *pos, int arg);
void Eft17_set_ex(f32 *pos, int ang, int arg, f32 scale);
void Shell22_set2(f32 *pos, u8 arg, int stg, u16 ang);
void Eft14_set2(f32 *pos, s16 arg);
void Eft08_set(f32 *pos, int arg, int x07, f32 scale);

void rt_debug_spawn(const float pos[3])
{
    const char *s = getenv("RT_SPAWN");
    char name[16];
    int arg, n;
    f32 p[3];
    while (s && sscanf(s, "%15[a-z0-9]:%d%n", name, &arg, &n) == 2) {
        p[0] = pos[0];
        p[1] = pos[1];
        p[2] = pos[2];
        if (!strcmp(name, "eft13"))
            Eft13_set_pos(1.0f, p, arg);
        else if (!strcmp(name, "eft17"))
            Eft17_set_ex(p, 0, arg, 1.0f);
        else if (!strcmp(name, "shell22"))
            Shell22_set2(p, (u8)arg, game_w.stage, 0);
        else if (!strcmp(name, "eft14"))
            Eft14_set2(p, (s16)arg);
        else if (!strcmp(name, "eft08"))
            Eft08_set(p, arg, 0, 1.0f);
        else
            fprintf(stderr, "rt: RT_SPAWN: unknown %s\n", name);
        s += n;
        if (*s == ',')
            s++;
    }
}
