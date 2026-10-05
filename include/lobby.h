/* Lobby overlay (online town) shared declarations. Add only; do not change existing lines. */
#ifndef LOBBY_H
#define LOBBY_H
#include "types.h"
#include "pl.h"
#include "game.h"

extern u8 *cw;                 /* client work (D_6DD7E0, 0xBF40 bytes): u8 accessed by offset */
#define CW8(o)  (*(u8 *)(cw + (o)))
#define CWPLAYER(i) (cw + (i) * 0x2FC)  /* per member records start at cw+0x1334 */

typedef struct LBCOMMER { u8 mac[6]; u8 _pad06[2]; char name[0x10]; u8 _pad18[0x5C - 0x18]; } LBCOMMER; /* 0x5C bytes x8 at lbCommer */
extern LBCOMMER lbCommer[8];

typedef struct LBPLAYER { u8 _pad00[4]; u8 x04[0x10]; u8 _pad14[0x38 - 0x14]; } LBPLAYER; /* lb_player 0x38 x8 */
extern LBPLAYER lb_player[8];

typedef struct LBSYS {         /* lb_sys 0x90 bytes */
    u8 _pad00[3];
    s8 x03;                    /* 0x03 mode (4 = send positions) */
    u8 _pad04[0x64 - 4];
    u16 chair_mask;            /* 0x64 bit per occupied chair */
    u16 x66;                   /* 0x66 chair number (sent as a packet) */
    s32 x68;                   /* 0x68 */
    s32 x6C;                   /* 0x6C */
    u8 _pad70[0x85 - 0x70];
    s8 x85;                    /* 0x85 send interval counter */
    u8 _pad86[0x90 - 0x86];
} LBSYS;
extern u8 lbSendInterval;
extern LBSYS lb_sys;

typedef struct LBPOS { f32 x, z; u16 ang; u16 stg; } LBPOS;   /* received position packet (0xC bytes) */
typedef struct LBSTAT { f32 x, z; u16 ang; u8 chair; u8 act14; u8 act15; u8 x0D; u8 _pad0E[2]; } LBSTAT; /* received status packet (0x10 bytes) */
typedef struct LBTRADE2 { u16 item; s16 num; u8 plid[6]; u8 _pad0A[2]; u8 result; u8 _pad0D; } LBTRADE2;
typedef struct LBTRADE { u16 item; u16 num; u8 plid[6]; u8 _pad0A[2]; u8 result; u8 _pad0D; } LBTRADE; /* trade packet (0xE bytes) */
typedef struct LBV3 { f32 x, y, z; } LBV3;
#define PLU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PLS8(p, o) (*(s8 *)((u8 *)(p) + (o)))

void *memcpy(void *, const void *, unsigned int);
void *memset(void *, int, unsigned int);
int memcmp(const void *, const void *, unsigned int);
int sprintf(char *, const char *, ...);
void Chat_log_add(int, void *);
void flMemcpy(void *, void *, int);
int Lb_get_plID();
void Lb_Pl_act_set2();
void Lb_Pl_adj_calc();
void Lb_act_set();
void Lb_send_item_result();
void Lb_send_chair_status();
void Lb_send_chair_release();
void Lb_pl_to_chair();
void Lb_send_pl_status();
void Lb_send_stage();
void Lb_send_trade_startTU();
void Lb_clearChatMember();
void Lb_set_mini_data_to_pl();
void Lb_set_player();
void Lb_player_release();
void Ud_item_stack(u16, s16);
long Pl_item_num_ck();
void set01_set(int, int, int);
s16 act_ck(PLW *, int, int);
extern u8 my_user_mini_data[];
typedef struct LBQUEST { u8 _pad00[4]; s32 fee; u8 _pad08[0x10]; s32 str_ofs; } LBQUEST; /* quest record (get_quest_info) */
extern LBQUEST *lb_quest_all[0xC8];
extern int mission_area;
typedef struct MHRULE { u8 _pad00[0x54]; u32 quest; u8 _pad58[0x68 - 0x58]; } MHRULE;
extern MHRULE mhRule;
extern s32 User_gold;          /* User_data + 0x20 */
void Gold_add(int);
void cnWrap_SoundRequest(int);
void Lbc_init_network_work(int);
void NPCZoomInCameraCancel();
LBQUEST *get_quest_info(void);
int Online_ck();
int SoftKeyboard_alive_check();
void font_set_stack_no(int);
void DispSoftkeyboard(int);
void flfntLocate();
void font_print(char *, ...);
int strlen_sp();
void lb_pl_chr_set_com();
void Lb_Pl_act_set();
void lb_sw_set_sub(int);
void Lbs_MatchStart(int);
extern char lit_429_00664C38[];
void lb_send_data();
void lb_send_dataTU();
void Lb_send_data_to_myself();
s8 check_sender0();
typedef struct LBPKPOS { f32 x, z; u16 ang; u16 stg; } LBPKPOS;
typedef struct LBPKPOS2 { f32 x, z; u16 ang; u8 stg; u8 _pad; } LBPKPOS2;
typedef struct LBPKTRD { s16 item; s16 num; u8 id[8]; } LBPKTRD;
typedef struct LBLAST { f32 x; u8 _pad04[4]; f32 z; u16 ang; u8 stat; u8 x0F; u8 x10; u8 x11; u16 stg; } LBLAST; /* last sent packet (lastSend, 0x14 bytes) */
extern LBLAST lastSend;
extern u8 my_user_handle[0x10];
extern u8 D_3F3404[];
#endif
