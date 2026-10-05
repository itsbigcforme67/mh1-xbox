/* Lobby overlay (online town) shared declarations. Add only; do not change existing lines. */
#ifndef LOBBY_F_H
#define LOBBY_F_H
#include "types.h"
#include "pl.h"
#include "game.h"

extern u8 *cw;                 /* client work (D_6DD7E0, 0xBF40 bytes): u8 accessed by offset */
#define CW8(o)  (*(u8 *)(cw + (o)))
#define CWPLAYER(i) (cw + (i) * 0x2FC)  /* per member records start at cw+0x1334 */

typedef struct LBCOMMER { s8 mac[6]; u8 _pad06[2]; char name[0x10]; u8 _pad18[0x5C - 0x18]; } LBCOMMER; /* 0x5C bytes x8 at lbCommer */
extern LBCOMMER lbCommer[8];

typedef struct LBPLAYER { PLW *pl; u8 x04[0x10]; u8 _pad14[0x38 - 0x14]; } LBPLAYER; /* lb_player 0x38 x8 */
extern LBPLAYER lb_player[8];

typedef struct LBSYS {         /* lb_sys 0x90 bytes */
    u8 _pad00[3];
    s8 x03;                    /* 0x03 mode (4 = send positions) */
    s8 x04;                    /* 0x04 phase counter of vs_square_* */
    s8 x05;                    /* 0x05 */
    s8 x06;                    /* 0x06 guild screen state */
    s8 x07;                    /* 0x07 guild/quest sub state */
    s8 x08;                    /* 0x08 rule sheet/quest sub state */
    u8 _pad09[0x64 - 9];
    u16 chair_mask;            /* 0x64 bit per occupied chair */
    u16 x66;                   /* 0x66 chair number (sent as a packet) */
    s32 x68;                   /* 0x68 */
    s32 x6C;                   /* 0x6C */
    u8 _pad70;
    s8 x71;                    /* 0x71 */
    s8 x72;                    /* 0x72 set01 message timer */
    u8 _pad73;
    s16 x74;                   /* 0x74 */
    u8 _pad76[2];
    s8 x78;                    /* 0x78 */
    u8 _pad79[3];
    s32 x7C;                   /* 0x7C */
    s32 x80;                   /* 0x80 */
    s8 x84;                    /* 0x84 */
    s8 x85;                    /* 0x85 send interval counter */
    u8 _pad86;
    u8 x87;                    /* 0x87 */
    u8 _pad88[5];
    u8 x8D;                    /* 0x8D */
    u8 x8E;                    /* 0x8E frame counter */
    u8 _pad8F;
} LBSYS;
extern u8 lbSendInterval;
extern LBSYS lb_sys;

