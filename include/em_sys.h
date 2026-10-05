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
    u8 _pad00[0x12];
    u8 type;            /* 0x12 0 food, 1 poison, 2 sleep, 3 paralysis */
    u8 eaten;           /* 0x13 */
    u8 _pad14[2];
    s16 val;            /* 0x16 */
} EM_SMELL;

extern EM_TAISEI_DATA *em_sleep_data_tbl[];
extern EM_TAISEI_DATA *em_sleep2_data_tbl[];
extern EM_TAISEI_DATA *em_poison_data_tbl[];
extern EM_TAISEI_DATA *em_mahi_data_tbl[];

#endif
