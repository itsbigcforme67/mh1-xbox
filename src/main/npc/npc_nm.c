/* NPC (the hunter-guild "jijii" and cat-like helper models in the plaza),
 * SLPM_654.95 main 0x23D870-0x23E500. An NPC is a player-like work block
 * (PLW layout: 0x0 flag, 0x1 active, 0x2 kind (0 = idol, 1 = jijii), 0x4 step,
 * 0xA0 angle, 0xAC position, 0x4E6 per-part visible flags, 0x50C model).
 * npc_mv is the per-frame task function; npc_trans draws the model. Raw byte
 * offsets are used for the fields that PLW does not name yet. Meanings are
 * guesses. */
#include "types.h"
#include "fl.h"
#include "clay.h"

#define B8(p, o)   (*(u8 *)((u8 *)(p) + (o)))
#define BS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define B16(p, o)  (*(u16 *)((u8 *)(p) + (o)))
#define BS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B32(p, o)  (*(s32 *)((u8 *)(p) + (o)))
#define BF(p, o)   (*(f32 *)((u8 *)(p) + (o)))
#define BP(p, o)   (*(void **)((u8 *)(p) + (o)))

typedef struct NPCW {
    u8 on0; /* 0x0 */
    u8 on; /* 0x1 */
    u8 kind; /* 0x2 */
    u8 _pad003[0x1];
    u8 step; /* 0x4 */
    u8 sub; /* 0x5 */
    u8 x06; /* 0x6 */
    u8 _pad007[0x1];
    s32 timer; /* 0x8 */
    u8 _pad00C[0x4];
    s8 x10; /* 0x10 */
    u8 _pad011[0x3];
    u8 x14; /* 0x14 */
    u8 x15; /* 0x15 */
    u8 _pad016[0x5];
    u8 var; /* 0x1B */
    u8 _pad01C[0x2];
    s8 x1E; /* 0x1E */
    u8 _pad01F[0x81];
    s32 ang0; /* 0xA0 */
    s32 ang1; /* 0xA4 */
    s32 ang2; /* 0xA8 */
    f32 posx; /* 0xAC */
    f32 posy; /* 0xB0 */
    f32 posz; /* 0xB4 */
    f32 sclx; /* 0xB8 */
    f32 scly; /* 0xBC */
    f32 sclz; /* 0xC0 */
    u8 _pad0C4[0xDC];
    f32 f1A0; /* 0x1A0 */
    u8 _pad1A4[0x4C];
    f32 f1F0; /* 0x1F0 */
    u8 _pad1F4[0x4C];
    f32 f240; /* 0x240 */
    u8 _pad244[0x4C];
    f32 f290; /* 0x290 */
    u8 _pad294[0x48];
    s16 char0; /* 0x2DC */
    s16 c2DE; /* 0x2DE */
    s16 c2E0; /* 0x2E0 */
    s16 c2E2; /* 0x2E2 */
    u16 act0; /* 0x2E4 */
    u16 act1; /* 0x2E6 */
    u8 _pad2E8[0x4];
    s16 bl0; /* 0x2EC */
    s16 bl1; /* 0x2EE */
    u8 _pad2F0[0xC];
    u8 f2FC; /* 0x2FC */
    u8 f2FD; /* 0x2FD */
    u8 _pad2FE[0x2];
    u16 w300; /* 0x300 */
    u8 _pad302[0x9A];
    s32 w39C; /* 0x39C */
    u8 _pad3A0[0x2C];
    u8 *prog; /* 0x3CC */
    u8 _pad3D0[0x3A];
    u8 x40A; /* 0x40A */
    u8 _pad40B[0x3];
    s16 x40E; /* 0x40E */
    u8 _pad410[0x2];
    s8 x412; /* 0x412 */
    u8 _pad413[0xC1];
    s8 x4D4; /* 0x4D4 */
    u8 _pad4D5[0x11];
    s8 parts[0x20]; /* 0x4E6 */
    u8 _pad506[0x6];
    u8 *mdl; /* 0x50C */
    u8 _pad510[0x54];
    u8 *prim; /* 0x564 */
    s16 primno; /* 0x568 */
    u8 _pad56A[0x36];
    f32 ox; /* 0x5A0 */
    f32 oy; /* 0x5A4 */
    f32 oz; /* 0x5A8 */
    f32 gy; /* 0x5AC */
    u8 _pad5B0[0x186];
    u8 stg; /* 0x736 */
    u8 _pad737[0x61];
    f32 f798; /* 0x798 */
    u8 _pad79C[0x3A];
    s8 x7D6; /* 0x7D6 */
    u8 _pad7D7[0x159];
    f32 f930; /* 0x930 */
    u8 _pad_end[0x940 - 0x934];
} NPCW;

