/* MH2 only. MH1's browser globals (bsFirstOpen, bsTimer0, BsToolMenuReq...) live in one allocated block in MH2, reached
   through the pointer at 0x895270, the slot that is bsUrl in MH1. Offsets read from MH2's code. */
#ifndef MH2_LB_H
#define MH2_LB_H
extern char *bsBw;
typedef struct BSBW { u8 _p[0xDF0]; s32 outerHdl[20]; s16 outerW[20]; s16 outerH[20]; } BSBW;
#define BW8(o)  (*(u8 *)(bsBw + (o)))
#define BW16(o) (*(u16 *)(bsBw + (o)))
#define BW32(o) (*(s32 *)(bsBw + (o)))
#define BW_FIRSTOPEN    BW8(0xE9C)
#define BW_RETRYCTR     BW8(0xE9D)
#define BW_MAINRET      (*(s8 *)(bsBw + 0xE98))
#define BW_NETERROCCUR  BW8(0xE94)
#define BW_TOOLMENUREQ  BW8(0x8EB4)
#define BW_TIMER0       BW16(0x1C27E)
#define BW_GOHIDEPAGE   BW8(0xE99)
#define BW_OW           ((s32 *)(bsBw + 0x5EC))
#define BW_OUTERHDL(i)  (*(s32 *)(bsBw + (i) * 4 + 0xDF0))
#define BW_OUTERW(i)    (*(s16 *)(bsBw + (i) * 2 + 0xE40))
#define BW_OUTERH(i)    (*(s16 *)(bsBw + (i) * 2 + 0xE68))
#define BW_DIALOGREQ    BW8(0x8EB7)
#define BW_CURSORREQ    BW8(0x8EB5)
#endif
