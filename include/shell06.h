#ifndef SHELL06_H
#define SHELL06_H
/* shell06: bowgun shots. Types shared by the shell06 split files.
 * Offsets from matched code (shell06*.c). */
#include "shell.h"
#include "pl.h"
#include "prim.h"

/* One row of the weapon's growth table (Gun_Grow_Up_DATA[n][ammo & 0xF]).
 * Silencer_Grow_Up_Tbl and LBarrel_Grow_Up_Tbl are single rows added on
 * when the silencer (ammo bit 0x10) or long barrel (bit 0x20) is fitted. */
typedef struct GROW_UP {
    u8 _pad00[4];
    f32 spd;            /* 0x04 added to the shot's base speed */
    f32 spread;         /* 0x08 scales the random part of the speed */
    f32 drop;           /* 0x0C scales the random part of the gravity */
    u8 _pad10[4];
    f32 range;          /* 0x14 scales shell06_change_time (the fall-off steps) */
} GROW_UP;

/* Gun_data: 20 bytes per gun. */
typedef struct GUN_DATA {
    u8 _pad00[2];
    u8 grow_no;         /* 0x02 row of Gun_Grow_Up_DATA */
    u8 _pad03[0x14 - 3];
} GUN_DATA;

/* Shell_data: 4 bytes per shot type. */
typedef struct SHELL_DATA {
    u8 _pad00;
    u8 se_kind;         /* 0x01 firing sound: 0, 1 or 2 (shell06_se_req) */
    u8 _pad02[2];
} SHELL_DATA;

/* shell06_param_tbl: 36 bytes per shot type (sh->arg). */
typedef struct SH06P {
    u8 kind;            /* 0x00 0/1/4: straight, 2: scattered (pellets) */
    u8 x01;             /* 0x01 0x62: has a muzzle flash that follows the shot */
    u8 _pad02;
    u8 split;           /* 0x03 0xFF: splits (sets w->x16 to 2) */
    s16 time_no;        /* 0x04 row of shell06_change_time, -1 = no fall-off */
    u8 _pad06[2];
    f32 spd;            /* 0x08 */
    f32 spread;         /* 0x0C */
    f32 grav;           /* 0x10 */
    f32 drop;           /* 0x14 */
    u8 _pad18[0x24 - 0x18];
} SH06P;

/* Muzzle-flash slot. */
typedef struct SH06E {
    PRIM *prim;         /* 0x00 */
    s16 prim_no;        /* 0x04 */
    s16 x06;            /* 0x06 */
    s16 cnt;            /* 0x08 frames shown; bit 0 picks the scale row */
    s16 x0A;            /* 0x0A */
} SH06E;

/* The per-shot work that sh->senko (0x18) points at for shell06. */
typedef struct SH06W {
    u8 x00;             /* 0x00 */
    u8 state;           /* 0x01 fall-off step, 0xFF = done */
    u8 x02;             /* 0x02 */
    u8 x03;             /* 0x03 */
    VEC3 pos;           /* 0x04 muzzle position */
    f32 atk;            /* 0x10 attack power at the muzzle */
    s16 x14;            /* 0x14 frames in state 1 */
    u8 x16;             /* 0x16 */
    u8 x17;             /* 0x17 non-zero: has a muzzle flash */
    f32 atk_rate;       /* 0x18 copied from the player */
    GROW_UP *grow;      /* 0x1C */
    u8 ammo;            /* 0x20 copied from the player */
    u8 _pad21[3];
    SH06E eft[1];       /* 0x24 */
} SH06W;

#define SH06_W(sh) ((SH06W *)(sh)->senko)

extern SH06P shell06_param_tbl[];
extern GUN_DATA Gun_data[];
extern SHELL_DATA Shell_data[];
extern GROW_UP *Gun_Grow_Up_DATA[];
extern GROW_UP Silencer_Grow_Up_Tbl;
extern GROW_UP LBarrel_Grow_Up_Tbl;

void shell06_move(SHLW *sh);
void shell06_init_sub(SHLW *sh);
void shell06_move_sub(SHLW *sh);
void shell06_hit(SHLW *sh);
void shell06_trans_sub(PRIM *pr);
void shell06_i(SHLW *sh);
void shell06_m(SHLW *sh);
void shell06_d(SHLW *sh);
void shell06_e(SHLW *sh);
void shell06_trans(PRIM *pr);
void shell06_get_weaopn_data(SH06W *w, PLW *pl);
void shell06_change_atck_data(SHLW *sh);
int shell06_time_ck(SHLW *sh);
void shell06_atck_data_calc(SHLW *sh);
void sleeve_trans(SHLW *sh);
u16 calc_rand(u16 a, u16 b, u16 c, u16 d);
f32 rand_sub(u16 seed, u16 bits);
f32 rand_sub2(u16 seed, u16 bits);
void shell06_se_req(SHLW *sh, s16 kind);
void shell06_eft_i(SHLW *sh);
void shell06_eft_m(SHLW *sh);
void shell06_eft_d(SHLW *sh);
void shell06_eft_t(PRIM *pr);

#endif