extern u16 Psw[];
extern u8 game_w[];
extern f32 stage_start_pos[][3];
extern u16 *npc_jijii_tbl[];
extern s16 npc_disp_parts_003494B0[][2];
extern u8 dummy_adr_tbl_003494F0[];
extern u8 ot1[];

void pl_light_change();
void Pl_light_set();
void cpAng2Rad_all();
void cpRotMatrixYXZ2();
void cpRotMatrix();
void flmatCopy();
void SetFilterMode();
void flSetSkinTrans();
void reload_tex();
void flCalcTransSI();
s16 get_prim();
void *get_prim_ptr();
void add_prim();
void em_work_set();
void func_53A190();
void frame_init();
void frame_move();
void World_calc();
void pl_chr_set();
void pl_chr_set2();
void pl_timer_calc();
void hit_stop_calc();
void act_set();
int ran_suu();
void HitWallPlayer();
void GetGroundHitStatusAreaEm();
int softdip_ck();
void push_em_work();
void npc_trans();
void npc_init_sub();
void npc_init();
void npc_move();
void npc_die();
void npc_erase();
void npc_chr_sub();
void npc_effect_move();

void npc_trans(arg)
u8 *arg;
{
    f32 rad[3];
    FLMAT m3;
    FLMAT m4;
    FLMAT m2;
    NPCW *pl;
    u8 *mdl;
    u8 *tbl;
    u8 *clay;
    int cnt;
    int i;
    int j;

    pl = BP(arg, 0x18);
    mdl = pl->mdl;
    if (pl->on0 == 0 || pl->on == 0) {
        return;
    }
    pl_light_change(pl, 1);
    Pl_light_set(pl);
    cpAng2Rad_all((u8 *)pl + 0xA0, rad);
    flmatMakeScale(&m2, pl->sclx, pl->scly, pl->sclz);
    cpRotMatrixYXZ2((u8 *)pl + 0xA0, m3);
    flmatSetTrans(&m3, pl->posx, pl->posy, pl->posz);
    flmatMul33_2(&m3, &m2);
    flmatCopy((u8 *)pl + 0x60, m3);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    clay = BP(mdl, 0x30);
    cnt = BS16(mdl, 0x2C);
    tbl = BP(mdl, 0x10);
    flSetSkinTrans(B32(mdl, 0x24));
    flmatInit(&m4);
    flSetRenderState(0x19, (u32)&m4);
    reload_tex(0xA, pl->kind * 0x14 + 0x9A);
    for (i = 0; i < cnt; i++) {
        if (pl->kind != 0 || pl->parts[i] != 0) {
            flSetRenderState(0x67, -1);
            if (B32(clay, 0) != -1) {
                u8 *e = clay;

                for (j = 0; j < B32(clay, 4); j++) {
                    u8 *t = tbl + B32(e, 8) * 0x4C;

                    if (pl->kind == 1) {
                        if (pl->parts[j] == 0) {
                            B32(t, 0x10) = 0;
                        } else {
                            BF(t, 0x10) = pl->f798;
                        }
                    } else {
                        BF(t, 0x10) = pl->f798;
                    }
                    flSetRenderState((j + 0x3A) & 0xFF, (u32)t);
                    e += 4;
                }
                clay_attr_set(B32(clay, 0x88));
                flExecuteClay(B32(clay, 0), 0);
            }
        }
        clay += 0x8C;
    }
    clay_attr_reset();
    flSetRenderState(0x60, 0);
}

void npc_init_sub(p)
NPCW *p;
{
    p->prog = dummy_adr_tbl_003494F0;
    (*(void (**)())p->prog)();
    if (p->on != 0) {
        p->primno = get_prim();
        if (p->primno != -1) {
            p->prim = get_prim_ptr(p->primno);
            BP(p->prim, 0x18) = p;
            BP(p->prim, 0x14) = npc_trans;
        }
    }
}

