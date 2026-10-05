#ifndef LBNET_H
#define LBNET_H
/* lobby network layer (cnLBS / __cnet*). CnetSys_w is one big work area of
 * the online lobby client; fields are added as functions are matched. */
#include "types.h"

/* event/result record passed by value (8 bytes) to notice and completion callbacks */
typedef struct CNET_RES {
    s8 val;             /* 0x00 result (0 ok, -1 error; 1 for events) */
    s8 id;              /* 0x01 event id */
    u8 _pad02[6];
} CNET_RES;

/* background request slot: 0x80 slots at CnetSys_w+0x18, stride 0x1C */
typedef struct CNET_BG {
    void (*cb)();       /* 0x00 (CnetSys_w+0x18) per-frame run callback, else completion callback */
    void (*done)();     /* 0x04 (CnetSys_w+0x1C) completion callback */
    u8 _pad08[0x0E];
    u16 cmd;            /* 0x16 (CnetSys_w+0x2E) command id the reply is matched against */
    u8 state;           /* 0x18 (CnetSys_w+0x30) 0 free, 1 waiting for reply, 2 running */
    u8 x19;             /* 0x19 (CnetSys_w+0x31) */
    u8 _pad1A[2];
} CNET_BG;

/* burst slot: 12 slots at CnetSys_w+0xE18, stride 0x24 */
typedef struct CNET_BURST {
    void (*run)(int);   /* 0x00 */
    u8 _pad04[0x1C];
    u8 state;           /* 0x20 (CnetSys_w+0xE38) 1 = run */
    u8 _pad21[3];
} CNET_BURST;

typedef struct CNET_SYS {
    s32 active;  /* 0x000  */
    u8 _pad004[0x14];
    CNET_BG bg[0x80];  /* 0x018 background request slots */
    CNET_BURST burst[12];  /* 0xE18 burst slots */
    u8 _padFC8[0x14];
    s32 sock;  /* 0xFDC socket handle */
    s32 rlen;  /* 0xFE0 bytes read by the last select */
    s16 rcmd;  /* 0xFE4 command of the last packet */
    s16 rcat;  /* 0xFE6 category of the last packet */
    u16 rseq;  /* 0xFE8 sequence of the last packet */
    u16 rseq2;  /* 0xFEA  */
    s8 rres;  /* 0xFEC result byte */
    u8 _padFED[0x3F];
    u16 rcnt;  /* 0x102C receive counter */
} CNET_SYS;
extern CNET_SYS CnetSys_w;

/* byte access at an offset of CnetSys_w for fields not named yet */
#define CNW(T, off) (*(T *)((u8 *)&CnetSys_w + (off)))

/* send_work (0x006EA740, size 0x310): outgoing packet being built */
typedef struct SEND_WORK {
    u16 total;          /* 0x00 bytes after the header (written to the socket + 0xC) */
    u16 len;            /* 0x02 payload length */
    s8 magic;           /* 0x04 0x81 */
    u8 cat;             /* 0x05 category */
    u8 cmd_h;           /* 0x06 */
    u8 cmd_l;           /* 0x07 */
    u8 len_h;           /* 0x08 */
    u8 len_l;           /* 0x09 */
    u8 seq_h;           /* 0x0A */
    u8 seq_l;           /* 0x0B */
    u8 x0C;             /* 0x0C */
    u8 x0D;             /* 0x0D */
    u8 x0E;             /* 0x0E */
    u8 x0F;             /* 0x0F */
    u8 data[0x300];     /* 0x10 */
} SEND_WORK;
extern SEND_WORK send_work;

#endif
