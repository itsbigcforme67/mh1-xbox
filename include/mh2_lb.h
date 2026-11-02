/* MH2 only. MH1's browser globals (bsFirstOpen, bsTimer0, BsToolMenuReq...) live in one allocated block in MH2, reached
   through the pointer at 0x895270, the slot that is bsUrl in MH1. Offsets read from MH2's code. */
#ifndef MH2_LB_H
#define MH2_LB_H
#include "version.h"
extern char *bsBw;
typedef struct PLM {           /* the fields of MH2's player work that lobby code uses; 0x970 bytes */
    u8 be_flag;
    u8 _01[0xB];
    u16 id;                    /* 0x0C */
    u8 _0E[0x52C - 0xE];
    void *prim_ptr;            /* 0x52C */
    s16 prim;                  /* 0x530 */
    u8 _532[0x81C - 0x532];
    u8 x81C;
    u8 _81D[0x85C - 0x81D];
    u16 trade_item;            /* 0x85C */
    s16 trade_num;             /* 0x85E */
    u8 _860;
    u8 trade_pl;               /* 0x861 */
    u8 trade_st;               /* 0x862 */
    u8 _863[0x970 - 0x863];
} PLM;
#define PL_W904(p) F(u16, p, 0x85C)
#define PL_W906(p) F(s16, p, 0x85E)
#define PL_W909(p) F(u8, p, 0x861)
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
#define BW_DIALOGREQ    BW8(0x8EB7)
#define BW_CURSORREQ    BW8(0x8EB5)
#endif
