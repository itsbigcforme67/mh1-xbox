/* misc01 - yn.bin misc 0x0053AD50-0x0053AD64: yn_mc_init. Whole file in misc_nm.c. */
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
