/* Monster update. SLPM_654.95 0x0010BA00-0x0010CD30 (f_em): em_init (state 0), em_die (2),
 * em_erase (3) and enemy_mv, the per frame driver that picks one by w->x04 and queues the draw
 * primitive. EMX has only the fields used here (EMW in em.h has the rest). */
#include "types.h"

typedef struct PRIM_H { u8 _pad00[8]; f32 pos[3]; } PRIM_H;

typedef struct EMX {
    u8 be_flag;                 /* 0x000 */
    u8 x01;                     /* 0x001 */
    u8 kind;                    /* 0x002 monster kind */
    u8 _pad003;
    u8 state;                   /* 0x004 0 init, 1 move, 2 die, 3 erase */
    u8 _pad005[0x10 - 5];
    u8 x10;                     /* 0x010 */
    u8 _pad011[0x1E - 0x11];
    u8 x1E;                     /* 0x01E */
    u8 _pad01F[0x20 - 0x1F];
    f32 mat[16];                /* 0x020 */
    u8 _pad060[0xA0 - 0x60];
    s32 ang[3];                 /* 0x0A0 */
    f32 pos[3];                 /* 0x0AC */
    u8 _pad0B8[0x1A0 - 0xB8];
    f32 spd0;                   /* 0x1A0 */
    u8 _pad1A4[0x1F0 - 0x1A4];
    f32 spd1;                   /* 0x1F0 */
    u8 _pad1F4[0x240 - 0x1F4];
    f32 spd2;                   /* 0x240 */
    u8 _pad244[0x290 - 0x244];
    f32 spd3;                   /* 0x290 */
    u8 _pad294[0x2DC - 0x294];
    u16 x2DC, x2DE, x2E0, x2E2; /* 0x2DC cleared by em_init */
    u8 _pad2E4[0x300 - 0x2E4];
    u16 parts;                  /* 0x300 */
    u8 _pad302[0x388 - 0x302];
    u8 x388;                    /* 0x388 */
    u8 _pad389[0x3A8 - 0x389];
    s32 x3A8;                   /* 0x3A8 */
    u8 _pad3AC[0x4E6 - 0x3AC];
    u8 hagi[0x20];              /* 0x4E6 set to 1 by em_init */
    u8 _pad506[0x564 - 0x506];
    PRIM_H *prim;               /* 0x564 */
    u8 _pad568[0x5A0 - 0x568];
    f32 old_pos[3];             /* 0x5A0 */
    f32 floor;                  /* 0x5AC */
    u8 _pad5B0[0x70C - 0x5B0];
    u8 x70C[0x2A];              /* 0x70C ground info */
    u8 stg;                     /* 0x736 */
    u8 _pad737[0x798 - 0x737];
    f32 draw_mode;              /* 0x798 */
    u8 _pad79C[0x7E4 - 0x79C];
    u8 x7E4[0xA0];              /* 0x7E4 */
    s8 x884, x885;              /* 0x884 */
    u8 _pad886[0x88E - 0x886];
    u8 x88E;                    /* 0x88E master player */
    u8 _pad88F[0x930 - 0x88F];
    f32 act_spd;                /* 0x930 */
} EMX;

typedef struct PLX { u8 _pad00[0xAC]; f32 pos[3]; } PLX;
typedef struct GWX { u8 _pad00[0x14]; u8 stage; u8 _pad15[0xD1 - 0x15]; u8 master; } GWX;

extern GWX game_w;
extern PLX player_work;
extern u8 ot0[], ot1[];

void em_work_set(EMX *);
void em_status_init(EMX *);
void func_53A190(EMX *);
void func_5341D0(EMX *, int);
void func_536BC0(EMX *, int);
void func_539C90(EMX *);
void func_539C40(EMX *);
void func_53A2B0(EMX *);
int Em_max_parts_get(int);
void em_init_sub(EMX *);
void em_dur_init(EMX *);
void World_calc(EMX *);
void pl_timer_calc(EMX *);
void hit_stop_calc(EMX *);
void cpRotMatrixYXZ2(s32 *, f32 *);
void func_534680(EMX *);
void func_5363C0(EMX *);
void func_536530(EMX *);
void frame_move(EMX *);
void HitWallPlayer(EMX *, int);
void GetGroundHitStatusAreaEm(EMX *, f32 *, u8 *, f32 *, u8 *);
void GetEmMaterialData(EMX *);
void Quest_enemy_die(EMX *);
void Em_hagi_point_clr(EMX *);
void push_em_work(EMX *);
void em_move(EMX *);
void em_die(EMX *);
void em_erase(EMX *);
void em_init(EMX *);
void em_effect_move(EMX *);
f32 flArcTan2(f32, f32);
int flConvertRtoS(f32);
void add_prim(void *, PRIM_H *, int, int);