void npc_init(p)
NPCW *p;
{
    int i;

    em_work_set();
    p->step++;
    p->x10 = 0;
    p->x1E = 1;
    p->x412 = 0;
    p->on = 1;
    p->f1A0 = 2.0f;
    p->f1F0 = 2.0f;
    p->f240 = 2.0f;
    p->f290 = 2.0f;
    p->f930 = 1.0f;
    if (p->kind == 0) {
        p->w300 = 2;
    } else {
        p->x10 = 1;
        p->w300 = 1;
    }
    p->sclx = 1.0f;
    p->scly = 1.0f;
    p->sclz = 1.0f;
    p->ang0 = 0;
    p->ang1 = 0;
    p->ang2 = 0;
    p->w39C = 0;
    p->stg = B8(game_w, 0x14);
    p->posx = stage_start_pos[p->stg][0];
    p->posy = stage_start_pos[p->stg][1];
    p->posz = stage_start_pos[p->stg][2];
    if (p->kind == 1) {
        p->posx = p->posx + 200.0f * (f32)p->var;
        p->posz = p->posz + 200.0f * (f32)p->var;
    }
    p->f798 = 1.0f;
    p->x7D6 = 0;
    p->x4D4 = 1;
    p->char0 = 0;
    p->c2DE = 0;
    p->c2E0 = 0;
    p->c2E2 = 0;
    func_53A190(p);
    i = 0;
    do {
        p->parts[i] = 0;
        p->parts[i + 1] = 0;
        p->parts[i + 2] = 0;
        p->parts[i + 3] = 0;
        p->parts[i + 4] = 0;
        p->parts[i + 5] = 0;
        p->parts[i + 6] = 0;
        p->parts[i + 7] = 0;
        i += 8;
    } while (i < 0x20);
    switch (p->kind) {
    case 0:
        p->parts[npc_disp_parts_003494B0[p->var][0]] = 1;
        if (p->var * 2 + 1 != 0xFF) {
            p->parts[npc_disp_parts_003494B0[p->var][1]] = 1;
        }
        break;
    case 1: {
        u16 *q = npc_jijii_tbl[p->var];
        u16 v = *q;

        if (v != 0xFFFF) {
            do {
                q++;
                p->parts[v] = 1;
                v = *q;
            } while (v != 0xFFFF);
        }
        break;
    }
    }
    npc_init_sub(p);
    if (p->kind == 0) {
        pl_chr_set2(p, 1, 0, 0);
        frame_init(p, p->act0, p->bl0, 0);
        frame_init(p, p->act1, p->bl1, 1);
    } else {
        pl_chr_set(p, 0x3E9, 0, 0, 0);
        frame_init(p, p->act0, p->bl0, 0);
    }
    World_calc(p);
}

