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
    void (*cb)();       /* 0x04 completion callback of the request */
    u8 _pad08[0x10];
    s32 val;            /* 0x18 request argument (start index / count) */
    u8 _pad1C[4];
    u8 state;           /* 0x20 (CnetSys_w+0xE38) 1 = run */
    s8 x21;             /* 0x21 progress */
    s8 res;             /* 0x22 result: 1 ok, 2 failed */
    u8 _pad23;
} CNET_BURST;

/* plaza / lobby / room table entries (0x164 bytes each); the entry of id n is table[n - 1] */
typedef struct CNET_PIECE {
    u32 flags;          /* 0x00 which parts have been received (bit 2 name, 2 status, 8 explain, ...) */
    s32 prop;           /* 0x04 room property */
    u16 id;             /* 0x08 */
    u16 ja;             /* 0x0A joined users (mh) */
    u16 jb;             /* 0x0C */
    u16 ri[5];          /* 0x0E room join info */
    u16 ma;             /* 0x18 match entry users */
    u16 mb;             /* 0x1A */
    u8 status;          /* 0x1C */
    u8 pwinfo;          /* 0x1D password info */
    char name[0x42];    /* 0x1E */
    char explain[0x104];/* 0x60 */
} CNET_PIECE;

/* personal data of the hunter (copied by value, 0x1D0 bytes) */
typedef struct CNET_PDATA {
    char name[0x41];    /* 0x00 */
    char zip[0xB];      /* 0x41 */
    char address[0x81]; /* 0x4C */
    char tel[0x81];     /* 0xCD */
    u8 age;             /* 0x14E */
    char mail[0x81];    /* 0x14F */
} CNET_PDATA;

/* condition-search request (copied by value, 0x224 bytes) */
typedef struct CNET_COND {
    u8 b[0x224];
} CNET_COND;

/* blobs copied by value out of CnetSys_w (struct assignment; the element type fixes the copy loop) */
typedef struct CNET_B5C { u8 b[0x5C]; } CNET_B5C;
typedef struct CNET_B308 { u8 b[0x308]; } CNET_B308;
typedef struct CNET_B1004 { u8 b[0x1004]; } CNET_B1004;
typedef struct CNET_H1004 { s16 h[0x802]; } CNET_H1004;
typedef struct CNET_W5D4 { s32 w[0x175]; } CNET_W5D4;
typedef struct CNET_RULEENT {   /* one room rule (0x14A5 bytes) */
    u8 flags;           /* 0x00 which parts have been received */
    char head[0x41];    /* 0x01 head word */
    u8 perm;            /* 0x42 permission */
    u8 numof;           /* 0x43 number of choices */
    u8 now;             /* 0x44 current choice */
    u8 cflag[0x20];     /* 0x45 per-choice received flags */
    char names[0x1440]; /* 0x65 choice names, 0x41 bytes each */
} CNET_RULEENT;
typedef struct CNET_RULETBL {   /* room rule allocation table at CnetSys_w+0x6E48 */
    u8 name_perm;       /* 0x00 */
    u8 pw_perm;         /* 0x01 */
    u8 explain_perm;    /* 0x02 */
    u8 _pad03;
    CNET_RULEENT e[32]; /* 0x04 */
} CNET_RULETBL;
typedef struct CNET_CHAT {
    char from[8];       /* 0x00 sender */
    char x[0x14];       /* 0x08 */
    char msg[0x100];    /* 0x1C text */
    u8 a, b, c, d;      /* 0x11C */
} CNET_CHAT;

typedef struct CNET_FIRSTDATA { s16 h[18]; } CNET_FIRSTDATA;    /* 0x24 bytes, halfword copy loop */
typedef struct CNET_LOGIN {     /* 0x40 bytes, copied by value (word copy loop) */
    u8 x00[2];          /* 0x00 */
    char key[0xB];      /* 0x02 key encoded into the connection-pair packet */
    char pass[0x13];    /* 0x0D */
    s32 patch_buf;      /* 0x20 patch download buffer */
    u8 x24[8];          /* 0x24 */
    char tel[0x14];     /* 0x2C telephone number */
} CNET_LOGIN;
typedef struct CNET_BUF2000 { u8 b[0x2000]; } CNET_BUF2000;
typedef struct CNET_LUSER {     /* a user returned by the login server (0x5C bytes) */
    char id[8];         /* 0x00 */
    char handle[0x10];  /* 0x08 */
    u8 pad18[4];        /* 0x18 */
    u8 mini[0x40];      /* 0x1C mini data */
} CNET_LUSER;

/* room rule block (0x16B bytes) */
typedef struct CNET_RULE {
    u8 b[0x16B];
} CNET_RULE;