void em_init(EMX *w) {
    int i;

    em_work_set(w);
    w->state++;
    w->x10 = 1;
    w->x1E = 0;
    w->x88E = game_w.master;
    em_status_init(w);
    w->x2DC = 0;
    w->x2DE = 0;
    w->x2E0 = 0;
    w->x2E2 = 0;
    func_53A190(w);
    func_5341D0(w, 0);
    func_536BC0(w, 0);
    func_539C90(w);
    func_539C40(w);
    func_53A2B0(w);
    i = 0;
    do {
        w->hagi[i + 0] = 1;
        w->hagi[i + 1] = 1;
        w->hagi[i + 2] = 1;
        w->hagi[i + 3] = 1;
        w->hagi[i + 4] = 1;
        w->hagi[i + 5] = 1;
        w->hagi[i + 6] = 1;
        w->hagi[i + 7] = 1;
        i += 8;
    } while (i < 0x20);
    w->parts = (u8)Em_max_parts_get(w->kind);
    em_init_sub(w);
    em_dur_init(w);
    World_calc(w);
}

void em_die(EMX *w) {
    pl_timer_calc(w);
    hit_stop_calc(w);
    w->old_pos[0] = w->pos[0];
    w->old_pos[1] = w->pos[1];
    w->old_pos[2] = w->pos[2];
    if (w->x884 != 0) {
        w->x884--;
    }
    if (w->x885 != 0) {
        w->x885--;
    }
    w->state++;
    w->x01 = 0;
    w->ang[0] = (u16)w->ang[0];
    w->ang[1] = (u16)w->ang[1];
    w->ang[2] = (u16)w->ang[2];
    cpRotMatrixYXZ2(w->ang, w->mat);
    func_534680(w);
    func_5363C0(w);
    func_536530(w);
    w->spd0 = 2.0f * w->act_spd;
    w->spd1 = 2.0f * w->act_spd;
    w->spd2 = 2.0f * w->act_spd;
    w->spd3 = 2.0f * w->act_spd;
    frame_move(w);
    HitWallPlayer(w, 0);
    GetGroundHitStatusAreaEm(w, w->pos, w->x70C, &w->floor, w->x7E4);
    if (w->x388 != 2 && w->x388 != 4) {
        w->pos[1] = w->floor;
    }
    GetEmMaterialData(w);
}

void em_erase(EMX *w) {
    Quest_enemy_die(w);
    Em_hagi_point_clr(w);
    push_em_work(w);
}

int enemy_mv(EMX *w) {
    switch (w->state) {
    case 0:
        em_init(w);
        em_effect_move(w);
        World_calc(w);
        return 0;
    case 1:
        em_move(w);
        break;
    case 2:
        em_die(w);
        break;
    case 3:
        em_erase(w);
        return 1;
    }
    em_effect_move(w);
    if (w->x01 != 0 && w->stg == game_w.stage) {
        w->x3A8 = ((u16)flConvertRtoS(flArcTan2(-(player_work.pos[2] - w->pos[2]), player_work.pos[0] - w->pos[0])) + 0x4000) - w->ang[1];
        w->prim->pos[0] = w->pos[0];
        w->prim->pos[1] = w->pos[1];
        w->prim->pos[2] = w->pos[2];
        if (w->prim != 0) {
            if (w->draw_mode == 1.0f) {
                if (w->kind == 2 || w->kind == 7) {
                    add_prim(ot1, w->prim, 0x20, 1);
                } else {
                    add_prim(ot1, w->prim, 0x20, 0);
                }
            } else {
                if (w->kind == 2 || w->kind == 7) {
                    add_prim(ot0, w->prim, 0x40, 1);
                } else {
                    add_prim(ot0, w->prim, 0x40, 0);
                }
            }
        }
    }
    World_calc(w);
    return 0;
}
