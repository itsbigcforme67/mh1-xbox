/* misc00 - yn.bin misc 0x00535310-0x00535334: yn_set_init. Whole file in misc_nm.c. */
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




s32 yn_set_init(s32 type) {
    yn_type = type;
    yn_r_no = 0;
    flfntInit();
    return 0;
}
