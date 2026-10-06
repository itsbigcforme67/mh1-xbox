/* yn.bin 0x0053AD50-0x0053AF00 and 0x0053530C-0x00535340: memory card init / device check and
   yn_set_init (near-match: yn_mc_device_check_all is not tuned). */
#include "yn.h"

void McActInit(int);
void McActCheckSet(void);
void McActMain(void);
void flfntInit(void);

/* main's MemcardWork (0x5317C0, 0xB4 bytes); offsets from the code below, meanings are guesses. */
typedef struct MCW {
    u8 _pad00[0x14];
    s32 x14;                /* 0x14 */
    u8 _pad18[0x34 - 0x18];
    s32 state[2];           /* 0x34 per card: 1/2 = card present, 3 = formatted (guess) */
    u8 _pad3C[0xA4 - 0x3C];
    s32 busy;               /* 0xA4 */
    u8 _padA8[0xB4 - 0xA8];
} MCW;
extern MCW MemcardWork;

extern s32 yn_r_no;
extern s32 yn_type;

/* work: +0 state, +1 ?, +2 / +3 bitmasks of cards (bit n+1 = card n), +4 flag */
void yn_mc_init(u8 *work) {
    M2C_FIELD(work, s8 *, 0) = 0;
    M2C_FIELD(work, s8 *, 1) = 0;
    M2C_FIELD(work, s8 *, 4) = 0;
    McActInit(2);
}

typedef struct MCD {
    s8 st;          /* 0 state */
    s8 x1;          /* 1 */
    u8 inserted;    /* 2 bit n+1 = card n present */
    u8 formatted;   /* 3 bit n+1 = card n formatted */
    s8 started;     /* 4 */
} MCD;

s32 yn_mc_device_check_all(MCD *w) {
    s8 st;
    s32 i;

    st = w->st;
    switch (st) {
    case 0:
        if (MemcardWork.busy != 0) {
            break;
        }
        w->st = st + 1;
        w->started = 0;
        w->inserted = 0;
        w->formatted = 0;
        McActInit(2);
        MemcardWork.x14 = 0;
        MemcardWork.state[0] = -1;
        MemcardWork.state[1] = -1;
        McActCheckSet();
    case 1:
        McActMain();
        if (MemcardWork.state[0] != -1) {
            st = w->started;
            if (st == 0) {
                w->started = st + 1;
                MemcardWork.x14 = 1;
            }
        }
        if (MemcardWork.state[0] == -1) {
            break;
        }
        if (MemcardWork.state[1] == -1) {
            break;
        }
        for (i = 0; i < 2; i++) {
            if (MemcardWork.state[i] == 1 || MemcardWork.state[i] == 2) {
                w->inserted |= (1 << (i + 1)) & 0xFF;
            }
            if (MemcardWork.state[i] == 3) {
                w->formatted |= (1 << (i + 1)) & 0xFF;
            }
        }
        w->st = 0;
        McActInit(2);
        return w->formatted | w->inserted;
    default:
        break;
    }
    return -1;
}

s32 yn_set_init(s32 type) {
    yn_type = type;
    yn_r_no = 0;
    flfntInit();
    return 0;
}
