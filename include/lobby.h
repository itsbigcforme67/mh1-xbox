/* Lobby overlay (online town) shared declarations. Add only; do not change existing lines. */
#ifndef LOBBY_H
#define LOBBY_H
#include "types.h"
#include "pl.h"
#include "game.h"

extern u8 *cw;                 /* client work (D_6DD7E0, 0xBF40 bytes): u8 accessed by offset */
#define CW8(o)  (*(u8 *)(cw + (o)))
#define CWPLAYER(i) (cw + (i) * 0x2FC)  /* per member records start at cw+0x1334 */

typedef struct LBCOMMER { s8 mac[6]; u8 _pad06[2]; char name[0x10]; u8 _pad18[0x5C - 0x18]; } LBCOMMER; /* 0x5C bytes x8 at lbCommer */
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
void Lb_put_hint();
void adx_se_set();
void Lb_put_set01();
int Lb_check_hotel();
void NPCZoomInCameraCancel();
#endif
