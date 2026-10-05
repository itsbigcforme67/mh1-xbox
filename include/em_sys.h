#ifndef EM_SYS_H
#define EM_SYS_H
/* Types used by the shared monster code (em_master.c, em_taisei.c...). */
#include "em.h"

/* Per-kind status tables (em_sleep_data_tbl, em_sleep2_data_tbl,
 * em_poison_data_tbl, em_mahi_data_tbl): tolerance and timers. */
typedef struct EM_TAISEI_DATA {
    s16 start;          /* 0x0 tolerance at start */
    s16 decay;          /* 0x2 frames between decreases of the stocked damage */
    s16 dec;            /* 0x4 stocked damage removed each time */
    s16 time;           /* 0x6 length of the state (work08); poison: damage per tick */
    s16 add;            /* 0x8 tolerance added each time it wears off; poison: duration */
    s16 tick;           /* 0xA poison: frames between damage ticks */
    s16 add2;           /* 0xC poison: tolerance added when it wears off */
} EM_TAISEI_DATA;

/* Smell clouds (smell_dmg_set, smell_ptr_ret). */
typedef struct EM_SMELL {
    f32 pos[3];         /* 0x00 */
    f32 range;          /* 0x0C */
    u8 x10;             /* 0x10 copied to em->x951 */
    u8 x11;             /* 0x11 copied to em->x952 */
    u8 type;            /* 0x12 0 food, 1 poison, 2 sleep, 3 paralysis */
    u8 eaten;           /* 0x13 */
    u8 stg;             /* 0x14 */
    u8 _pad15;
    s16 val;            /* 0x16 */
    u8 _pad18[0x30 - 0x18];
    f32 pos2[3];        /* 0x30 */
} EM_SMELL;

extern EM_TAISEI_DATA *em_sleep_data_tbl[];
extern EM_TAISEI_DATA *em_sleep2_data_tbl[];
extern EM_TAISEI_DATA *em_poison_data_tbl[];
extern EM_TAISEI_DATA *em_mahi_data_tbl[];



/* Entries of senko_stack / smoke_stack / em_yobi_stack: flash bombs,
 * smoke and calls for help placed in the world. */
typedef struct EM_SPOT {
    f32 pos[3];         /* 0x00 */
    f32 range;          /* 0x0C */
    u8 pl;              /* 0x10 player who made it */
    u8 _pad11;
    u8 flag;            /* 0x12 */
    u8 _pad13;
    u8 stg;             /* 0x14 */
} EM_SPOT;

/* View of a monster for em_eye_search_set and the sight checks. */
typedef struct EM_EYE {
    f32 *pos;           /* 0x0 eye position */
    f32 *tgt;           /* 0x4 looked-at position */
    s32 ang;            /* 0x8 facing angle */
    u16 fov;            /* 0xC half field of view */
} EM_EYE;

/* em_search_tbl[kind][no] (32 bytes): sight ranges. */
typedef struct EM_SEARCH {
    f32 dist;           /* 0x00 */
    f32 up;             /* 0x04 */
    f32 down;           /* 0x08 */
    f32 xC;             /* 0x0C */
    u16 fov;            /* 0x10 */
    u8 _pad12[6];
    f32 smell;          /* 0x18 smell range */
    f32 kehai;          /* 0x1C presence (kehai) range */
} EM_SEARCH;
#endif
