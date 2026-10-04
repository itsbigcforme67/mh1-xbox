/* Controller read (swset solved with decomp-permuter): raw pad state (Psw) to the per-port buffers used by
 * sw_set_sub, with scripted input (swset_w). SLPM_654.95 0x00163810-0x00163AB0. */
#include "pl.h"
#include "game.h"

typedef struct PSW {
    u16 sw;             /* 0x00 buttons */
    u8 _pad02[6];
    u16 an_sw;          /* 0x08 analog-derived buttons */
    u8 _pad0A[6];
    u16 ang[2];         /* 0x10 */
    u16 pow[2];         /* 0x14 */
    u8 _pad18[0x22 - 0x18];
} PSW;

/* Scripted input per player: data is {buttons, duration} pairs ending in a
 * 0xFFFF duration. */
typedef struct SWSET_W {
    u8 _pad00[0x0A];
    u8 on[2];           /* 0x0A */
    u16 *ptr[2];        /* 0x0C */
    u16 timer[2];       /* 0x14 */
    u8 _pad18[0x20 - 0x18];
} SWSET_W;

extern PSW Psw[4];
extern SWSET_W swset_w;
extern s16 Plsw_buff[2][2];
extern u16 Plan_buff[2][2];
extern u16 Plan_ang[2][2];
extern u16 Plan_pow[2][2];

static u16 get_sw(int no);
static void get_ana_sw(int no);

void swset(void) {
    int i;

    for (i = 0; i < 2; i++) {
        Plsw_buff[i][1] = Plsw_buff[i][0];
        Plsw_buff[i][0] = 0;
        Plan_buff[i][1] = Plan_buff[i][0];
        Plan_buff[i][0] = 0;
        Plan_pow[i][1] = Plan_pow[i][0] = Plan_ang[i][1] = Plan_ang[i][0] = 0;
    }
    if (game_w.pad_on != 0) {
        for (i = 0; i < 2; i++) {
            Plsw_buff[i][0] = get_sw(i);
            get_ana_sw(i);
        }
    }
}

static u16 get_sw(int no) {
    u16 sw;
    u16 *p;

    if (swset_w.on[no] == 0) {
        sw = Psw[game_w.port[no]].sw;
        if (game_w.sw_mask != 0) {
            game_w.sw_mask &= sw;
            sw &= ~game_w.sw_mask;
        }
        return sw & 0xFFFF;
    }
    p = swset_w.ptr[no];
    if (--swset_w.timer[no] == 0) {
        if (*++p == 0xFFFF) {
            swset_w.on[no] = 0;
            return 0;
        }
        swset_w.timer[no] = *p++;
        swset_w.ptr[no] = p;
    }
    sw = *p;
    if ((player_work[no].sw_cfg & 1) && (sw & 0xC00)) {
        sw ^= 0xC00;
    }
    return sw;
}

static void get_ana_sw(int no) {
    Plan_buff[no][0] = Psw[no].an_sw;
    Plan_ang[no][0] = Psw[no].ang[0];
    Plan_ang[no][1] = Psw[no].ang[1];
    Plan_pow[no][0] = Psw[no].pow[0];
    Plan_pow[no][1] = Psw[no].pow[1];
}
