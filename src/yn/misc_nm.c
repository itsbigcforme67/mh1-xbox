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

s32 yn_mc_device_check_all(u8 *work) {
    s8 st;
    s32 i;
    s32 v;

    st = M2C_FIELD(work, s8 *, 0);
    switch (st) {
    case 0:
        if (MemcardWork.busy == 0) {
            M2C_FIELD(work, s8 *, 0) = st + 1;
            M2C_FIELD(work, s8 *, 4) = 0;
            M2C_FIELD(work, u8 *, 2) = 0;
            M2C_FIELD(work, u8 *, 3) = 0;
            McActInit(2);
            MemcardWork.x14 = 0;
            MemcardWork.state[0] = -1;
            MemcardWork.state[1] = -1;
            McActCheckSet();
    case 1:
            McActMain();
            if ((u8)MemcardWork.state[0] != 0xFF) {
                st = M2C_FIELD(work, s8 *, 4);
                if (st == 0) {
                    M2C_FIELD(work, s8 *, 4) = st + 1;
                    MemcardWork.x14 = 1;
                }
            }
            if ((u8)MemcardWork.state[0] != 0xFF && (u8)MemcardWork.state[1] != 0xFF) {
                for (i = 0; i < 2; i++) {
                    v = MemcardWork.state[i];
                    if (v == 1 || v == 2) {
                        M2C_FIELD(work, u8 *, 2) |= (1 << (i + 1)) & 0xFF;
                    }
                    if (MemcardWork.state[i] == 3) {
                        M2C_FIELD(work, u8 *, 3) |= (1 << (i + 1)) & 0xFF;
                    }
                }
                M2C_FIELD(work, s8 *, 0) = 0;
                McActInit(2);
                return M2C_FIELD(work, u8 *, 3) | M2C_FIELD(work, u8 *, 2);
            }
        }
    default:
        return -1;
    }
}

s32 yn_set_init(s32 type) {
    yn_type = type;
    yn_r_no = 0;
    flfntInit();
    return 0;
}
