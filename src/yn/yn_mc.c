/* Yes/No (network setup) overlay: memory card helpers. yn.bin 0x0053AF00-0x0053B258.
   The work struct (arg) is a guess: +1 = state, +5 = current slot. */
#include "types.h"

typedef struct YN_MC {
    u8 _pad0[1];
    s8 state;       /* 0x1 */
    u8 _pad2[3];
    s8 slot;        /* 0x5 */
} YN_MC;

typedef struct MC_W { u8 _pad0[0xA4]; s32 busy; u8 _padA8[0xB4 - 0xA8]; } MC_W;
extern MC_W MemcardWork;             /* main 0x5317C0; busy flag at +0xA4 is a guess */
extern char lit_197_00541180[];      /* file name pattern (rodata) */
extern char lit_230_00541190[];
extern u8 table[];

void McActInit(int);
void McActFormatSet(int);
void McActListSet(int, char *, u8 *);
void McActMain(void);
s32 McActResult(void);

s32 yn_mc_error_conv(s32);

s32 yn_mc_format(YN_MC *w) {
    switch (w->state) {
    case 0:
        if (w->slot >= 2) {
            return -15;
        }
        if (MemcardWork.busy == 0) {
            w->state++;
            McActInit(2);
            McActFormatSet(w->slot);
    case 1:
            McActMain();
            {
                s32 r = McActResult();
                if (r != -1) {
                    w->state = 0;
                    return yn_mc_error_conv(r);
                }
            }
        }
    default:
        return -2;
    }
}

void yn_mc_set_current(YN_MC *w, s8 slot) {
    w->slot = slot;
}

s8 yn_mc_get_current(YN_MC *w) {
    return w->slot;
}

s32 yn_mc_ynfile_check(YN_MC *w) {
    switch (w->state) {
    case 0:
        if (w->slot >= 2) {
            return -15;
        }
        if (MemcardWork.busy == 0) {
            w->state++;
            McActInit(2);
            McActListSet(w->slot, lit_197_00541180, table);
    case 1:
            McActMain();
            {
                s32 r = McActResult();
                if (r != -1) {
                    w->state = 0;
                    if (r < 0) {
                        return yn_mc_error_conv(r);
                    }
                    return -1;
                }
            }
        }
    default:
        return -2;
    }
}

void yn_mc_gmfile_current_set(void) {
}

s32 yn_mc_gmfile_check(YN_MC *w) {
    switch (w->state) {
    case 0:
        if (w->slot >= 2) {
            return -15;
        }
        if (MemcardWork.busy == 0) {
            w->state++;
            McActInit(2);
            McActListSet(w->slot, lit_230_00541190, table);
    case 1:
            McActMain();
            {
                s32 r = McActResult();
                if (r != -1) {
                    w->state = 0;
                    if (r < 0) {
                        return yn_mc_error_conv(r);
                    }
                    return -1;
                }
            }
        }
    default:
        return -2;
    }
}

s32 yn_mc_gmfile_save(void) {
    return -1;
}

s32 yn_mc_error_conv(s32 e) {
    switch (e) {
    case 0:
        return -1;
    case -1:
        return -2;
    case -256:
        return -3;
    case -255:
        return -4;
    case -254:
        return -5;
    case -253:
        return -7;
    case -252:
        return -6;
    default:
        return -3;
    }
}
