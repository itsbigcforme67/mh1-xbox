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
    void (*run)(int);   /* 0x00 job function, called each frame while state == 1 */
    void *cb;           /* 0x04 completion callback of the request */
    u8 _pad08[0x10];
    s32 val;            /* 0x18 request argument (start index / count) */
    u8 _pad1C[4];
    u8 state;           /* 0x20 (CnetSys_w+0xE38) 1 = run */
    s8 x21;             /* 0x21 progress */
    u8 _pad22[2];
} CNET_BURST;

/* plaza / lobby / room table entries (0x164 bytes each), indexed from 0 as (id - 1) */
typedef struct CNET_PIECE {     /* plaza and room */
    s32 prop;           /* 0x00 room property */
    u8 _pad04[0x14];
    u8 status;          /* 0x18 */
    u8 pwinfo;          /* 0x19 password info */
    char name[0x42];    /* 0x1A */
    char explain[0x108];/* 0x5C */
} CNET_PIECE;

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
    u8 _padFED[0x1B];
    s32 patch_ver;  /* 0x1008 patch information version */
    u8 _pad100C[0x8];
    char patch_a[0x10];  /* 0x1014 patch information string */
    char patch_b[8];  /* 0x1024 patch information string 2 */
    u16 rcnt;  /* 0x102C receive counter */
    u8 _pad102E[0x32];
    char tel[0x14];  /* 0x1060 telephone number (personal data) */
    u8 _pad1074[0x502];
    char uid[8];  /* 0x1576 user id string */
    char uhandle[0x40];  /* 0x157E user handle string */
    u8 _pad15BE[0x70];
    u8 minidata[0x5C];  /* 0x162E mini data of a lobby member */
    u8 _pad168A[0x2064];
    u8 lobby_member[0x300];  /* 0x36EE  */
    u8 room_member[0x300];  /* 0x39EE  */
    u8 annex_member[0x300];  /* 0x3CEE  */
    u8 _pad3FEE[0x7A];
    CNET_PIECE plaza[10];  /* 0x4068 plaza table, entry for id n is plaza[n - 1] (counts of plaza/lobby/room at 0x404E/0x4050/0x4052 just before it) */
    CNET_PIECE lobby[14];  /* 0x4E50 lobby table */
    CNET_PIECE room[8];  /* 0x61C8 room table */
    u8 _pad6CE8[0x29604];
    u8 n_lobby_member;  /* 0x302EC  */
    u8 n_room_member;  /* 0x302ED  */
    u8 n_annex_member;  /* 0x302EE  */
    u8 _pad302EF[0x21];
    u8 matchinfo[0x5D4];  /* 0x30310 match information */
    u8 _pad308E4[0x2A6];
    char chat_from[8];  /* 0x30B8A chat message sender */
    char chat_x[0x14];  /* 0x30B92  */
    char chat_msg[0x100];  /* 0x30BA6 chat message text */
    u8 chat_a;  /* 0x30CA6  */
    u8 chat_b;  /* 0x30CA7  */
    u8 chat_c;  /* 0x30CA8  */
    u8 chat_d;  /* 0x30CA9  */
} CNET_SYS;
extern CNET_SYS CnetSys_w;

typedef long long s64;
typedef unsigned long long u64;
#define CNWP(off) ((u8 *)&CnetSys_w + (off))

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