typedef struct CNET_SYS {
    s32 active;  /* 0x000  */
    u8 _pad004[0x10];
    s8 x14;  /* 0x014  */
    u8 _pad015[0x3];
    CNET_BG bg[0x80];  /* 0x018 background request slots */
    CNET_BURST burst[12];  /* 0xE18 burst slots */
    u8 _padFC8[0x14];
    s32 sock;  /* 0xFDC socket handle */
    s32 rlen;  /* 0xFE0 bytes read by the last select */
    s16 rcmd;  /* 0xFE4 command of the last packet */
    u16 rcat;  /* 0xFE6 category of the last packet */
    u16 rseq;  /* 0xFE8 sequence of the last packet */
    u16 rseq2;  /* 0xFEA  */
    s8 rres;  /* 0xFEC result byte */
    u8 _padFED[0x3];
    s32 xff0;  /* 0xFF0  */
    s32 xff4;  /* 0xFF4  */
    u8* xff8;  /* 0xFF8  */
    s32 patch_ptr;  /* 0xFFC  */
    s32 patch_cnt;  /* 0x1000  */
    s16 x1004;  /* 0x1004  */
    u16 patch_x;  /* 0x1006  */
    s32 patch_ver;  /* 0x1008 patch information version */
    s32 patch_size;  /* 0x100C  */
    u8 _pad1010[0x4];
    char patch_a[0x10];  /* 0x1014 patch information string */
    char patch_b[8];  /* 0x1024 patch information string 2 */
    u16 rcnt;  /* 0x102C receive counter */
    u16 echo_sum;  /* 0x102E  */
    u8 echo_n;  /* 0x1030  */
    u8 _pad1031[0x3];
    CNET_LOGIN login;  /* 0x1034 login configuration (copied by value) */
    CNET_B5C acct;  /* 0x1074 account being logged in */
    CNET_FIRSTDATA firstdata;  /* 0x10D0 first data sent after login */
    u8 _pad10F4[0x6];
    CNET_PDATA pdata;  /* 0x10FA personal data being registered */
    CNET_RULE rule;  /* 0x12CA room rule being set */
    u8 _pad1435[0x29];
    u8 n_login_user;  /* 0x145E  */
    u8 _pad145F[0x3];
    CNET_LUSER login_users[4];  /* 0x1462 users returned by the login server (entry 3 is the account being logged in) */
    char decide_id[8];  /* 0x15D2 decided user id */
    char decide_handle[0x10];  /* 0x15DA decided user handle */
    u8 _pad15EA[0x44];
    CNET_B5C minidata;  /* 0x162E mini data of a lobby member */
    CNET_B1004 topinfo;  /* 0x168A top information */
    CNET_H1004 warnmsg;  /* 0x268E login warning message */
    CNET_B5C leave_user;  /* 0x3692 user who left the room */
    u8 lobby_member[0x300];  /* 0x36EE  */
    u8 room_member[0x300];  /* 0x39EE  */
    u8 annex_member[0x300];  /* 0x3CEE  */
    u8 extra_member[0x60];  /* 0x3FEE extra member entry */
    u8 _pad404E[0x16];
    CNET_PIECE plaza[10];  /* 0x4064 plaza table (counts of plaza/lobby/room at 0x404E/0x4050/0x4052 just before) */
    CNET_PIECE lobby[14];  /* 0x4E4C lobby table */
    CNET_PIECE room[8];  /* 0x61C4 room table */
    u8 _pad6CE4[0x8];
    u16 last_id;  /* 0x6CEC id of the last received plaza/lobby/room item */
    u8 _pad6CEE[0x12];
    u8 last_status;  /* 0x6D00 status of the last item */
    u8 last_pwinfo;  /* 0x6D01  */
    char last_name[0x42];  /* 0x6D02 name of the last item */
    u8 _pad6D44[0x104];
    CNET_RULETBL ruletbl;  /* 0x6E48 room rule allocation table */
    u8 n_lobby_member;  /* 0x302EC  */
    u8 n_room_member;  /* 0x302ED  */
    u8 n_annex_member;  /* 0x302EE  */
    u8 _pad302EF[0xF];
    u16 resttime;  /* 0x302FE room rest time */
    u8 gsaddr[4];  /* 0x30300 game server address bytes */
    u8 _pad30304[0x4];
    u8 gsport[2];  /* 0x30308 game server port bytes */
    u8 _pad3030A[0x6];
    CNET_W5D4 matchinfo;  /* 0x30310 match information */
    u8 _pad308E4[0x2A6];
    CNET_CHAT chat;  /* 0x30B8A chat message being received */
    u8 _pad30CAA[0x690E];
    CNET_B308 chatbin;  /* 0x375B8 chat binary */
    u8 _pad378C0[0x300];
    CNET_BUF2000 loginbuf;  /* 0x37BC0  */
    u8 _pad39BC0[0x1E98];
    s16 curplace[3];  /* 0x3BA58 current place (3 values) */
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
