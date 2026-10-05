/* emu01 - monster update 0x0010B7F0-0x0010BAD0: em_init_sub. Whole file in emupd_nm.c. */
#include "types.h"

typedef struct PRIM_H { u8 _pad00[8]; f32 pos[3]; void (*trans)(struct PRIM_H *); struct EMX *owner; } PRIM_H;

struct EMX;
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
    u8 _pad3AC[0x3CC - 0x3AC];
    void (**prog)(struct EMX *); /* 0x3CC per-monster program table */
    u8 _pad3D0[0x4E6 - 0x3D0];
    u8 hagi[0x20];              /* 0x4E6 set to 1 by em_init */
    u8 _pad506[0x564 - 0x506];
    PRIM_H *prim;               /* 0x564 */
    s16 prim_no;                /* 0x568 */
    u8 _pad56A[0x5A0 - 0x56A];
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





extern void (**em_prog_tbl[])(EMX *);
extern u8 enemy_trans[];
s16 get_prim(void);
PRIM_H *get_prim_ptr(int);
void func_566670(EMX *);
void func_57EFA0(EMX *);
void func_5873D0(EMX *);
void func_58B850(EMX *);
void func_58F8B0(EMX *);
void func_59A2C0(EMX *);
void func_5A7F70(EMX *);
void func_5AD5A0(EMX *);
void func_5AF530(EMX *);
void func_5B52D0(EMX *);
void func_5C2AC0(EMX *);
void func_5D0610(EMX *);
void func_5D9F20(EMX *);
void func_5E6C80(EMX *);
void func_5E7920(EMX *);
void func_5EBA50(EMX *);
void func_600010(EMX *);
void func_60D450(EMX *);
void func_6140B0(EMX *);
void func_6147D0(EMX *);


void em_init_sub(EMX *w) {
    w->prog = em_prog_tbl[w->kind];
    (*w->prog)(w);
    if (w->x01 != 0) {
        w->prim_no = get_prim();
        if (w->prim_no != -1) {
            w->prim = get_prim_ptr(w->prim_no);
            w->prim->owner = w;
            w->prim->trans = (void (*)(PRIM_H *))enemy_trans;
        } else {
            w->prim = 0;
        }
    }
    switch (w->kind) {
    case 1:
        func_566670(w);
        break;
    case 2:
        func_57EFA0(w);
        break;
    case 3:
        func_5873D0(w);
        break;
    case 4:
        func_58B850(w);
        break;
    case 5:
        func_58B850(w);
        break;
    case 6:
        func_5EBA50(w);
        break;
    case 7:
        func_58F8B0(w);
        break;
    case 8:
        func_59A2C0(w);
        break;
    case 9:
        func_5A7F70(w);
        break;
    case 10:
        func_5AD5A0(w);
        break;
    case 11:
        func_566670(w);
        break;
    case 12:
        func_5AF530(w);
        break;
    case 13:
        func_5D0610(w);
        break;
    case 14:
        func_5B52D0(w);
        break;
    case 15:
        func_5C2AC0(w);
        break;
    case 16:
        func_5D0610(w);
        break;
    case 17:
        func_5D9F20(w);
        break;
    case 18:
        func_5E6C80(w);
        break;
    case 19:
        func_5E7920(w);
        break;
    case 20:
        func_5EBA50(w);
        break;
    case 21:
        func_600010(w);
        break;
    case 22:
        func_5D9F20(w);
        break;
    case 23:
        func_5A7F70(w);
        break;
    case 24:
        func_5E7920(w);
        break;
    case 25:
        func_5AF530(w);
        break;
    case 26:
        func_5B52D0(w);
        break;
    case 27:
        func_60D450(w);
        break;
    case 28:
        func_60D450(w);
        break;
    case 29:
        func_6140B0(w);
        break;
    case 30:
        func_5D0610(w);
        break;
    case 31:
        func_60D450(w);
        break;
    case 32:
        func_58B850(w);
        break;
    case 33:
        func_6147D0(w);
        break;
    case 34:
        func_59A2C0(w);
        break;
    }
}