typedef struct LBPOS { f32 x, z; u16 ang; u16 stg; } LBPOS;   /* received position packet (0xC bytes) */
typedef struct LBSTAT { f32 x, z; u16 ang; u8 chair; u8 act14; u8 act15; u8 x0D; u8 _pad0E[2]; } LBSTAT; /* received status packet (0x10 bytes) */
typedef struct LBTRADE2 { u16 item; s16 num; u8 plid[6]; u8 _pad0A[2]; u8 result; u8 _pad0D; } LBTRADE2;
typedef struct LBTRADE { u16 item; u16 num; u8 plid[6]; u8 _pad0A[2]; u8 result; u8 _pad0D; } LBTRADE; /* trade packet (0xE bytes) */
typedef struct LBS16x2 { s16 a, b; } LBS16x2;
typedef struct LBV3 { f32 x, y, z; } LBV3;
#define F(T, p, o) (*(T *)((u8 *)(p) + (o)))
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
typedef struct MHRULE { s8 x00; u8 _pad01[6]; s8 x07; char pass[9]; s8 x11; char msg[0x3D]; s8 x4F; u8 _pad50[4]; u32 quest; u8 x58; u8 _pad59[3]; u32 x5C; u8 _pad60[8]; } MHRULE; /* guild room rule 0x68 bytes */
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
void font_set_palette();
void font_print_uf();
void lb_put_room_member_005CB220();
void cnLBS_Get_ConditionSearchUser();
extern u8 *pNet;
extern u8 *SearchResult;
void Skill_set_PL();
void Lb_get_comment();
void Set_equip_idx();
void Set_userdata();
void Lb_set_mini_data();
void flSetRenderState();
void InitRenderState();
void Lbc_set_prim();
void Lb_put_help();
void lb_disp_name();
void flps0008();
void Lb_put_icon_free();
extern s16 lb_icon_tbl[];
extern char lit_254_00664B00[];
void Eft26_set();
short get_prim();
void *get_prim_ptr();
void hit_chk_init();
void Lb_trans_pl();
void armor_create_model();
void parts_init();
void pl_chr_set3();
void pl_create_model();
void yure_init();
void armor_model_free();
void flCompact();
void release_prim();
int pl_flag_ck();
void Put_2TF();
void SetFilterMode();
void SetTextureStage();
void flfntSetSize();
void font_print_ex();
void reload_tex();
extern char *lb_num_str[10];
extern char lit_688_00664CB8[];
extern char lit_695_00664CB8[];
extern char lit_688_00664CB0[];
extern u8 lit_693_0064E180[0x14];
int Cockpit_menu_chk();
int Cockpit_menu_chk_lobby();
void pad_timer_calc();
void cpRotMatrix();
void frame_init();
void frame_move();
s16 ran_suu();
extern s16 Plsw_buff[2][2];
extern u16 Plan_buff[2][2];
extern u16 Plan_ang[2][2];
extern u16 Plan_pow[2][2];
int Lb_act_ck();
void lb_pl_to_normal_clr();
void lb_pl_to_normal_clr2();
void lb_to_normal();
void lb_action_timer_calc();
int ck_pl_send();
void Lb_pl_to_normal();
void Lb_send_pl_status();
void lb_pl_flag_clr();
void Lb_pl_flag_set();
int Lb_ck_target();
void swset();
void lb_pl_move_sub();
void lb_pl_normal();
void lb_pl_chat();
void lb_pl_turn_sub();
void lb_pl_horm_sub();
void Lb_Pl_pos_adj();
void Lb_pl_chr_sub();
void HitWallPlayer();
int GetGroundHitStatusAreaPl();
int Pl_master_ck();
u16 calc_vec_ang2();
void Lb_St_unique_adr_set();
void Lb_send_pl_pos();
void lb_basic_com_ck();
void Lb_pl_chr_set();
extern u8 chat_act_tbl_0064E198[];
void ItemPickingDeclaration();
void pl_chr_set2();
void Lbc_set_prim();
extern LBCOMMER lbCommer[8];
void action_timer_calc();
extern s16 chat09_chr_tbl_0064E1C0[];
void Lb_eat_to_bell();
extern s32 lbs_command_jmp[];
extern u8 D_3C7357[];
void Gold_add(int);
void str_pause();
void str_volume();
void str_fadein_vol();
extern u8 D_32D471[];
void pl_flag_clr();
void flvecApplyMat33();
void Lb_put_hint();
void adx_se_set();
void Lb_put_set01();
int Lb_check_hotel();
void NPCZoomInCameraCancel();
void Eft06_set(f32, PLW *, int, int, int);
void Lb_eat_to_end();
extern s8 eatResult;
int frame_check2(f32, PLW *, int);
f32 flvecCalcDistance();
long Ud_item_num_ck();
long Ud_item_num_ck2();
long Ud_item_search_space();
void set01_set2();
extern u8 Item_data[327][16];
extern char lit_516_00664D50[];
void Lb_send_item_request();
void Eft06_set2(f32, PLW *, int, int, void *);
extern u8 lit_584_0064E1A8[12];
extern u8 sendDat[0x300];
int Lbs_CheckMatchingFlag();
void cnLBS_Send_ChatBinary();
void cnLBS_Send_ChatBinaryTU();
void CallBack_Result_SendChatBinaryTU();
void Lb_check_receipt();
extern char *lb_set01_msg[];
int Quest_clear_bit_ck();
int Event_flag_ck();
void lb_select_quest_level_trans();
void lb_questpage_trans();
void lb_rule_seet_trans();
void Plaza_chat_log_add();
void DispFrameMessage();
void PutButtonICON();
extern char lit_1026_00665D68[];
extern u8 client_work[0xBF40];
extern u8 network_work[0x2C];
void flMemset();
void init_set_work();
void Lbc_init_network_work();
extern char lit_275_00665668[];
extern char lit_743_00665D60[];
extern int *hint_tbl[2];
extern u8 em_work[];
extern u8 ot1[];
extern u8 lbShop[0x90];
int Lb_shop_sw();
void Lb_check_newCommer();
void Npc_se_req();
void ItemCopy_Ud2Pl();
void Info_control();
void Lb_pl_move();
void old_pos_save();
int enemy_mv();
void enemy_mk();
void em_ride_sub();
int Lb_npc_mv();
void Lb_npc_mk();
void player_mk();
void yure_move();
void Lb_check_target();
void lb_check_status();
void CameraMove();
void light_move();
void move_eft();
void move_set();
void move_stage();
void Pit_mv_lb();
void Lb_cockpit_move();
int Lb_check_pl_load();
int add_prim();
typedef struct BSCELL { u8 p[0x5C]; } BSCELL;   /* browser table-cell record (0x5C bytes): indexing an ARRAY of structs gives idx*size + base (addu order) */
typedef struct BSCELL1 { u8 p[1]; } BSCELL1;
#define BSC1(T, b, i, d) (*(T *)(((BSCELL1 *)((u8 *)(b) + (d)))[i].p))
typedef struct BSCELL2 { u8 p[2]; } BSCELL2;
typedef struct BSCELL4 { u8 p[4]; } BSCELL4;
typedef struct BSCELL8 { u8 p[8]; } BSCELL8;
#define BSC2(T, b, i, d) (*(T *)(((BSCELL2 *)((u8 *)(b) + (d) / 2 * 2))[i].p + (d) % 2))
#define BSC4(T, b, i, d) (*(T *)(((BSCELL4 *)((u8 *)(b) + (d) / 4 * 4))[i].p + (d) % 4))
#define BSC8(T, b, i, d) (*(T *)(((BSCELL8 *)((u8 *)(b) + (d) / 8 * 8))[i].p + (d) % 8))
#define BSC(T, b, i, d) (*(T *)(((BSCELL *)((u8 *)(b) + (d) / 0x5C * 0x5C))[i].p + (d) % 0x5C))
/* Browser system work (bsSysWork, 0x5C4 bytes; pointer bsSys). Fields named from the stock functions, finalAccount and UpdateEndpoint. */
typedef struct BSSYS {
    u8 x00, x01, x02, x03;   /* x01 = browser mode, x02 = sub state */
    u8 _pad04[0xC - 4];
    s32 x0C;               /* right edge reached by the page so far */
    s32 x10;               /* bottom edge */
    s32 x14;
    u16 x18;               /* page style flags */
    s16 x1A, x1C, x1E, x20;/* blank margins (setUpDnLtRtBlank) */
    s16 x22;               /* 0x20490 / x10 */
    s16 x24;
    s16 x26;               /* 0x4EB40 / x0C */
    s16 x28;
    u8 x2A, x2B, x2C, x2D, x2E, x2F, x30, x31, x32, x33, x34, x35, x36, x37, x38, x39, x3A;
    char meta[0x100];      /* 0x3B meta refresh url */
    u8 _pad13B[0x33B - 0x13B];
    char style[0x222];     /* 0x33B */
    char id1[0x22];        /* 0x55D */
    char id2[0x22];        /* 0x57F */
    char id3[0x22];        /* 0x5A1 */
    u8 x5C3;
} BSSYS;
/* Browser work object (BsWorkPull): one per on-screen element (background, scroll bars, title bar, cursor ...). */
typedef struct BSWK {
    s8 x00;                /* active */
    s8 x01;
    u8 x02;                /* kind of the stocked page object */
    u8 _pad03[2];
    u8 x05;
    s8 x06;                /* sprite state requested by the task */
    s8 x07;
    u8 _pad08[2];
    s16 x0A, x0C, x0E, x10;
    u8 _pad12[2];
    void *task;            /* 0x14 */
    void *trans;           /* 0x18 */
    u8 _pad1C[0x30 - 0x1C];
    u8 x30;
    u8 _pad31[3];
    f32 x34, x38;          /* position */
    u8 _pad3C[4];
    f32 x40, x44;          /* size */
    u8 _pad48[4];
    s32 x4C, x50;
    u8 _pad54[0x5D - 0x54];
    s8 x5D;
    u8 _pad5E;
    u8 x5F;
} BSWK;
/* Browser queue node (request / route / cache / source / image queues): singly linked, url at +4 */
typedef struct BSNODE {
    struct BSNODE *next;      /* 0x000 */
    char url[0x100];          /* 0x004 */
    s32 used;                 /* 0x104 state / in-use flag */
    u8 x108, x109;            /* 0x108 page counter, status flags */
    u8 _pad10A[2];
    s8 x10C;                  /* 0x10C request kind */
    s8 x10D;                  /* 0x10D 1 = html, 2 = image */
    u8 _pad10E[2];
    char *x110;               /* 0x110 */
    u32 tex;                  /* 0x114 texture | palette handle << 16 */
    s16 x118, x11A;
    u8 _pad11C[8];
    s8 x124, x125;
} BSNODE;
#endif
