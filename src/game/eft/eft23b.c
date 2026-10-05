/* eft23 - game.bin 0x00557480-0x005589EC, part 2 (after fish_type_set). A fish in a fishing spot. It
 * appears (toujyou), wanders inside its range looking for a float
 * (eft22, found by uki_serch), nibbles and bites (atari), and once a
 * player hooks it (turare) it follows the float until landed. The fish
 * kind comes from weighted per-spot tables (fish_type_set). */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

/* One material table entry (0x4C bytes). */
typedef struct MATERIAL {
    u8 _pad00[4];
    f32 col[3];         /* 0x04 */
    u8 _pad10[0x4C - 0x10];
} MATERIAL;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    MATERIAL *mat;      /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* The fish (ew->work). */
typedef struct FISH {
    f32 home[3];        /* 0x00 centre of its range */
    f32 scale[3];       /* 0x0C */
    f32 range;          /* 0x18 */
    f32 dist;           /* 0x1C to the float */
    f32 accel;          /* 0x20 */
    s16 bite_time;      /* 0x24 */
    s16 nibbles;        /* 0x26 */
    EFTW *uki;          /* 0x28 the float it is after */
    s32 tgt_ang;        /* 0x2C */
    s32 turn;           /* 0x30 */
    s32 sway;           /* 0x34 */
    s32 rot[3];         /* 0x38 */
    s8 type;            /* 0x44 index into fish_type_data */
    s8 hooked;          /* 0x45 */
} FISH;

typedef struct FISH_DATA {
    u16 catch_time;     /* 0x00 */
    u8 _pad02[2];
    u16 mdl;            /* 0x04 */
    u16 col;            /* 0x06 */
    f32 scale[3];       /* 0x08 */
} FISH_DATA;

typedef struct FISH_CHANCE {
    u16 weight;         /* 0xFFFF ends the list */
    s8 type;
    u8 _pad03;
} FISH_CHANCE;

typedef struct QUEST_W {
    u8 _pad00[0x14E];
    s8 x14E;            /* 0x14E selects the alternate fish tables */
} QUEST_W;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern EFTW eft_work[128];
extern QUEST_W quest_w;
extern FISH_DATA fish_type_data[];
extern FISH_CHANCE *fish_type_tbl[26];
extern u16 kuituki_time_tbl[4];
extern u8 Eft_fish_rgb[][3];
extern u8 Eft_hire_rgb[][3];

u32 ran_suu(int);
int Pl_master_ck(PLW *);
int act_ck(void *, int, int);
void release_prim(s16);
f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, s32);
void cpRotMatrix(s32 *, FLMAT *);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecNormalize(f32 *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flmatRotZXY33(FLMAT *, f32, f32, f32);
void vib_set_pl(void *, int);
f32 Eft22_suimen_ck(EFTW *);
void Eft20_set2(f32, f32 *, int, int);

void eft23_move(EFTW *ew);

void Eft23_set(int arg, f32 *pos) {
    EFTW *ew = pull_eft_work(1);
    FISH *w;

    if (ew != 0) {
        w = ew->work;
        ew->type = 23;
        ew->move = eft23_move;
        ew->work14 = 0;
        ew->arg = arg;
        w->home[0] = ew->pos[0] = *pos++;
        w->home[1] = ew->pos[1] = *pos++;
        w->home[2] = ew->pos[2] = *pos++;
        w->range = *pos;
    }
}

/* Fish spot: centre, range, fish type (negative ends the list) and how
 * many fish of that type to spawn. */
typedef struct FISH_SPOT {
    f32 pos[3];         /* 0x00 */
    f32 range;          /* 0x0C */
    s32 kind;           /* 0x10 */
    s32 num;            /* 0x14 */
} FISH_SPOT;

extern FISH_SPOT *Fish_hani_tbl[];

/* Spawns every fish of a stage's spot table (stage argument). */
void Fish_set(int stage) {
    FISH_SPOT *sp = Fish_hani_tbl[stage];
    int n;

    if (sp != 0) {
        for (;;) {
            if (sp->kind < 0) {
                break;
            }
            n = sp->num;
            while (n > 0) {
                Eft23_set(sp->kind, (f32 *)sp);
                n--;
            }
            sp++;
        }
    }
}
