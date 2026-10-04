/* Controller vibration. SLPM_654.95 0x001698B0-0x00169988. */
#include "pl.h"

typedef struct OPTION_W {
    u8 _pad0[3];
    s8 vib_off;         /* 0x3 vibration disabled in options */
    u8 _pad4[0x1200 - 4];
} OPTION_W;

extern OPTION_W option_w;
extern s32 vib_tbl[5][2];   /* {strength, time} */

int Pl_master_ck(PLW *);
void flPADShockSet(int port, s32, s32);

static int vib_check(void) {
    return option_w.vib_off == 0;
}

void vib_set(int port, int no) {
    if (vib_check() == 0) {
        flPADShockSet(port, vib_tbl[no][0], vib_tbl[no][1]);
    }
}

void vib_set_pl(PLW *pl, int no) {
    if (vib_check() == 0 && Pl_master_ck(pl) != 0) {
        flPADShockSet(0, vib_tbl[no][0], vib_tbl[no][1]);
    }
}