void npc_move(p)
NPCW *p;
{
    int i;
    u16 *q;
    u16 v;

    p->x40E = 0xA;
    pl_timer_calc();
    hit_stop_calc(p);
    p->ox = p->posx;
    p->oy = p->posy;
    p->oz = p->posz;
    if (p->x14 != 0) {
        p->x14 = 0;
        p->x15 = 0;
        p->sub = 0;
    } else {
        switch (p->x15) {
        case 0:
            if (p->kind == 0) {
                if (p->sub == 0) {
                    p->sub++;
                    p->timer = ((ran_suu(1) & 0xFFFF) + 0x3C) & 0xFF;
                    if (p->char0 != 1) {
                        pl_chr_set2(p, 1, 4, 0);
                    }
                } else {
                    p->timer = p->timer - 1;
                    if (p->timer <= 0) {
                        act_set(p, 0, 1);
                    }
                }
            } else {
                p->timer = p->timer + 1;
                if (Psw[2] & 0x200) {
                    p->var++;
                    p->var = p->var % 6;
                    i = 0;
                    do {
                        p->parts[i & 0xFFFF] = 0;
                        p->parts[(i + 1) & 0xFFFF] = 0;
                        p->parts[(i + 2) & 0xFFFF] = 0;
                        p->parts[(i + 3) & 0xFFFF] = 0;
                        p->parts[(i + 4) & 0xFFFF] = 0;
                        p->parts[(i + 5) & 0xFFFF] = 0;
                        p->parts[(i + 6) & 0xFFFF] = 0;
                        p->parts[(i + 7) & 0xFFFF] = 0;
                        i = (i + 8) & 0xFFFF;
                    } while (i < 0x20);
                    q = npc_jijii_tbl[p->var];
                    v = *q;
                    if (v != 0xFFFF) {
                        do {
                            q++;
                            p->parts[v & 0xFFFF] = 1;
                            v = *q;
                        } while (v != 0xFFFF);
                    }
                }
            }
            break;
        case 1:
            if (p->sub == 0) {
                p->sub++;
                p->x06 = ran_suu(1) & 0xFFFF & 1;
                p->timer = ((ran_suu(1) & 0xFFFF) + 0x3C) & 0xFF;
                pl_chr_set2(p, 3, 4, 0);
            } else {
                p->timer = p->timer - 1;
                if (p->timer <= 0) {
                    act_set(p, 0, 0);
                } else if (p->x06 == 0) {
                    p->ang1 = p->ang1 + (ran_suu(1) & 0xFFFF & 0x1FF);
                } else {
                    p->ang1 = p->ang1 - (ran_suu(1) & 0xFFFF & 0x1FF);
                }
            }
            break;
        }
    }
    p->ang0 = p->ang0;
    p->ang1 = (u16)p->ang1;
    p->ang2 = p->ang2;
    cpRotMatrixYXZ2((u8 *)p + 0xA0, (u8 *)p + 0x20);
    p->f1A0 = 2.0f * p->f930;
    p->f1F0 = 2.0f * p->f930;
    p->f240 = 2.0f * p->f930;
    p->f290 = 2.0f * p->f930;
    npc_chr_sub(p);
    HitWallPlayer(p, 0);
    GetGroundHitStatusAreaEm(p, (u8 *)p + 0xAC, (u8 *)p + 0x70C, (u8 *)p + 0x5AC);
    p->posy = p->gy;
}

void npc_die(p)
NPCW *p;
{
    p->step++;
}

void npc_erase(void)
{
    push_em_work();
}

int npc_mv(p)
NPCW *p;
{
    switch (p->step) {
    case 0:
        npc_init();
        npc_effect_move(p);
        World_calc(p);
        return 0;
    case 1:
        npc_move();
    default:
        break;
    case 2:
        npc_die();
        break;
    case 3:
        npc_erase();
        return 1;
    }
    npc_effect_move(p);
    if (p->on != 0 && p->stg == B8(game_w, 0x14)) {
        BF(p->prim, 8) = p->posx;
        BF(p->prim, 0xC) = p->posy;
        BF(p->prim, 0x10) = p->posz;
        add_prim(ot1, p->prim, 0x20, 0);
    }
    World_calc(p);
    return 0;
}

void npc_chr_sub(p)
NPCW *p;
{
    if (softdip_ck(0x28) == 0) {
        cpRotMatrix((u8 *)p + 0xA0, (u8 *)p + 0x20);
        if (p->x40A == 0) {
            if (p->f2FC == 0) {
                p->f2FC = p->f2FC + 1;
                frame_init(p, p->act0, p->bl0, 0);
            }
            if (p->f2FD == 0 && p->w300 > 1) {
                p->f2FD = p->f2FD + 1;
                frame_init(p, p->act1, p->bl1, 1);
            }
            frame_move(p);
        }
    }
}

void npc_mk(p)
NPCW *p;
{
    FLMAT b;
    FLMAT a;
    u8 *mdl = p->mdl;

    flmatMakeScale(&a, p->sclx, p->scly, p->sclz);
    cpRotMatrixYXZ2((u8 *)p + 0xA0, &b);
    flmatSetTrans(&b, p->posx, p->posy, p->posz);
    flmatMul33_2(&b, &a);
    flmatCopy((u8 *)p + 0x60, &b);
    flCalcTransSI(B32(mdl, 0x24), &b);
}

void npc_effect_move(p)
NPCW *p;
{
    (*(void (**)())((u8 *)p->prog + 0xC))(p);
}
