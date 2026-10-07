/* lb_cli - one translation unit 0x005B7020-0x005BF808 (lbtu3). */
#define Lbs_MatchStart Lbs_MatchStart_hdr
#include "lobby_b.h"
#undef Lbs_MatchStart
typedef struct CNET_W5D4 { s32 w[0x175]; } CNET_W5D4;
extern char D_4E36F4[];
extern s32 netr_ret;
extern int lpSKey;
extern u8 BsLbsErrNum;
extern char network_lobby_client_main_jmp_207[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[5]; s8 x2C0E; u8 pad2C0F[0x22]; u8 x2C31; u8 pad2C32[0x12]; u8 x2C44; u8 pad2C45[0x9AF]; s32 x35F4; s32 x35F8; } CWS_ila;
#define CWX ((CWS_ila *)cw)
extern u8 COM_R_No_1;
extern int connect_jmp_tbl_270[];
extern int dial_jmp_tbl_540[2];
extern int dial_jmp_tbl_558[];
extern int lobby_client_login_jmp_324[];
extern int lbc_user_regist_jmp_939[];
extern int lobby_client_top_menu_jmp_1131[1];
extern int lobby_client_top_menu_jmp_1136[];
extern int lobby_client_matching_failed_jmp_2996[];
extern int lobby_client_logout_jmp_3078[];
extern int lbc_in_lobby_03_jmp_2430[];
extern char lb_prim[];
extern char D_3EBC90[];
extern char D_3EBCB0[];
extern char ot5[4];
extern char ot6[4];
extern char ot7[4];
extern char ot2[];
typedef struct { u8 pad0000[0x14]; int x0014; int x0018; int x001C; int x0020; } ARG_lbc_text_lobby_trans_arg0;
extern char CnetWork[];
extern char textLobbyTbl[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x1]; u8 x2C33; } CWS_text_lobby_trans_ot0;
typedef struct { u8 pad0000[0x18]; s32 x0018; } ARG_text_lobby_trans_ot0_arg0;
extern u8 * pNet_c7;
extern u8 dod_new_reguration_agree_type;
extern char first_url[];
extern char lit_452_0065E820[];
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0xF]; u8 x2C44; } CWS_reg;
#define CWX_c8 ((CWS_reg *)cw)
typedef struct { u8 pad0[5]; u8 x05; u8 pad06[0x26]; } CNW5;
extern CNW5 CnetWork_c9;
extern u16 Get_sw2();
extern char lit_547_0065E850[];
typedef struct { u8 pad0[0x2C34]; u8 step; u8 pad2C35[0x17]; s32 x2C4C; s32 x2C50; u8 pad2C54[0x9AA]; u8 x35FE; u8 pad35FF; u16 x3600; u8 x3602[4]; } CWS_lw;
#define CWX_c10 ((CWS_lw *)cw)
void cnWrap_FontDisp(f32, f32, f32, char *);
extern char jtbl_575_0065E890[];
extern char D_3C6FC8[];
typedef struct { u8 pad00[0x5]; u8 x05; u8 padEND[0x2A]; } CNW;
extern CNW CnetWork_c12;
void cnetGet_Login_UserID(u8, u8 *);
void cnetGet_Login_UserHandle(u8, u8 *);
void cnetGet_Login_UserMiniData(u8, u8 *);
typedef struct { u8 b[0x1D0]; } PD;
extern PD BrPersonalData;
extern PD tmpPersonalData;
extern char lit_707_0065E8D0[];
extern char CallBack_Result_LoginPersonalDataRegist_o[];
typedef struct { u8 pad0[0x2C33]; u8 x2C33; u8 step; u8 pad2C35[0x9]; u8 x2C3E[6]; u8 x2C44; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; u8 pad2C50[0x681]; u8 x32D1[1]; } CWS_pd;
#define CWX_c14 ((CWS_pd *)cw)
extern char text_lobby_trans_ot0_o[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; u8 x2C34; u8 pad2C35[0x17]; s32 x2C4C; } CWS_le;
#define CWX_c16 ((CWS_le *)cw)
extern u8 BsLbsCount;
extern char D_3E5326[];
extern char CallBack_SendMiniData_o[];
typedef struct { u8 pad0[2]; u8 x02; u8 pad03[0x12]; u8 x15; u8 pad16[0x2A]; } MINI40;
#define MYMINI ((MINI40 *)&my_user_mini_data)
extern CNW5 CnetWork_c17;
typedef struct { u8 pad0000[0x2C33]; s8 x2C33; u8 x2C34; u8 pad2C35[0x10]; s8 x2C45; u8 pad2C46[0x98C]; u8 x35D2; u8 pad35D3[3]; s8 x35D6; s8 x35D7; } CWS_lbc_login_finish;
#define CWX_c17 ((CWS_lbc_login_finish *)cw)
extern CNW5 CnetWork_c18;
extern char CallBack_Result_LoginTopInformation_o[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; u8 step; u8 pad2C35[0x17]; s32 x2C4C; s32 x2C50; u8 pad2C54[0x19AE]; u8 x4602; u8 pad4603[3]; u8 x4606[4]; } CWS_lt;
#define CWX_c19 ((CWS_lt *)cw)
extern CNW5 CnetWork_c21;
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0x17]; s32 x2C4C; u8 pad2C50[0x982]; s8 x35D2; } CWS_lfa;
#define CWX_c21 ((CWS_lfa *)cw)
extern s8 BsLbsErrNum_c23;
extern s8 net_char_change;
extern s8 BS_MODE_R_NO;
extern char FirstURL[];
typedef struct BRPD { unsigned __int128 q[29]; } BRPD;
extern BRPD BrPersonalData_c24;
extern BRPD tmpPersonalData_c24;
typedef struct { u8 pad2C08[0x2C08]; s8 x2C08; u8 pad2C09[0x2C43 - 0x2C09]; u8 x2C43; } CWS_b3;
extern u8 sendDat_c24[];
extern u8 D_3C73B4[];
void CallBack_Result_netComment();
void CallBack_Result_netCommentSend();
int cnLBS_Get_RoomMemberList();
extern u8 * pNet_c25;
extern char tmpPersonalData_c25[];
extern char BrPersonalData_c25[];
typedef struct { u8 pad0000[0x2C43]; u8 x2C43; } CWS_lbc_browser_04;
extern s8 BsLbsErrNum_c26;
extern char BrPersonalData_c27[];
extern char CallBack_Result_LoginPersonalDataRegist2_o[];
extern char CallBack_ReadCurrentPlace_o[];
extern char CallBack_Result_Plaza_ReadAllocation2_o[];
extern char CallBack_Result_Plaza_ReadLobbyAllocation2_o[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x29]; u8 x2C32; u8 pad2C33[0x12]; s8 x2C45; u8 pad2C46[0x98F]; s8 x35D5; } CWS_rl2;
#define CWX_c30 ((CWS_rl2 *)cw)
int cnLBS_Get_LobbyStatus(u16 id, void *p);
int cnLBS_Get_mhLobbyJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_LobbyName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1[5]; u16 x6; u8 pad8[0x18]; } CLSI;
extern CLSI ClassInfo;
typedef struct { s16 x0; u8 pad2[0x15A]; } PLZ;
extern PLZ LobbyInfo[];
typedef struct { u8 x0; u8 pad1[3]; u8 x4; u8 pad5[0x1B]; } CLSI_c32;
extern CLSI_c32 ClassInfo_c32;
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 x2C32; u8 pad2C33[0x12]; u8 x2C45; u8 pad2C46[0x98C]; s8 x35D2; } CWS_cp;
#define CWX_c32 ((CWS_cp *)cw)
extern char CallBack_Result_SendUserMiniData_o[];
extern char ClassInfo_c36[];
extern char PlazaInfo[];
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI_c37;
extern CLSI_c37 ClassInfo_c37;
typedef struct { u8 pad0[0x10]; u8 st; u8 pad11[0x14B]; } PLI;
extern PLI PlazaInfo_c37[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x2A]; s8 x2C33; s8 x2C34; } CWS_tm1;
#define CWX_c37 ((CWS_tm1 *)cw)
extern char CallBack_Result_Plaza_PlazaEntry_o[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_CallBack_Result_Plaza_PlazaEntry;
extern char CallBack_Result_Plaza_ReadAllocation_o[];
int cnLBS_Get_PlazaStatus(u16 id, void *p);
int cnLBS_Get_mhPlazaJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_PlazaName(u16 id, void *p);
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI_c43;
extern CLSI_c43 ClassInfo_c43;
extern PLZ PlazaInfo_c43[];
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[1]; s8 x2C33; s8 x2C34; u8 pad2C35[0x10]; u8 x2C45; } CWS_ra;
#define CWX_c43 ((CWS_ra *)cw)
extern u8 * pNet_c44;
extern char ClassInfo_c44[];
extern char lbc_in_plaza_jmp_1345[];
extern char ClassInfo_c46[];
extern char LobbyInfo_c46[];
extern char put_back[];
typedef struct { s8 x0; u8 pad1; u16 x2; u8 pad4[0x1C]; } CLSI_c47;
extern CLSI_c47 ClassInfo_c47;
extern PLZ PlazaInfo_c47[];
extern char CallBack_Result_Plaza_ReadLobbyAllocation_o[];
extern char D_3EBCD0[];
extern char text_lobby_trans_ot3_o[];
typedef struct FRIENDENT { f32 f[12]; } FRIENDENT;      /* friend list entry (0x30 bytes), copied word-wise through the FPU */
typedef struct { u8 pad0000[0x2C08]; s8 x2C08; u8 pad2C09[0x2C34 - 0x2C09]; u8 x2C34; } CWS_ip1;
int Lbs_plaza();
extern char CallBack_Result_Plaza_LobbyEntry_o[];
extern char CallBack_Result_Plaza_LobbyMember_o[];
/* CallBack_Result_Plaza_LobbyMember (0x5BA940): logic complete; 7/173 differ (v0/v1 naming of the cw+off pointer vs the counter at the Lb_set_player call). Not built. */
typedef struct { char id[8]; char name[0x10]; u8 pad18[4]; char mini[0x40]; } LUSER;
int cnLBS_Read_LobbyMemberList(u16 id, void *cb);
extern char CallBack_Result_Plaza_LobbyMember2_o[];
typedef struct { u8 pad0[0x2C45]; s8 x2C45; } CWS_gl;
#define CWX_c56 ((CWS_gl *)cw)
extern char tl_member_buff[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_LobbyMember2;
extern char CallBack_Result_Plaza_PlazaExit_o[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_CallBack_Result_Plaza_PlazaExit;
typedef struct { u8 pad0[0x2C33]; s8 x2C33; s8 x2C34; u8 pad2C35[7]; s8 x2C3C; u8 pad2C3D[0xF]; s32 x2C4C; } CWS_p4;
#define CWX_c60 ((CWS_p4 *)cw)
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[2]; u8 x2C34; u8 pad2C35[0x10]; u8 x2C45; } CWS_rl;
#define CWX_c61 ((CWS_rl *)cw)
extern char CallBack_Result_SearchUserPlace_o[];
extern char lbc_in_lobby_jmp_1826[];
extern char lbc_in_lobby_00_jmp_1842[];
extern char RoomInfo[];
extern char ClassInfo_c67[];
extern char CallBack_Result_Lobby_JoinUser_o[];
typedef struct { u8 pad[0x15C]; } LINFO;
extern LINFO LobbyInfo_c68[];
extern char D_3A14C6[];
extern s32 mission_area_c70;
extern char CallBack_Result_ReadFileDownload_o[];
extern char CallBack_Result_Lobby_ReadRoomAllocation_o[];
int cnLBS_Get_RoomStatus(u16 id, void *p);
int cnLBS_Get_mhRoomJoinUser(u16 id, void *a, void *b);
int cnLBS_Get_RoomJoinInfo(u16 id, void *a, void *b, void *c, void *d, void *e);
int cnLBS_Get_RoomPasswordInfo(u16 id, void *p);
int cnLBS_Get_RoomProperty(u16 id, void *p);
int cnLBS_Get_RoomExplain(u16 id, void *p);
typedef struct { s8 x0; u8 pad1[9]; u16 xA; u8 padC[0x14]; } CLSI_c73;
extern CLSI_c73 ClassInfo_c73;
extern PLZ RoomInfo_c73[];
typedef struct { u8 pad0[0x2C31]; u8 x2C31; u8 pad2C32[3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_rr2;
#define CWX_c73 ((CWS_rr2 *)cw)
extern char ClassInfo_c74[];
typedef struct { u16 x0; u8 pad2[0xE]; u8 x10; u8 pad11[0x14B]; } RINFO;
extern RINFO RoomInfo_c76[];
extern char ClassInfo_c76[];
typedef struct { u8 pad0[0x2C33]; s8 x2C33; s8 x2C34; s8 x2C35; s8 x2C36; u8 pad2C37[0x15]; s32 x2C4C; u8 pad2C50[0x673]; s8 x32C3; s8 x32C4; u8 pad32C5[0xD]; s8 x32D1_; } CWS_l005;
#define CWX_c76 ((CWS_l005 *)cw)
extern char CallBack_GetDate_o[];
extern char ClassInfo_c79[];
extern char CallBack_Result_Lobby_RoomCreate_o[];
typedef struct { u16 x0; u8 pad2[0xE]; u8 st; u8 pad11[0x14B]; } RINF;
extern RINF RoomInfo_c79[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; } CWS_rr;
#define CWX_c79 ((CWS_rr *)cw)
typedef struct { u8 pad0[0x54]; u8 n; } RRH;
extern RRH RoomRule;
extern char CallBack_Result_Lobby_SetRoomRule_o[];
typedef struct { u8 pad0[0x2C35]; u8 step; } CWS_sr;
#define CWX_c82 ((CWS_sr *)cw)
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_SetRoomRule;
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
void Paint_square();
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help(int a, int b, int c, u16 d);
extern u8 RoomInfo_c85[];
extern u8 join_member[];
extern char CallBack_Result_Lobby_GuestRoomMember_o[];
extern char CallBack_Result_Lobby_GuestRuleAllocation_o[];
typedef struct { u8 pad0[0x2C35]; u8 step; } CWS_gr;
#define CWX_c85 ((CWS_gr *)cw)
int cnLBS_Get_RoomRuleAllocation(u16, u8 *);
int cnLBS_Read_RoomMemberList(u16, void *);
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_GuestRoomMember;
extern char ClassInfo_c88[];
extern char RoomRule_c88[];
extern char CallBack_Result_Lobby_RoomEntry_o[];
typedef struct { u8 pad0[0x2C31]; s8 x2C31; s8 x2C32; s8 x2C33; s8 x2C34; u8 x2C35; u8 pad2C36[0x16]; s32 x2C4C; u8 pad2C50[0x66F]; u8 x32BF; } CWS_l300;
#define CWX_c92 ((CWS_l300 *)cw)
extern char jtbl_2502[];
extern char CallBack_Result_Lobby_LobbyExit_o[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; u8 pad2C50[0x985]; s8 x35D5; } CWS_l301;
#define CWX_c93 ((CWS_l301 *)cw)
typedef struct { u8 pad0000[0x2C35]; u8 x2C35; } CWS_Lbs_LobbyExit;
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_LobbyExit;
extern char CallBack_Result_Lobby_RoomExit_o[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; } CWS_l302;
#define CWX_c99 ((CWS_l302 *)cw)
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_RoomExit;
int cnLBS_Read_RoomJoinUser(u16 id, void *cb);
int cnLBS_Read_MatchEntryJoinUser(u16 id, void *cb);
extern char ClassInfo_c103[];
extern char CallBack_Result_InRoom00_Member_o[];
extern char CallBack_Result_InRoom00_JoinUser_o[];
extern char CallBack_Result_InRoom00_EntryJoinUser_o[];
extern BRPD BrPersonalData_c104;
extern BRPD tmpPersonalData_c104;
extern u8 sendDat_c104[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; } CWS_im;
extern LINFO RoomInfo_c105[];
extern char RoomRule_c105[];
extern u8 ClassInfo_c105[];
extern char ClassInfo_c106[];
extern char CallBack_Result_InRoom_MatchEntryCancel_o[];
extern char CallBack_Result_InRoom_MatchEntry_o[];
#pragma readonly_strings on
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
extern char lobby_client_game_ready_jmp_2838[];
extern u8 USER_PL_ID;
extern char room_member_id[];
extern char room_member_handle[];
extern char room_member_mini_data[];
extern char CallBack_Result_Match_MatchInformation_o[];
typedef struct { u8 pad0[0x2C33]; u8 x2C33; u8 x2C34; u8 pad2C35[0x10]; s8 x2C45; u8 pad2C46; u8 x2C47; } CWS_gr_c115;
#define CWX_c115 ((CWS_gr_c115 *)cw)
extern s8 COM_R_No_0;
extern s8 COM_R_No_1_c120;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern s8 COM_R_No_5;
extern s8 COM_R_No_6;
extern s8 net_game_invalid_flag;
extern u16 System_timer;
extern char CallBack_Result_Match_Logout_o[];
extern char jtbl_2977[];
typedef struct { u8 pad[0x2C31]; u8 x31; u8 x32; u8 x33; u8 x34; u8 x35; u8 pad2[0x2C45-0x2C36]; u8 x45; } CWS;
#define CWX_c121 ((CWS *)cw)
extern u8 * pNet_c126;
extern u8 * pNet_c127;
extern u8 net_char_change_c127;
extern s8 COM_R_No_1_c132;
extern s8 MMBB_LOGIN;
extern char jtbl_3160[];
extern char D_3E4C05[];
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0x11]; u8 x2C46; } CWS_lo0;
#define CWX_c132 ((CWS_lo0 *)cw)
extern s8 COM_R_No_1_c133;
extern u8 net_char_change_c133;
extern char jtbl_3219[];
typedef struct { u8 pad0[0x2C08]; s8 x2C08; u8 pad2C09[0x29]; u8 x2C32; u8 x2C33; u8 x2C34; u8 pad2C35[0x11]; u8 x2C46; u8 pad2C47[5]; s32 x2C4C; } CWS_lo1;
#define CWX_c133 ((CWS_lo1 *)cw)
extern char CallBack_Result_GotoTop_o[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x13]; u8 x2C45; } CWS_CallBack_Result_GotoTop;
extern char lit_3323[];
void cnWrap_SetFontSize(f32);
typedef struct { u8 pad0[0x2F6E]; u8 x2F6E; u8 x2F6F; u8 pad2F70[4]; s16 x2F74; s8 x2F76; } CWS_am1;
#define CWX_c138 ((CWS_am1 *)cw)
extern s8 COM_R_No_Disconnect;
extern s8 COM_R_No_Logout;
extern u8 COM_R_No_Logout_c142;
extern u8 COM_R_No_Logout_t;
s32 internet_connect_minimum_cleanup();
static void CallBackWaitInit();
s32 Check_CallBackWait();
s32 internet_lobby_act();
void Lbc_connect();
void lobby_client_login();
void Lbc_set_prim(s32 arg0, s32 arg1, s32 arg2);
void lbc_text_lobby_trans(ARG_lbc_text_lobby_trans_arg0 *arg0);
void text_lobby_trans_ot0(ARG_text_lobby_trans_ot0_arg0 *arg0);
void text_lobby_trans_ot3(u8 *arg0);
void lbc_login_reguration();
s32 check_warning_level(s32 arg0);
void lbc_login_warning_message();
void lbc_login_patch();
void lbc_login_id_select();
void CallBack_SendMiniData(CNET_RES res);
void lbc_login_users_personal_data();
void CallBack_Result_LoginPersonalDataRegist(CNET_RES res);
void lbc_login_error();
void lbc_login_finish();
s32 check_top_information_level(s32 arg0);
void lbc_login_top_information();
void CallBack_Result_LoginTopInformation(CNET_RES res);
void lbc_login_finish_after();
void lbc_browser();
void To_BootUpBrowser();
void lbc_browser_00();
void lbc_browser_01();
void lbc_browser_02();
void lbc_browser_03();
void lbc_browser_04();
void lbc_browser_05();
s32 Lbc_SendBrowserResult();
void CallBack_Result_LoginPersonalDataRegist2(CNET_RES res);
void To_MyLobby();
void lobby_return_to_lobby();
void CallBack_Result_Plaza_ReadLobbyAllocation2(CNET_RES res);
void CallBack_ReadCurrentPlace(CNET_RES res);
void MH_lobbyClear();
void lobby_client_top_menu();
void lbc_top_menu();
void To_TopMenu();
void lbc_top_menu_00();
void lbc_top_menu_01();
void lbc_top_menu_02();
void CallBack_Result_Plaza_PlazaEntry(CNET_RES res);
void CallBack_Result_Plaza_PlazaExit2(CNET_RES res);
void CallBack_Result_Plaza_PlazaEntry2(CNET_RES res);
void lbc_top_menu_03();
void lbc_top_menu_04();
void lbc_top_menu_05();
void CallBack_Result_Plaza_ReadAllocation(CNET_RES res);
void lbc_top_menu_06();
void lobby_client_game_in_plaza();
void To_EnterPlaza();
void To_EnterPlaza2Lobby();
void CallBack_Result_Plaza_ReadAllocation2(CNET_RES res);
void lbc_in_plaza_00();
void Lbc_init_network_work();
void lbc_in_plaza_01();
void lbc_in_plaza_02();
s32 Lbs_request_enter_lobby();
s32 Lbs_request_enter_lobby2();
void CallBack_Result_Plaza_LobbyEntry(CNET_RES res);
void CallBack_Result_Plaza_LobbyMember(CNET_RES res);
s32 Lbs_GetLobbyMemberList();
void CallBack_Result_Plaza_LobbyMember2(CNET_RES res);
void lbc_in_plaza_03();
void CallBack_Result_Plaza_PlazaExit(CNET_RES res);
void lbc_in_plaza_04();
void CallBack_Result_Plaza_ReadLobbyAllocation(CNET_RES res);
void CallBack_Result_ConditionSearchUser(CNET_RES res);
s32 Lbs_SeekId();
void CallBack_Result_SearchUserPlace(CNET_RES res);
void To_PlazaExit(s32 arg0);
void lobby_client_game_in_lobby();
void lbc_in_lobby_00();
void To_EnterLobby();
void CallBack_Result_Lobby_JoinUser(CNET_RES res);
void lbc_in_lobby_00_00();
s32 Lbc_DownloadQuest();
void CallBack_Result_ReadFileDownload(CNET_RES res);
s32 Lbc_ReadRoomInfo();
void CallBack_Result_Lobby_ReadRoomAllocation(CNET_RES res);
u16 Lbs_GetClassAdd();
int Lbs_GetRoomInfo(int arg0);
void lbc_in_lobby_00_05();
s32 Lbc_getDate();
void CallBack_GetDate(CNET_RES res);
s32 Lbc_ReserveRoom();
void CallBack_Result_Lobby_RoomCreate(CNET_RES res);
void CallBack_Result_RuleAllocation(CNET_RES res);
s32 Lbc_SetRoomRule();
void CallBack_Result_Lobby_SetRoomRule();
char * GetRoomRule();
s32 Lbc_GuestReadRoom(s32 arg0);
void CallBack_Result_Lobby_GuestRuleAllocation(CNET_RES res);
void CallBack_Result_Lobby_GuestRoomMember();
s32 Lbs_GuestEnterRoom();
void CallBack_Result_Lobby_RoomEntry(CNET_RES res);
void lbc_in_lobby_03();
void To_LobbyExit(s32 arg0);
void lbc_in_lobby_03_00();
void lbc_in_lobby_03_01();
s32 Lbc_SendMiniData();
void CallBack_Result_SendUserMiniData();
void CallBack_NoticeUserMiniData(CNET_RES res);
s32 Lbs_LobbyExit();
void CallBack_Result_Lobby_LobbyExit(CNET_RES res);
void lbc_in_lobby_03_02();
s32 Lbs_InRoomCheck();
s32 Lbs_RoomExit();
void CallBack_Result_Lobby_RoomExit(CNET_RES res);
void To_EnterRoom();
void CallBack_Result_InRoom00_Member(CNET_RES res);
void CallBack_Result_InRoom00_JoinUser(CNET_RES res);
void CallBack_Result_InRoom00_EntryJoinUser(CNET_RES res);
s32 Lbs_MatchEntryCancel();
void CallBack_Result_InRoom_MatchEntryCancel(CNET_RES res);
s32 Lbs_MatchEntry();
void CallBack_Result_InRoom_MatchEntry(CNET_RES res);
void CallBack_Result_SetRoomPropaty(CNET_RES res);
int Lbs_MatchStart();
void lobby_client_game_ready();
void To_ReadyBattle();
void lbc_game_ready_00();
void CallBack_Result_Match_MatchInformation(CNET_RES res);
void lbc_game_ready_01();
void lbc_game_ready_02(int arg0, int arg1, s32 arg2);
void lbc_game_ready_03();
void lbc_game_ready_04();
void CallBack_Result_Match_Logout();
void lobby_client_matching_failed();
void To_MatchingFailed();
void lbc_matching_failed_00();
void lbc_matching_failed_01();
void lbc_matching_failed_02();
void lbc_matching_failed_03();
void lbc_matching_failed_04();
void lbc_matching_failed_05();
void To_LogOut();
void Lbs_LogOutRequest();
void Set_ErrorDialog(int arg0);
void lobby_client_logout();
void lbc_logout_00();
void lbc_logout_01();
void To_GoToTop();
void lobby_client_goto_top();
void CallBack_Result_GotoTop(CNET_RES res);
void lbc_admin_message_00();
void lbc_admin_message_01();
void lbc_admin_message_02();
void Init_InterruptFlag();
s32 Lbs_CheckMatchingFlag();
s32 Check_InterruptFlag();
void tk_logout_init();
void CallBack_Logout_ShutDown();
int Lbc_set_prim_k();
int Set_ErrorDialog_k();
int To_LobbyExit_k();
int To_PlazaExit_k();
int check_top_information_level_k();
int check_warning_level_k();
int lbc_text_lobby_trans_k();
s32 internet_connect_minimum_cleanup() {
    CpInetTcpAbort(*(s32 *)0x4E36F4);
    CpInetTcpDelete(&D_4E36F4);
    return 2;
}

static void CallBackWaitInit() {
    F(s32, (u8 *)cw, 0x35DC) = 0xE10;
    F(s8, (u8 *)cw, 0x35D9) = 0;
}

s32 Check_CallBackWait() {
    void *temp_v1;
    void *temp_v1_2;

    temp_v1 = (u8 *)cw;
    F(s32, temp_v1, 0x35DC) = (F(s32, temp_v1, 0x35DC) - 1);
    temp_v1_2 = (u8 *)cw;
    if (F(s32, temp_v1_2, 0x35DC) < 0) {
        F(s8, temp_v1_2, 0x35D9) = 1;
        To_LogOut(4);
        return 1;
    }
    return 0;
}

s32 internet_lobby_act() {
    s32 i;
    s32 sw;
    s8 t;
    s32 v;

    sw = Get_sw(0) & 0xFFFF;
    i = 0;
    if (*(s8 *)0x3F36CC == 0) {
        do {
            if (cnLBS_RecvData(*(s32 *)0x4E36F4) != 1) {
                break;
            }
            i++;
            CWX->x35F4 = 0xE10;
        } while (i < 0x64);
    }
    CWX->x35F4--;
    if (CWX->x2C31 != 5) {
        if (cnWrap_IsAccessMemoryCard() == 0 && CWX->x2C08 != 0) {
            if (sw != 0 || F(u8, lpSKey, 0x658) != 0 || (*(u16 *)0x3F3718 & 0x3C3C) != 0) {
                cnLbc_SetIspRestTime((u8 *)cw + 0x35F8);
            }
            v = CWX->x35F8 - 1;
            CWX->x35F8 = v;
            if (v < 0) {
                To_LogOut(3);
                return 1;
            }
            if (CpInetGetStatus() != 0 || BsLbsErrNum != 0) {
                To_LogOut(6);
                return 1;
            }
            if (CWX->x35F4 < 0) {
                To_LogOut(6);
                return 1;
            }
            t = CWX->x2C0E;
            if (t == 1) {
                To_LogOut(5);
                return 1;
            }
            if (t == 2) {
                To_LogOut(7);
                return 1;
            }
        } else if (CWX->x2C44 == 1 && BsLbsErrNum == 0) {
            if (sw != 0 || F(u8, lpSKey, 0x658) != 0) {
                cnLbc_SetIspRestTime((u8 *)cw + 0x35F8);
            }
            if (CWX->x35F4 < 0) {
                BsLbsErrNum = 1;
            } else {
                v = CWX->x35F8 - 1;
                CWX->x35F8 = v;
                if (v < 0) {
                    BsLbsErrNum = 1;
                } else {
                    t = CWX->x2C0E;
                    if (t == 1 || t == 2) {
                        BsLbsErrNum = 1;
                    } else if (CpInetGetStatus() != 0) {
                        BsLbsErrNum = 1;
                    }
                }
            }
        }
    }
    if (lobby_client_admin_message() != 0) {
        if (F(u8, pNet, 0x11) == 0) {
            lbc_text_lobby_trans_k(&network_work);
        }
        return netr_ret;
    }
    F(s8, pNet, 0xC) = 0;
    F(u8, pNet, 0x11) = 0;
    font_set_stack_no(0);
    ((int (**)())network_lobby_client_main_jmp_207)[CWX->x2C31]();
    if (F(u8, pNet, 0x11) == 0) {
        lbc_text_lobby_trans_k(&network_work);
    }
    return netr_ret;
}

void Lbc_connect() {
    s32 var_s0;

    if (Online_ck() == 1) {
        var_s0 = 0;
loop_2:
        if (cnLBS_RecvData(*(s32 *)0x4E36F4) == 1) {
            var_s0 += 1;
            F(s32, (u8 *)cw, 0x35F4) = 0xE10;
            if (var_s0 < 0x64) {
                goto loop_2;
            }
        }
    }
}

void lobby_client_login() {
    ((int (**)())lobby_client_login_jmp_324)[F(u8, (u8 *)cw, 0x2C33)]();
}

void Lbc_set_prim(s32 arg0, s32 arg1, s32 arg2) {
    F(int, pNet, 0x14) = (int)&lb_prim;
    F(s32, F(int, pNet, 0x14), 0x14) = arg0;
    F(int, pNet, 0x18) = (int)&D_3EBC90;
    F(s32, F(int, pNet, 0x18), 0x14) = arg1;
    F(int, pNet, 0x1C) = (int)&D_3EBCB0;
    F(s32, F(int, pNet, 0x1C), 0x14) = arg2;
}

#ifdef __MWERKS__
asm int lbc_login_init()
{
#include "lbc_login_init.inc"
}
#endif

void lbc_text_lobby_trans(ARG_lbc_text_lobby_trans_arg0 *arg0) {
    int temp_a1;
    int temp_a1_2;
    int temp_a1_3;
    int temp_a1_4;

    temp_a1 = arg0->x0014;
    if ((temp_a1 != 0) && (F(s32, temp_a1, 0x14) != 0)) {
        add_prim2(&ot5, temp_a1, 0, 1);
    }
    temp_a1_2 = arg0->x0018;
    if ((temp_a1_2 != 0) && (F(s32, temp_a1_2, 0x14) != 0)) {
        add_prim2(&ot6, temp_a1_2, 0, 1);
    }
    temp_a1_3 = arg0->x001C;
    if ((temp_a1_3 != 0) && (F(s32, temp_a1_3, 0x14) != 0)) {
        add_prim2(&ot7, temp_a1_3, 0, 1);
    }
    temp_a1_4 = arg0->x0020;
    if ((temp_a1_4 != 0) && (F(s32, temp_a1_4, 0x14) != 0)) {
        add_prim2(&ot2, temp_a1_4, 0, 0x10);
    }
}

void text_lobby_trans_ot0(ARG_text_lobby_trans_ot0_arg0 *arg0) {
    u8 temp_a0;
    u8 temp_v1;

    font_set_stack_no(arg0->x0018);
    if (Online_ck() == 1) {
        temp_v1 = ((CWS_text_lobby_trans_ot0 *)cw)->x2C31;
        if ((temp_v1 == 0) && (((CWS_text_lobby_trans_ot0 *)cw)->x2C33 < 7) && (F(u8, &CnetWork, 5) == 0)) {
            reload_tex(1, 0x14D);
            SetTextureStage(0x14D);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            Put_2TF((u8 *)&textLobbyTbl + 0x28);
        } else if (((u32) (temp_v1 - 5) <= 1U) || (temp_v1 == 3)) {
            Disp_back();
        } else {
            reload_tex(1, 0x154);
            SetTextureStage(0x154);
            SetFilterMode(1);
            flSetRenderState(0x60, 0);
            Put_2TF(&textLobbyTbl);
        }
    } else {
        reload_tex(1, 0x119);
        SetTextureStage(0x119);
        Put_2TF((u8 *)&textLobbyTbl + 0x28);
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    if (((CWS_text_lobby_trans_ot0 *)cw)->x2C31 != 5) {
        temp_a0 = ((CWS_text_lobby_trans_ot0 *)cw)->x2C33;
        if ((temp_a0 == 4) && (temp_a0 == 3)) {
            DispSceneTitle(temp_a0, ((CWS_text_lobby_trans_ot0 *)cw));
            DispHelpLine();
        }
    }
}

void text_lobby_trans_ot3(u8 *arg0) {
    u8 temp_a0;

    font_set_stack_no(F(s32, arg0, 0x18));
    temp_a0 = F(u8, pNet_c7, 0xC);
    if (temp_a0 == 1) {
        DispDialogData(temp_a0);
        Lb_on_dialog();
    }
}

void lbc_login_reguration() {
    switch (CWX_c8->x2C34) {
    case 0:
        CWX_c8->x2C34++;
        fade_set(2);
        strcpy(&first_url, &lit_452_0065E820);
        To_BootUpBrowser();
        return;
    case 1:
        if (CWX_c8->x2C44 == 2) {
            CWX_c8->x2C34++;
            CWX_c8->x2C44 = 0;
            fade_set(1);
            return;
        }
        lbc_browser(2);
        return;
    case 2:
        if (dod_new_reguration_agree_type == 0) {
            To_LogOut(1);
            return;
        }
        CWX_c8->x2C34++;
        CallBackWaitInit();
        cnLBS_Send_RegurationAgree(0);
        return;
    case 3:
        Check_CallBackWait();
    }
}

s32 check_warning_level(s32 arg0) {
    switch (arg0 & 0xFF) {
    case 0:
        return 1;
    case 1:
        if (CnetWork_c9.x05 == 0) {
            return 1;
        }
        break;
    case 2:
        if (CnetWork_c9.x05 == 0) {
            return 1;
        }
        if (CnetWork_c9.x05 == 3) {
            return 1;
        }
        break;
    case 3:
        if (CnetWork_c9.x05 == 0) {
            return 1;
        }
        if (CnetWork_c9.x05 == 3) {
            return 1;
        }
        if (CnetWork_c9.x05 == 2) {
            return 1;
        }
        break;
    case 4:
        if (CnetWork_c9.x05 == 0) {
            return 1;
        }
        if (CnetWork_c9.x05 == 3) {
            return 1;
        }
        if (CnetWork_c9.x05 == 2) {
            return 1;
        }
        if (CnetWork_c9.x05 == 1) {
            return 1;
        }
        break;
    }
    return 0;
}

void lbc_login_warning_message() {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_lw *c;

    sw = Get_sw2(0);
    c = CWX_c10;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        if (check_warning_level_k(c->x35FE) == 0) {
            CWX_c10->step = 7;
            CallBackWaitInit();
            cnLBS_Answer_LoginWarningMessage(0);
            return;
        }
        CWX_c10->step++;
        return;
    case 1:
        *stp = st + 1;
    case 2:
        CWX_c10->step++;
        CWX_c10->x2C4C = 0x3C;
        return;
    case 3:
        *stp = st + 1;
        if (CWX_c10->x35FE != 0) {
            CWX_c10->x2C50 = 8;
            SetDialogData_HTML(CWX_c10->x3602);
            return;
        }
        To_LogOut(1);
        return;
    case 4:
        F(s8, pNet, 0xC) = 1;
        c = CWX_c10;
        if (c->x3600 != 0) {
            c->x2C4C--;
            if ((s8) CWX_c10->x2C4C > 0) {
                return;
            }
            CWX_c10->x2C4C = 0x3C;
            CWX_c10->x3600--;
            if ((s16) CWX_c10->x3600 >= 0) {
                return;
            }
            CWX_c10->x3600 = 0;
            return;
        }
        if (sw & 0xFFFF & 0x20) {
            c->step++;
            cnWrap_SoundRequest(0);
        }
        cnWrap_SetFontColor(5);
        flfntSetSize(0x14, 0x14);
        cnWrap_FontDisp(210.0f, 302.0f, 2.0f, lit_547_0065E850);
        return;
    case 5:
        *stp = st + 1;
        return;
    case 6:
        *stp = st + 1;
        CallBackWaitInit();
        cnLBS_Answer_LoginWarningMessage(1);
        return;
    case 7:
        Check_CallBackWait();
        break;
    }
}

void lbc_login_patch() {
    s32 temp_v0;
    u8 temp_a1;
    int temp_a2;
    int temp_a3;
    int temp_v1;

    temp_a2 = (int)cw;
    temp_a1 = F(u8, temp_a2, 0x2C34);
    temp_a3 = temp_a2 + 0x2C34;
    switch (temp_a1) {
    case 0:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        cnLBS_Get_PatchInformation((u8 *)cw + 0xBF20, temp_a1, temp_a2);
        ms_net_patch_set_init();
        all_reset();
        F(s8, &network_work, 0x11) = 1;
        return;
    case 1:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, &network_work, 0x11) = 1;
        return;
    case 2:
        F(s8, &network_work, 0x11) = 1;
        temp_v0 = ms_net_patch_set();
        if (temp_v0 == 1) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
            cnWrap_InitWork();
        } else if (temp_v0 == -1) {
            F(u8, (u8 *)cw, 0x2C34) = 6U;
            cnWrap_InitWork();
        }
        Net_trans_set(0);
        return;
    case 3:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        Lbs_load();
        F(s8, &network_work, 0x11) = 0;
        return;
    case 4:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        F(s8, (u8 *)cw, 0x2C08) = 1;
        CallBackWaitInit();
        cnLBS_Answer_PatchFinish();
        return;
    case 5:
        Check_CallBackWait();
        return;
    case 6:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        Lbs_load();
        F(s8, &network_work, 0x11) = 0;
        return;
    case 7:
        F(s8, temp_a2, 0x2C08) = 1;
        To_LogOut(1);
        /* fallthrough */
    default:
        return;
    }
}

void lbc_login_id_select() {
    s32 s3;
    s32 s2;
    s32 s1;
    s32 s0;
    s32 ix;
    s32 of;
    u8 st;
    u8 *stp;
    s32 r;

    st = cw[0x2C34];
    stp = cw + 0x2C34;
    switch (st) {
    case 0:
        *stp = st + 1;
        cw[2] = 0;
        cw[1] = 0;
        memset(cw + 0xB, 0, 0x20);
        memset(cw + 0x2B, 0, 0x44);
        memset(cw + 0x6F, 0, 0x100);
        cw[0] = cnetGet_Login_NoOfUserAccount();
        s3 = 0;
        if (0 < cw[0]) {
            s2 = 0;
            s1 = 0;
            s0 = 0;
            do {
                cnetGet_Login_UserID(s3, cw + s2 + 0xB);
                cnetGet_Login_UserHandle(s3, cw + s1 + 0x2B);
                cnetGet_Login_UserMiniData(s3, cw + s0 + 0x6F);
                s3 += 1;
                s2 += 8;
                s1 += 0x11;
                s0 += 0x40;
            } while (s3 < cw[0]);
        }
        if (cnWrap_IsBBConnect() != 0) {
            if (cw[0] == 0) {
                cw[0x2C33] = 5;
                cw[0x2C34] = 0;
                return;
            }
        }
        break;
    case 1:
        *stp = st + 1;
        Lbc_init_network_work();
        F(u8, pNet, 6) = cw[2];
        F(u8, pNet, 8) = cw[1];
        return;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cw[0x2C34]++;
            if (CnetWork_c12.x05 == 0) {
                fade_set(2);
                return;
            }
        }
        break;
    case 3:
        r = net_SetMenu_SelectHandleName(&network_work);
        switch (r) {
        case 0:
            cw[0x2C34]++;
            return;
        case -1:
        case -2:
            break;
        }
        break;
    case 4:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cw[0x2C34]++;
            Lbc_set_prim_k(0, 0, 0);
        }
    case 5:
        cw[0x2C34]++;
        ix = F(u8, pNet, 8);
        cw[1] = ix;
        CallBackWaitInit();
        Set_userdata((int)&player_work + (game_w.master * 0xA00));
        of = ix * 0x11;
        memcpy(cw + of + 0x2B, D_3C6FC8, 0x10);
        if ((s8)cw[0xB + ix * 8] == 0) {
            cnLBS_Send_LoginUserAccount(0, cw + of + 0x2B, cw + (ix << 6) + 0x6F);
        } else {
            cnLBS_Send_LoginUserAccount(cw + ix * 8 + 0xB, cw + of + 0x2B, cw + (ix << 6) + 0x6F);
        }
        cnetGet_Login_DecideUserID(cw + 0x440);
        cnetGet_Login_DecideUserHandle(cw + 0x448);
        return;
    case 6:
        Check_CallBackWait();
        break;
    }
}

void CallBack_SendMiniData(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            memcpy((u8 *)cw + 0x45A, &my_user_mini_data, 0x40);
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
            return;
        }
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}

void lbc_login_users_personal_data() {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_pd *c;

    sw = Get_sw2(0);
    c = CWX_c14;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        *stp = st + 1;
    case 1:
        CWX_c14->step++;
        tmpPersonalData = BrPersonalData;
        strcpy(first_url, lit_707_0065E8D0);
        To_BootUpBrowser();
        return;
    case 2:
        if (c->x2C44 == 2) {
            *stp = st + 1;
            CWX_c14->x2C44 = 0;
            return;
        }
        lbc_browser(c->x2C44);
        return;
    case 3:
        if (memcmp(&tmpPersonalData, &BrPersonalData, 0x1D0) != 0) {
            CallBackWaitInit();
            CWX_c14->step++;
            CWX_c14->x2C45 = 0xD;
            cnLBS_RegistPersonalData(&BrPersonalData, CallBack_Result_LoginPersonalDataRegist_o);
            return;
        }
        CWX_c14->step = 8;
        return;
    case 4:
        Check_CallBackWait();
        return;
    case 5:
        *stp = st + 1;
        CWX_c14->x2C4C = 0;
        SetDialogData_HTML(CWX_c14->x32D1);
        return;
    case 6:
        F(s8, pNet, 0xC) = 1;
        if (CWX_c14->x2C4C++ >= 0x258 || (CWX_c14->x2C4C >= 0x3C && (sw & 0x60))) {
            CWX_c14->step++;
        }
        break;
    case 7:
        *stp = st + 1;
    case 8:
        CWX_c14->x2C33 = 4;
        CWX_c14->step = 1;
        ((s8 *)cw)[0x2C35] = 0;
        break;
    }
}

void CallBack_Result_LoginPersonalDataRegist(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xD)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            F(u8, (u8 *)cw, 0x2C34) = 8U;
            return;
        }
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}

void lbc_login_error() {
    u16 sw;

    sw = Get_sw2(0);
    switch (CWX_c16->x2C34) {
    case 0:
        Lbc_init_network_work(1);
        Lbc_set_prim_k(&text_lobby_trans_ot0_o, 0, 0);
        CWX_c16->x2C34++;
        fade_set(2);
        CWX_c16->x2C4C = 0x258;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        break;
    case 1:
        F(s8, pNet, 0xC) = 1;
        CWX_c16->x2C4C--;
        if (CWX_c16->x2C4C > 0) {
            if (CWX_c16->x2C4C < 0x1E0 && (sw & 0x20)) {
                CWX_c16->x2C34++;
                cnWrap_SoundRequest(0);
                fade_set(1);
            }
        } else {
            CWX_c16->x2C34++;
            fade_set(1);
        }
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            F(s8, pNet, 0x11) = 1;
            CWX_c16->x2C33 = 4;
            CWX_c16->x2C34 = 1;
        }
        break;
    }
}

void lbc_login_finish() {
    MINI40 *mini = (MINI40 *)&my_user_mini_data;

    switch (CWX_c17->x2C34) {
    case 0:
        CallBackWaitInit();
        if (CnetWork_c17.x05 == 0) {
            if (CWX_c17->x35D2 == 0) {
                D_3E5326[game_w.master * 0xA00] = 0x4C;
                *(s8 *)0x3F3404 = 0x4C;
            } else {
                D_3E5326[game_w.master * 0xA00] = 0x4D;
                *(s8 *)0x3F3404 = 0x4D;
            }
            Lb_set_mini_data(&my_user_mini_data);
        }
        if (CnetWork_c17.x05 == 3 || CnetWork_c17.x05 == 0) {
            CWX_c17->x35D7 = 0;
            CWX_c17->x35D6 = 0;
            *(s8 *)0x3F3603 = 0;
            *(s8 *)0x3F3604 = 0;
            *(s8 *)0x3F3605 = 0;
            *(s16 *)0x3F3606 = 0;
            Lb_make_quest_tbl();
        }
        mini->x02 = 0;
        mini->x15 = 0;
        CWX_c17->x2C45 = 2;
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_SendMiniData_o);
        CWX_c17->x2C34++;
        return;
    case 1:
        Check_CallBackWait();
        return;
    case 2:
        cnLbc_Init_NgServerId();
        if (CnetWork_c17.x05 == 0 && BsLbsCount == 1) {
            CnetWork_c17.x05 = 1;
        }
        if (CnetWork_c17.x05 == 0) {
            CnetWork_c17.x05 = 1;
            To_LogOut(0);
            return;
        }
        CWX_c17->x2C33 = 8;
        CWX_c17->x2C34 = 0;
    }
}

s32 check_top_information_level(s32 arg0) {
    switch (arg0 & 0xFF) {
    case 0:
        return 0;
    case 1:
        if (CnetWork_c18.x05 == 1) {
            return 1;
        }
        break;
    case 2:
        if (CnetWork_c18.x05 == 1) {
            return 1;
        }
        if (CnetWork_c18.x05 == 3) {
            return 1;
        }
        break;
    case 3:
        if (CnetWork_c18.x05 == 1) {
            return 1;
        }
        if (CnetWork_c18.x05 == 3) {
            return 1;
        }
        if (CnetWork_c18.x05 == 2) {
            return 1;
        }
        break;
    }
    return 0;
}

void lbc_login_top_information() {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_lt *c;
    s32 *p;
    s32 v;

    sw = Get_sw2(0);
    c = CWX_c19;
    st = c->step;
    stp = &c->step;
    switch (st) {
    case 0:
        *stp = st + 1;
        Lbc_set_prim_k(text_lobby_trans_ot0_o, 0, 0);
        SetSceneTitle(0, 1);
        CallBackWaitInit();
        cnLBS_Read_TopInformation(CallBack_Result_LoginTopInformation_o);
        return;
    case 1:
        Check_CallBackWait();
        return;
    case 2:
        *stp = st + 1;
        cnLBS_Get_TopInformation(&CWX_c19->x4602);
        return;
    case 3:
        if (check_top_information_level_k(c->x4602) != 0) {
            CWX_c19->step++;
            return;
        }
        CWX_c19->step = 7;
        return;
    case 4:
        *stp = st + 1;
        fade_set(2);
        SetDialogData_HTML(CWX_c19->x4606);
        CWX_c19->x2C4C = 0x1E;
        CWX_c19->x2C50 = 0x708;
        return;
    case 5:
        F(s8, pNet, 0xC) = 1;
        c = CWX_c19;
        p = &c->x2C4C;
        v = *p;
        if (v != 0) {
            *p = v - 1;
            return;
        }
        if ((u8)Fade_busy_ck() != 1) {
            if ((sw & 0x20) || (v = CWX_c19->x2C50 - 1, CWX_c19->x2C50 = v, v <= 0)) {
                CWX_c19->step++;
                cnWrap_SoundRequest(0);
                fade_set(1);
                return;
            }
        }
        break;
    case 6:
        F(s8, pNet, 0xC) = 1;
        if ((u8)Fade_busy_ck() == 1) {
            break;
        }
    case 7:
        CWX_c19->x2C33 = 9;
        CWX_c19->step = 0;
        break;
    }
}

void CallBack_Result_LoginTopInformation(CNET_RES res) {
    int temp_a1;

    temp_a1 = (int)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C34) = (u8) (F(u8, temp_a1, 0x2C34) + 1);
        } else {
            F(u8, temp_a1, 0x2C34) = 7U;
        }
    }
}

void lbc_login_finish_after() {
    switch (CWX_c21->x2C34) {
    case 0:
        if (CnetWork_c21.x05 == 3) {
            CWX_c21->x2C34++;
            McOperationSet(7, 2);
            fade_set(2);
            CWX_c21->x2C4C = 3;
            str_stop_all();
            break;
        }
        CWX_c21->x2C34 = 2;
        break;
    case 1:
        if (CWX_c21->x2C4C == 0) {
            if (McCardOperation(CWX_c21, &CWX_c21->x2C4C) & 0xFF) {
                CWX_c21->x2C34++;
                fade_set(1);
            }
        } else {
            CWX_c21->x2C4C--;
        }
        break;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            cnLBS_Send_LoginFinish();
            if (CnetWork_c21.x05 == 3) {
                To_MyLobby();
                CWX_c21->x35D2 = 1;
            } else {
                To_MyLobby();
                CWX_c21->x35D2 = 0;
            }
            CnetWork_c21.x05 = 2;
            Q_camera_init();
        }
        break;
    }
}

#ifdef __MWERKS__
asm int CallBack_Result_LoginLobbyServer()
{
#include "CallBack_Result_LoginLobbyServer.inc"
}
#endif

void lbc_browser() {
    ((int (**)())lbc_user_regist_jmp_939)[F(u8, (u8 *)cw, 0x2C43)]();
}

void To_BootUpBrowser() {
    F(s8, (u8 *)cw, 0x2C43) = 0;
    F(s8, (u8 *)cw, 0x2C44) = 1;
    F(s8, (u8 *)cw, 0x2C08) = 0;
}

void lbc_browser_00() {
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    F(u8, temp_a0, 0x2C43) = (u8) (F(u8, temp_a0, 0x2C43) + 1);
    F(s8, (u8 *)cw, 0x2C08) = 0;
    BsLbsErrNum_c23 = 0;
}

void lbc_browser_01() {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
    all_reset();
    cnWrap_InitWork();
    Lbs_load();
    net_char_change = 1;
}

void lbc_browser_02() {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
    BS_MODE_R_NO = 0;
    FlushCache(0);
    MainBsInitialize(1);
    memset(&FirstURL, 0, 0x100);
    strcpy(&FirstURL, &first_url);
}

void lbc_browser_03() {
    tmpPersonalData_c24 = BrPersonalData_c24;
    ((CWS_b3 *)cw)->x2C43 = ((CWS_b3 *)cw)->x2C43 + 1;
    ((CWS_b3 *)cw)->x2C08 = 1;
    fade_set(2);
}

void lbc_browser_04() {
    s32 temp_v0;

    F(s8, pNet_c25, 0x11) = 1;
    temp_v0 = MainBrowser();
    switch (temp_v0) {
    case 0:
        break;
    case 1:
        MainBsDispose();
        if (memcmp(&tmpPersonalData_c25, &BrPersonalData_c25, 0x1D0) != 0) {
            F(s8, (u8 *)cw, 0x2C3C) = 1;
        } else {
            F(s8, (u8 *)cw, 0x2C3C) = 0;
        }
        ((CWS_lbc_browser_04 *)cw)->x2C43 = (u8) (((CWS_lbc_browser_04 *)cw)->x2C43 + 1);
        fade_set(1);
        break;
    case -1:
        MainBsDispose();
        ((CWS_lbc_browser_04 *)cw)->x2C43 = (u8) (((CWS_lbc_browser_04 *)cw)->x2C43 + 1);
        fade_set(2);
        break;
    }
}

void lbc_browser_05() {
    s32 temp_a0;
    void *temp_v1;

    temp_a0 = Fade_busy_ck() & 0xFF;
    if (temp_a0 != 1) {
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
        all_reset(temp_a0);
        Lbs_load();
        net_char_change = 0;
        F(s8, (u8 *)cw, 0x2C08) = 1;
        F(s8, (u8 *)cw, 0x2C44) = 2;
        BsLbsErrNum_c26 = 0;
    }
}

s32 Lbc_SendBrowserResult() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        CallBackWaitInit(temp_a0);
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C35) = (u8) (F(u8, temp_v1_3, 0x2C35) + 1);
        cnLBS_RegistPersonalData(&BrPersonalData_c27, &CallBack_Result_LoginPersonalDataRegist2_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        return 1;
    }
    return 2;
}

void CallBack_Result_LoginPersonalDataRegist2(CNET_RES res) {
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
            return;
        }
        F(u8, temp_a1, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}

void To_MyLobby() {
    F(s8, (u8 *)cw, 0x2C31) = 8;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C3F) = 0;
    Init_InterruptFlag();
}

void lobby_return_to_lobby() {
    switch (CWX_c30->x2C32) {
    case 0:
        CWX_c30->x2C32++;
        CWX_c30->x2C08 = 0;
        CWX_c30->x2C45 = 1;
        CallBackWaitInit();
        cnLBS_Read_CurrentPlace(&CallBack_ReadCurrentPlace_o);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        CWX_c30->x2C32++;
        cnLBS_Read_PlazaAllocation(0, 7, &CallBack_Result_Plaza_ReadAllocation2_o);
        Set_userdata((u8 *)&player_work + *(u8 *)0x3F34C1 * 0xA00);
        Lbc_SendMiniData();
        break;
    case 3:
        CWX_c30->x2C32++;
        CWX_c30->x35D5 = 1;
        MH_lobbyClear();
        cnLBS_Read_LobbyAllocation(0, 7, &CallBack_Result_Plaza_ReadLobbyAllocation2_o);
        break;
    case 4:
        switch (Lbs_request_enter_lobby2()) {
        case 0:
            CWX_c30->x2C32++;
            break;
        case 1:
            break;
        }
        break;
    case 5:
        switch (Lbc_DownloadQuest()) {
        case 0:
            CWX_c30->x2C08 = 1;
            To_EnterLobby();
            return;
        case 1:
            break;
        }
        break;
    }
}

void CallBack_Result_Plaza_ReadLobbyAllocation2(CNET_RES res) {
    int i;
    PLZ *p;

    if ((F(u8, (u8 *)cw, 0x2C31) != 5) && ((res.val != 2) || (res.id != 0xB)) && (res.val == 0)) {
        cnLBS_Get_LobbyCount(&ClassInfo.x6);
        i = 0;
        if (0 < ClassInfo.x6) {
            p = LobbyInfo;
            do {
                p->x0 = i + 1;
                cnLBS_Get_LobbyStatus(i + 1, (u8 *)p + 0x10);
                cnLBS_Get_mhLobbyJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                cnLBS_Get_LobbyName(i + 1, (u8 *)p + 0x14);
                i++;
                p++;
            } while (i < ClassInfo.x6);
        }
    }
}

void CallBack_ReadCurrentPlace(CNET_RES res) {
    u16 sp18[3];

    if ((CWX_c32->x2C31 != 5) && (CWX_c32->x2C45 == 1)) {
        CWX_c32->x2C45 = 0;
        if (res.val == 0) {
            CWX_c32->x2C32++;
            cnLBS_Get_CurrentPlace(sp18);
            ClassInfo_c32.x0 = sp18[0];
            ClassInfo_c32.x4 = sp18[1];
            if (ClassInfo_c32.x0 == 0 || ClassInfo_c32.x4 == 0) {
                To_TopMenu();
                CWX_c32->x35D2 = 0;
            } else {
                CWX_c32->x35D2 = 1;
            }
        } else {
            To_TopMenu();
            CWX_c32->x35D2 = 0;
        }
    }
}

void MH_lobbyClear() {
    u8 *m;

    memset((u8 *)cw + 0x10AC, 0, 0x17E0);
    memset((u8 *)cw + 0x2BFE, 0, 8);
    memset(&lbCommer, 0, 0xB80);
    F(s8, (u8 *)cw, 3) = 0;
    F(s8, (u8 *)cw, 0x2C06) = 0;
    F(s8, (u8 *)cw, 0x32C5) = 0;
    F(s8, (u8 *)cw, 0x2C07) = 0;
    Lb_clearChatList();
    m = (u8 *)&my_user_mini_data;
    if ((m[2] != 0) || (m[0x15] != 0)) {
        m[0x15] = 0;
        m[2] = 0;
        cnLBS_Send_UserMiniData(m, 0x40, &CallBack_Result_SendUserMiniData_o);
    }
}

void lobby_client_top_menu() {
    ((int (**)())lobby_client_top_menu_jmp_1131)[F(u8, (u8 *)cw, 0x2C32)]();
}

void lbc_top_menu() {
    ((int (**)())lobby_client_top_menu_jmp_1136)[F(u8, (u8 *)cw, 0x2C33)]();
}

void To_TopMenu() {
    F(s8, (u8 *)cw, 0x2C31) = 1;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C3F) = 0;
    Init_InterruptFlag();
}

void lbc_top_menu_00() {
    int var_v1;
    s32 var_a0;

    F(s8, (u8 *)cw, 0x2C33) = 5;
    F(s8, &ClassInfo_c36, 0) = 0;
    F(s8, &ClassInfo_c36, 4) = 0;
    F(s8, &ClassInfo_c36, 8) = 0;
    memset(&PlazaInfo, 0, 0xD98);
    var_a0 = 0;
    var_v1 = (int)&PlazaInfo;
    do {
        F(s8, var_v1, 0x10) = 0;
        F(s8, var_v1, 0x16C) = 0;
        var_a0 += 5;
        F(s8, var_v1, 0x2C8) = 0;
        F(s8, var_v1, 0x424) = 0;
        F(s8, var_v1, 0x580) = 0;
        var_v1 += 0x6CC;
    } while (var_a0 < 0xA);
    Lbc_init_network_work(var_a0);
    F(s8, pNet, 6) = 0;
}

void lbc_top_menu_01() {
    int i;
    PLI *p;

    CWX_c37->x2C08 = 1;
    i = 0;
    if (0 < ClassInfo_c37.x2) {
        p = PlazaInfo_c37;
        do {
            if (p->st == 3) {
                break;
            }
            i++;
            p++;
            if (i == ClassInfo_c37.x2) {
                To_LogOut(1);
            }
        } while (i < ClassInfo_c37.x2);
    }
    ClassInfo_c37.x0 = i + 1;
    CWX_c37->x2C33 = 2;
    CWX_c37->x2C34 = 0;
}

void lbc_top_menu_02() {
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 4;
        cnLBS_PlazaEntry(cnLbc_CheckInFloorOrder(0) & 0xFFFF, &CallBack_Result_Plaza_PlazaEntry_o);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, temp_a1);
    }
}

void CallBack_Result_Plaza_PlazaEntry(CNET_RES res) {
    if ((((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C45 == 4)) {
        ((CWS_CallBack_Result_Plaza_PlazaEntry *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            To_EnterPlaza();
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        F(s8, (u8 *)cw, 0x2C35) = 0;
    }
}

s32 Lbs_ExitAndEnterPlaza(arg0)
int arg0;
{
    switch (F(u8, (u8 *)cw, 0x2C35)) {
    case 0:
        F(u8, (u8 *)cw, 0x2C35)++;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 9;
        cnLBS_PlazaExit(CallBack_Result_Plaza_PlazaExit2);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        F(u8, (u8 *)cw, 0x2C35)++;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 4;
        cnLBS_PlazaEntry(arg0 & 0xFFFF, CallBack_Result_Plaza_PlazaEntry2);
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 0;
    case 5:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 1;
    }
    return 2;
}

void CallBack_Result_Plaza_PlazaExit2(CNET_RES res) {
    u8 temp_a0_2;
    int temp_a0;
    int temp_v1;

    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (temp_a0_2 = F(u8, temp_a0, 0x2C45), (temp_a0_2 == 9))) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C35) = (u8) (F(u8, temp_v1, 0x2C35) + 1);
            Init_InterruptFlag(temp_a0_2, 5, temp_a0 + 0x2C45);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}

void CallBack_Result_Plaza_PlazaEntry2(CNET_RES res) {
    int temp_a0;
    int temp_a0_2;
    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 4)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C35) = (u8) (F(u8, temp_a0_2, 0x2C35) + 1);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}

void lbc_top_menu_03() {
    To_TopMenu();
}

void lbc_top_menu_04() {
    To_TopMenu();
}

void lbc_top_menu_05() {
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 6;
        cnLBS_Read_PlazaAllocation(0, 7, &CallBack_Result_Plaza_ReadAllocation_o);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, temp_a1);
    }
}

void CallBack_Result_Plaza_ReadAllocation(CNET_RES res) {
    int i;
    PLZ *p;

    if ((CWX_c43->x2C31 != 5) && (CWX_c43->x2C45 == 6) && (res.val != 2) && (res.val == 0)) {
        CWX_c43->x2C45 = 0;
        CWX_c43->x2C33 = 1;
        CWX_c43->x2C34 = 0;
        cnLBS_Get_PlazaCount(&ClassInfo_c43.x2);
        i = 0;
        if (0 < ClassInfo_c43.x2) {
            p = PlazaInfo_c43;
            do {
                p->x0 = i + 1;
                cnLBS_Get_PlazaStatus(i + 1, (u8 *)p + 0x10);
                cnLBS_Get_mhPlazaJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                cnLBS_Get_PlazaName(i + 1, (u8 *)p + 0x14);
                i++;
                p++;
            } while (i < ClassInfo_c43.x2);
        }
    }
}

void lbc_top_menu_06() {
    F(s8, (u8 *)cw, 0x2C33) = 1;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, &ClassInfo_c44, 0) = 0;
    F(s8, &ClassInfo_c44, 4) = 0;
    F(s8, &ClassInfo_c44, 8) = 0;
    Lbc_init_network_work();
    F(s8, pNet_c44, 6) = 0;
}

void lobby_client_game_in_plaza() {
    if (Check_InterruptFlag() == 0) {
        if (((int *)&lbc_in_plaza_jmp_1345)[F(u8, (u8 *)cw, 0x2C33)] == 0) {
            To_TopMenu();
        }
        ((int (**)())&lbc_in_plaza_jmp_1345)[F(u8, (u8 *)cw, 0x2C33)]();
    }
}

void To_EnterPlaza() {
    F(s8, (u8 *)cw, 0x2C31) = 2;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C41) = 0;
    F(s8, (u8 *)cw, 0x2C42) = 0;
    F(s8, &ClassInfo_c46, 4) = 0;
    F(s8, &ClassInfo_c46, 8) = 0;
    memset(&LobbyInfo_c46, 0, 0x1308);
    Plaza_chat_clear();
    Lb_clearChatList();
    fade_set(2);
    SetDialogData(0xB, 5);
}

void To_EnterPlaza2Lobby() {
    F(s8, (u8 *)cw, 0x2C31) = 2;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag();
    Lbc_init_network_work();
    F(s8, (u8 *)cw, 0x2C08) = 0;
    F(s8, &ClassInfo_c46, 4) = 0;
    F(s8, &ClassInfo_c46, 8) = 0;
    memset(&LobbyInfo_c46, 0, 0x1308);
    Pit_reset();
    all_reset();
    Lbs_load();
    Plaza_chat_clear();
    Lb_clearChatList();
    Lbc_set_prim_k(&put_back, 0, 0);
    cnLBS_Read_PlazaAllocation(0, 7, &CallBack_Result_Plaza_ReadAllocation2_o);
    SetDialogData(0xC, 5);
}

void CallBack_Result_Plaza_ReadAllocation2(CNET_RES res) {
    int i;
    PLZ *p;

    if ((F(u8, (u8 *)cw, 0x2C31) != 5) && (res.val != 2) && (res.val == 0)) {
        cnLBS_Get_PlazaCount(&ClassInfo_c47.x2);
        i = 0;
        if (0 < ClassInfo_c47.x2) {
            p = PlazaInfo_c47;
            do {
                p->x0 = i + 1;
                cnLBS_Get_PlazaStatus(i + 1, (u8 *)p + 0x10);
                cnLBS_Get_mhPlazaJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                cnLBS_Get_PlazaName(i + 1, (u8 *)p + 0x14);
                i++;
                p++;
            } while (i < ClassInfo_c47.x2);
        }
    }
}

void lbc_in_plaza_00() {
    s32 temp_a2;
    u8 temp_a0_2;
    int temp_a0;

    F(s8, pNet, 0xC) = 1;
    temp_a0 = (int)cw;
    temp_a2 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, 1, temp_a2);
        F(s8, (u8 *)cw, 0x2C45) = 0xB;
        cnLBS_Read_LobbyAllocation(0, 7, &CallBack_Result_Plaza_ReadLobbyAllocation_o);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, 1, temp_a2);
        return;
    case 2:
        if ((Fade_busy_ck(temp_a0_2, 1, temp_a2) & 0xFF) != 1) {
            F(s8, (u8 *)cw, 0x2C33) = 1;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
        }
    }
}

void Lbc_init_network_work() {
    memset(pNet, 0, 0x2C);
    F(int, pNet, 0x14) = (int)&lb_prim;
    F(int, pNet, 0x18) = (int)&D_3EBC90;
    F(int, pNet, 0x1C) = (int)&D_3EBCB0;
    F(int, pNet, 0x20) = (int)&D_3EBCD0;
    F(int, F(int, pNet, 0x20), 0x14) = (int)&text_lobby_trans_ot3_o;
    F(s32, F(int, pNet, 0x14), 0x18) = 0;
    F(s32, F(int, pNet, 0x18), 0x18) = 1;
    F(s32, F(int, pNet, 0x1C), 0x18) = 2;
    F(s32, F(int, pNet, 0x20), 0x18) = 3;
}

void lbc_in_plaza_01() {
    switch (((CWS_ip1 *)cw)->x2C34) {
    case 0:
        ((CWS_ip1 *)cw)->x2C08 = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            ((CWS_ip1 *)cw)->x2C34 = ((CWS_ip1 *)cw)->x2C34 + 1;
            Lbc_init_network_work();
            fade_set(2);
        }
        break;
    case 1:
        switch (Lbs_plaza(network_work)) {
        case 1:
            break;
        case 2:
            To_LogOut(0);
            break;
        }
        break;
    }
}

void lbc_in_plaza_02() {
    To_LogOut(1);
}

s32 Lbs_request_enter_lobby() {
    u8 temp_a0;
    int temp_a1;
    int temp_v0;

    temp_v0 = (int)cw;
    temp_a0 = F(u8, temp_v0, 0x2C35);
    temp_a1 = temp_v0 + 0x2C35;
    switch (temp_a0) {
    case 0:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        MH_lobbyClear(temp_a0, temp_a1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 8;
        cnLBS_LobbyEntry(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Plaza_LobbyEntry_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0, temp_a1);
        break;
    case 2:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        CallBackWaitInit(temp_a0, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_Read_LobbyMemberList(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Plaza_LobbyMember_o);
        break;
    case 3:
        Check_CallBackWait(temp_a0, temp_a1);
        break;
    case 4:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 0;
    case 5:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

s32 Lbs_request_enter_lobby2() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_Read_LobbyMemberList(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Plaza_LobbyMember_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

void CallBack_Result_Plaza_LobbyEntry(CNET_RES res) {
    u8 *temp_a0;
    temp_a0 = (u8 *)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 8)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x2C35) = 2;
            F(s8, (u8 *)cw, 0x35D5) = 1;
            return;
        }
        F(s8, (u8 *)cw, 0x2C35) = 5;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, 5, temp_a0 + 0x2C45);
    }
}

void CallBack_Result_Plaza_LobbyMember(CNET_RES res) {
    LUSER u;
    s32 i;
    s32 j;
    u8 *w;
    s32 found;
    s32 off;
    s32 o;
    u8 *p3;
    u8 *e;

    if (cw[0x2C31] != 5) {
        if (cw[0x2C45] == 10) {
            cw[0x2C45] = 0;
            if (res.val == 0) {
                game_w.pl_num = 0;
                i = 0;
                off = 0;
                w = (u8 *)lbCommer;
                do {
                    cnLBS_Get_LobbyMemberList(i & 0xFFFF, &u, u.name, u.mini);
                    if (u.id[0] != 0) {
                        found = 0;
                        j = 0;
                        o = 0;
                        do {
                            if (memcmp(&u, cw + o + 0x132C, 8) == 0) {
                                found = 1;
                            }
                            j++;
                            o += 0x2FC;
                        } while (j < 8);
                        if (found == 0) {
                            strcpy(cw + off + 0x132C, u.id);
                            strcpy(cw + off + 0x1334, u.name);
                            memcpy(cw + off + 0x1346, u.mini, 0x40);
                            memcpy(w, cw + off + 0x132C, 8);
                            memcpy(w + 8, cw + off + 0x1334, 0x10);
                            memcpy(w + 0x1C, cw + off + 0x1346, 0x40);
                            if (memcmp(&u, cw + 0x440, 8) == 0) {
                                game_w.master = i;
                            }
                            if (game_w.pl_num == 0 || 0 > memcmp(p3 = cw + 3, cw + off + 0x132C, 8)) {
                                memcpy(cw + 3, cw + off + 0x132C, 8);
                            }
                            Lb_set_mini_data_to_pl((s8)i, cw + off + 0x1346);
                            game_w.pl_num++;
                            e = cw + off;
                            Lb_set_player(i & 0xFF, e + 0x132C, e + 0x1334);
                        }
                    }
                    i++;
                    off += 0x2FC;
                    w += 0x5C;
                } while (i < 8);
                cw[0x35D1] = game_w.pl_num;
                cw[0x2C35]++;
                lb_sys.x04 = 0;
            } else {
                cnLBS_Get_ServerMessage(cw + 0x32D1);
                cw[0x2C35] += 2;
            }
        }
    }
}

s32 Lbs_GetLobbyMemberList(arg0)
s16 arg0;
{
    switch (((u8 *)pNet)[0x12]) {
    case 0:
        CWX_c56->x2C45 = 0xA;
        ((u8 *)pNet)[0x12]++;
        cnLBS_Read_LobbyMemberList(arg0 + 1, &CallBack_Result_Plaza_LobbyMember2_o);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        ((u8 *)pNet)[0x12] = 0;
        return 1;
    }
    return 0;
}

void CallBack_Result_Plaza_LobbyMember2(CNET_RES res) {
    s32 var_s1;
    int var_s0;
    int temp_a0;

    if ((((CWS_LobbyMember2 *)cw)->x2C31 != 5) && (((CWS_LobbyMember2 *)cw)->x2C45 == 0xA)) {
        ((CWS_LobbyMember2 *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            var_s1 = 0;
            var_s0 = (int)&tl_member_buff;
            do {
                cnLBS_Get_LobbyMemberList(var_s1 & 0xFFFF, var_s0 + 0x280, var_s0 + 0x288, var_s0 + 0x29A);
                var_s1 += 1;
                var_s0 += 0x2FC;
            } while (var_s1 < 8);
        }
        temp_a0 = (int)pNet;
        F(u8, temp_a0, 0x12) = (u8) (F(u8, temp_a0, 0x12) + 1);
    }
}

void lbc_in_plaza_03() {
    u8 var_a0;
    int temp_a1;
    int temp_a2;

    temp_a2 = (int)cw;
    var_a0 = F(u8, temp_a2, 0x2C34);
    temp_a1 = temp_a2 + 0x2C34;
    switch (var_a0) {                               /* irregular */
    case 0:
        F(u8, temp_a2, 0x2C34) = (u8) (var_a0 + 1);
        Lbc_init_network_work(var_a0, temp_a1, temp_a2);
        F(s32, (u8 *)cw, 0x2C4C) = 0x44;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        F(s8, pNet, 0xC) = 1;
        F(s32, (u8 *)cw, 0x2C4C) = (F(s32, (u8 *)cw, 0x2C4C) - 1);
        if (F(s32, (u8 *)cw, 0x2C4C) <= 0) {
        case 2:
            F(u8, (u8 *)cw, 0x2C34) = (u8) (F(u8, (u8 *)cw, 0x2C34) + 1);
            CallBackWaitInit();
            F(s8, (u8 *)cw, 0x2C45) = 9;
            cnLBS_PlazaExit(&CallBack_Result_Plaza_PlazaExit_o);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
    }
}

void CallBack_Result_Plaza_PlazaExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C45 == 9)) {
        ((CWS_CallBack_Result_Plaza_PlazaExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            F(u8, (u8 *)cw, 0x2C31) = 1U;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 0;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            Init_InterruptFlag();
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
    }
}

void lbc_in_plaza_04() {
    u16 sw;
    sw = Get_sw2(0);
    if (CWX_c60->x2C4C++ >= 0x258 || (CWX_c60->x2C4C >= 0x3C && (sw & 0x60))) {
        CWX_c60->x2C33 = 0;
        CWX_c60->x2C34 = 0;
        CWX_c60->x2C3C = 0;
    }
}

void CallBack_Result_Plaza_ReadLobbyAllocation(CNET_RES res) {
    int sp3C;
    int i;
    PLZ *p;

    if ((CWX_c61->x2C31 != 5) && (CWX_c61->x2C45 == 0xB)) {
        if (res.val == 2 && res.id == 0xB) {
            cnLBS_Get_AllocationProgressCount(&sp3C);
            return;
        }
        if (res.val == 0) {
            CWX_c61->x2C45 = 0;
            CWX_c61->x2C34++;
            fade_set(1);
            cnLBS_Get_LobbyCount(&ClassInfo.x6);
            i = 0;
            if (0 < ClassInfo.x6) {
                p = LobbyInfo;
                do {
                    p->x0 = i + 1;
                    cnLBS_Get_LobbyStatus(i + 1, (u8 *)p + 0x10);
                    cnLBS_Get_mhLobbyJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                    cnLBS_Get_LobbyName(i + 1, (u8 *)p + 0x14);
                    i++;
                    p++;
                } while (i < ClassInfo.x6);
            }
        }
    }
}

typedef struct { char c[0x44]; } CSI;
s32 Lbc_ConditionSearch(arg0, arg1)
CSI *arg0;
int arg1;
{
    struct { s8 a; s8 n; u8 p[2]; CSI item[8]; } sp;
    int i;
    s8 n;
    u8 st;

    st = F(u8, (u8 *)cw, 0x2C35);
    switch (st) {
    case 0:
        F(u8, (u8 *)cw, 0x2C35) = st + 1;
        sp.a = 0x50;
        i = 0;
        n = arg1;
        sp.n = arg1;
        for (; i < n; i++) {
            sp.item[i] = *arg0++;
        }
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_ConditionSearchUser(&sp, CallBack_Result_ConditionSearchUser);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        cnLBS_Get_ConditionSearchUser(&SearchResult);
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 0;
    case 3:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 1;
    }
    return 2;
}

void CallBack_Result_ConditionSearchUser(CNET_RES res) {
    int temp_a1;

    temp_a1 = (int)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xA)) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
        } else {
            F(u8, temp_a1, 0x2C35) = 3U;
        }
    }
}

s32 Lbs_SeekId() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v0;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_SerchUserPlace((u8 *)cw + 0x2F80, &CallBack_Result_SearchUserPlace_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        temp_v0 = (int)cw;
        cnLBS_Get_SerchUserPlace(temp_v0 + 0x30B4, temp_v0 + 0x30B6, temp_v0 + 0x30B8, temp_v0 + 0x30BB);
        cnLBS_Get_SerchUserPlaceMessage((u8 *)cw + 0x30BC);
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
        return 1;
    }
    return 2;
}

void CallBack_Result_SearchUserPlace(CNET_RES res) {
    int temp_a1;

    temp_a1 = (int)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xA)) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
        } else {
            F(u8, temp_a1, 0x2C35) = 3U;
        }
    }
}

void To_PlazaExit(s32 arg0) {
    if (arg0 == 0) {
        F(s8, (u8 *)cw, 0x2C31) = 2;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 2;
    } else {
        F(s8, (u8 *)cw, 0x2C31) = 2;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 0;
    }
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 0;
    F(s8, (u8 *)cw, 0x2C09) = 2;
}

void lobby_client_game_in_lobby() {
    int temp_a0;

    if (Check_InterruptFlag() != 0) {
        temp_a0 = (int)cw;
        if ((F(u8, temp_a0, 0x2C33) == 0) && (F(u8, temp_a0, 0x2C34) == 1)) {
            Lbc_set_prim_k(0, 0, &put_back);
            F(s8, pNet, 0x11) = 0;
        }
        return;
    }
    if (((int *)&lbc_in_lobby_jmp_1826)[F(u8, (u8 *)cw, 0x2C33)] == 0) {
        To_LogOut(1);
    }
    ((int (**)())&lbc_in_lobby_jmp_1826)[F(u8, (u8 *)cw, 0x2C33)]();
}

void lbc_in_lobby_00() {
    if (((int *)&lbc_in_lobby_00_jmp_1842)[F(u8, (u8 *)cw, 0x2C34)] == 0) {
        To_LogOut(1);
    }
    ((int (**)())&lbc_in_lobby_00_jmp_1842)[F(u8, (u8 *)cw, 0x2C34)]();
}

void To_EnterLobby() {
    F(s8, (u8 *)cw, 0x2C31) = 3;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    memset(&RoomInfo, 0, 0xAE0);
    F(s8, &ClassInfo_c67, 8) = 0;
    F(s8, (u8 *)cw, 0x32C0) = 0;
    F(s8, (u8 *)cw, 0x32C1) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    F(s8, (u8 *)cw, 0x35D3) = 0;
    F(s8, (u8 *)cw, 0x35D4) = 0;
    F(s8, &lb_sys, 0x70) = 0;
    Plaza_chat_clear();
    Lb_clearChatList();
    cnLBS_Read_LobbyJoinUser(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Lobby_JoinUser_o);
}

void CallBack_Result_Lobby_JoinUser(CNET_RES res) {
    int n;

    if (F(u8, cw, 0x2C31) != 5) {
        n = cnLbc_CheckInFloorOrder(1);
        if (res.val == 0) {
            cnLBS_Get_mhLobbyJoinUser(n & 0xFFFF, (u8 *)&LobbyInfo_c68[n - 1] + 2, (u8 *)&LobbyInfo_c68[n - 1] + 0xE);
            return;
        }
        *(s16 *)(D_3A14C6 + n * 0x15C) = 0;
    }
}

void lbc_in_lobby_00_00() {
    void *temp_v1;

    cnLbc_CheckInFloorOrder(1);
    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C39) = 0;
    F(s8, (u8 *)cw, 0x32C5) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    cnWrap_InitWork();
    Lbc_init_network_work();
    Lbc_set_prim_k(0, 0, 0);
    Lbc_release();
    Clear_lobby_ram();
    str_stop(0);
    str_stop(1);
}

s32 Lbc_DownloadQuest() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C2F) = 0;
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xC;
        cnLBS_Read_FileDownload(mission_area_c70, &CallBack_Result_ReadFileDownload_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

void CallBack_Result_ReadFileDownload(CNET_RES res) {
    int sp1C;
    s32 sp18;
    int temp_a2;

    temp_a2 = (int)cw;
    if ((F(u8, temp_a2, 0x2C31) != 5) && (F(u8, temp_a2, 0x2C45) == 0xC)) {
        if (res.val == 0) {
            F(u8, temp_a2, 0x2C35) = (u8) (F(u8, temp_a2, 0x2C35) + 1);
            cnLBS_Get_FileDownloadInfo(&sp18, &sp1C, temp_a2);
            if (sp18 != 0) {
                F(s8, (u8 *)cw, 0x2C2F) = 1;
            }
        } else {
            F(u8, temp_a2, 0x2C35) = 3U;
        }
    }
}

s32 Lbc_ReadRoomInfo() {
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        memset(&RoomInfo, 0, 0xAE0);
        F(s16, &RoomInfo, 0) = 1;
        F(s16, &RoomInfo, 0x15C) = 2;
        F(s16, &RoomInfo, 0x2B8) = 3;
        F(s16, &RoomInfo, 0x414) = 4;
        F(s16, &RoomInfo, 0x570) = 5;
        F(s16, &RoomInfo, 0x6CC) = 6;
        F(s16, &RoomInfo, 0x828) = 7;
        F(s16, &RoomInfo, 0x984) = 8;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0xE;
        cnLBS_Read_RoomAllocation(0, 0xBB, &CallBack_Result_Lobby_ReadRoomAllocation_o);
        break;
    case 1:
        Check_CallBackWait(temp_v1 + 0x2C35);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

void CallBack_Result_Lobby_ReadRoomAllocation(CNET_RES res) {
    int sp3C;
    int i;
    PLZ *p;

    if ((CWX_c73->x2C31 != 5) && (CWX_c73->x2C45 == 0xE)) {
        if (res.val == 2) {
            if (res.id == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp3C);
            }
        } else if (res.val == 0) {
            CWX_c73->x2C45 = 0;
            CWX_c73->x2C35++;
            cnLBS_Get_RoomCount(&ClassInfo_c73.xA);
            i = 0;
            if (0 < ClassInfo_c73.xA) {
                p = RoomInfo_c73;
                do {
                    p->x0 = i + 1;
                    cnLBS_Get_RoomStatus(i + 1, (u8 *)p + 0x10);
                    cnLBS_Get_mhRoomJoinUser(i + 1, (u8 *)p + 2, (u8 *)p + 0xE);
                    cnLBS_Get_RoomJoinInfo(i + 1, (u8 *)p + 4, (u8 *)p + 6, (u8 *)p + 8, (u8 *)p + 10, (u8 *)p + 12);
                    cnLBS_Get_RoomPasswordInfo(i + 1, (u8 *)p + 0x11);
                    cnLBS_Get_RoomProperty(i + 1, (u8 *)p + 0x158);
                    cnLBS_Get_RoomExplain(i + 1, (u8 *)p + 0x55);
                    i++;
                    p++;
                } while (i < ClassInfo_c73.xA);
            }
        } else {
            CWX_c73->x2C35 = 3;
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
        }
    }
}

u16 Lbs_GetClassAdd() {
    return F(u16, &ClassInfo_c74, 0xA);
}

int Lbs_GetRoomInfo(int arg0) {
    return (int)&RoomInfo + ((s16)arg0 * 0x15C);
}

void lbc_in_lobby_00_05() {
    int idx;
    CWX_c76->x2C4C--;
    if (CWX_c76->x2C4C < 0) {
        idx = CWX_c76->x32C3 + CWX_c76->x32C4;
        switch (RoomInfo_c76[idx].x10) {
        case 3:
            F(s8, &ClassInfo_c76, 8) = (s8) RoomInfo_c76[idx].x0;
            CWX_c76->x2C33 = 2;
            CWX_c76->x2C34 = 0;
            CWX_c76->x2C35 = 0;
            CWX_c76->x2C36 = 0;
            break;
        case 1:
            F(s8, &ClassInfo_c76, 8) = (s8) RoomInfo_c76[idx].x0;
            CWX_c76->x2C33 = 1;
            CWX_c76->x2C34 = 0;
            CWX_c76->x2C35 = 0;
            CWX_c76->x2C36 = 0;
            F(s8, (u8 *)cw, 0x32C2) = 0;
            break;
        default:
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            break;
        }
    }
}

s32 Lbc_getDate() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C45) = 0x17;
        CallBackWaitInit(temp_a0);
        cnLBS_Read_TimingValue(&CallBack_GetDate_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    }
    return 2;
}

void CallBack_GetDate(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x17)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            cnLBS_Get_TimingValue((u8 *)cw + 0xBF3C, temp_a1, temp_a1 + 0x2C45);
        } else {
            F(s32, (u8 *)cw, 0xBF3C) = 0;
        }
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
    }
}

s32 Lbc_ReserveRoom() {
    int i;
    u8 *st;


    st = (u8 *)cw + 0x2C35;
    switch (*st) {
    case 0:
        for (i = 0; i < 8; i++) {
            if (RoomInfo_c79[i].st == 1) {
                F(s8, &ClassInfo_c79, 8) = RoomInfo_c79[i].x0;
                F(s8, &lb_sys, 0x73) = i;
                break;
            }
        }
        (*st)++;
        CallBackWaitInit();
        CWX_c79->x2C45 = 0xF;
        cnLBS_RoomCreate(cnLbc_CheckInFloorOrder(2) & 0xFFFF, &CallBack_Result_Lobby_RoomCreate_o);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        *st = 0;
        return 0;
    case 3:
        *st = 0;
        return 1;
    }
    return 2;
}

void CallBack_Result_Lobby_RoomCreate(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xF)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x32C5) = 1;
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        F(u8, (u8 *)cw, 0x2C35) = 3U;
    }
}

#ifdef __MWERKS__
asm int Lbc_GetRoomRule()
{
#include "Lbc_GetRoomRule.inc"
}
#endif

void CallBack_Result_RuleAllocation(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x10) && (res.val != 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a1_2 = (u8 *)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x32C2) = 1;
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        *(s8 *)0x3F36AB = 0;
        F(s32, &lb_sys, 0x6C) = 1;
        F(s32, &lb_sys, 0x68) = 0x17;
        F(u8, (u8 *)cw, 0x2C35) = 0U;
        F(s8, &lb_sys, 6) = 0;
    }
}

s32 Lbc_SetRoomRule() {
    u8 buf[0x170];
    s32 i;
    u8 *stp;
    u8 st;

    st = CWX_c82->step;
    stp = &CWX_c82->step;
    switch (st) {
    case 0:
        *stp = 1;
        CallBackWaitInit();
        cw[0x2C45] = 0x11;
        memcpy(buf, (u8 *)&RoomRule + 0x12, 0x41);
        memcpy(buf + 0x41, (u8 *)&RoomRule + 2, 9);
        memcpy(buf + 0x4A, mhRule.msg, 0x3D);
        for (i = 0; i < RoomRule.n; i++) {
            buf[0x14B + i] = ((u8 *)&RoomRule)[i * 0x14A8 + 0x9A];
        }
        cnLBS_Set_RoomRule(buf, CallBack_Result_Lobby_SetRoomRule_o);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        To_EnterRoom();
        return 1;
    }
    return 0;
}

void CallBack_Result_Lobby_SetRoomRule() {
    if ((((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C45 == 0x11)) {
        ((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C45 = 0U;
        ((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C35 + 1);
        F(s8, (u8 *)cw, 0x35D3) = 1;
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

char *GetRoomRule() {
    return (char *)&RoomRule;
}

s32 Lbc_GuestReadRoom(s32 arg0) {
    u8 buf[0x294B0];
    s32 off;
    s32 r;
    u8 st;
    u8 *stp;
    s32 j;
    u8 t8;
    u8 *s;
    u8 *d;
    u8 *s2;
    u8 *d2;
    u8 *jm;
    s32 i;

    st = CWX_c85->step;
    stp = &CWX_c85->step;
    switch (st) {
    case 0:
        *stp = st + 1;
        memset(&RoomRule, 0, 0x29555);
        off = (arg0 & 0xFFFF) * 0x15C;
        strcpy((char *)&RoomRule + 0x12, (char *)RoomInfo_c85 + off + 0x14);
        ((u8 *)&RoomRule)[0x52] = RoomInfo_c85[off + 0x11];
        break;
    case 1:
        *stp = st + 1;
        CallBackWaitInit();
        cw[0x2C45] = 0x13;
        cnLBS_Read_RoomRuleAllocation(((arg0 & 0xFFFF) + 1) & 0xFFFF, CallBack_Result_Lobby_GuestRuleAllocation_o, 0x13);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        *stp = st + 1;
        r = (arg0 & 0xFFFF) + 1;
        cnLBS_Get_RoomRuleAllocation(r, buf);
        *(u8 *)&RoomRule = buf[0];
        ((u8 *)&RoomRule)[1] = buf[1];
        RoomRule.n = buf[3];
        i = 0;
        if (0 < RoomRule.n) {
            s = buf;
            d = (u8 *)&RoomRule;
            do {
                strcpy(d + 0x56, s + 5);
                d[0x98] = s[0x46];
                d[0x97] = s[0x47];
                t8 = s[0x48];
                d[0x9A] = t8;
                d[0x99] = t8;
                j = 0;
                if (0 < d[0x97]) {
                    s2 = s;
                    d2 = d;
                    do {
                        strcpy(d2 + 0xBD, s2 + 0x69);
                        j++;
                        s2 += 0x41;
                        d2 += 0x41;
                    } while (j < d[0x97]);
                }
                i++;
                s += 0x14A5;
                d += 0x14A8;
            } while (i < RoomRule.n);
        }
        CallBackWaitInit();
        cw[0x2C45] = 0x14;
        cnLBS_Read_RoomMemberList(r, CallBack_Result_Lobby_GuestRoomMember_o);
        break;
    case 4:
        Check_CallBackWait();
        break;
    case 5:
        *stp = 0;
        memset(join_member, 0, 0xBF0);
        i = 0;
        jm = join_member;
        do {
            cnLBS_Get_RoomMemberList(i & 0xFFFF, jm + 0x280, jm + 0x288, jm + 0x29A);
            i++;
            jm += 0x2FC;
        } while (i < 4);
        cw[0x2C3A] = 1;
        cw[0x32BF] = 3;
        F(s8, pNet, 6) = 2;
        return 0;
    }
    return 2;
}

void CallBack_Result_Lobby_GuestRuleAllocation(CNET_RES res) {
    int temp_a0;
    int temp_a1;

    temp_a1 = (int)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x13) && (res.val != 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
        } else if (res.val == -1) {
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            F(u8, (u8 *)cw, 0x2C35) = 0U;
            F(s8, &lb_sys, 6) = 0;
        }
    }
}

void CallBack_Result_Lobby_GuestRoomMember() {
    if ((((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C45 == 0x14)) {
        ((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C45 = 0U;
        ((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_GuestRoomMember *)cw)->x2C35 + 1);
    }
}

s32 Lbs_GuestEnterRoom() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x15;
        F(s8, &ClassInfo_c88, 8) = (s8) (F(u8, &lb_sys, 0x73) + 1);
        cnLBS_RoomEntry(cnLbc_CheckInFloorOrder(2) & 0xFFFF, &RoomRule_c88[2], &CallBack_Result_Lobby_RoomEntry_o);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        To_EnterRoom();
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

void CallBack_Result_Lobby_RoomEntry(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x15)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a1_2 = (u8 *)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x35D3) = 1;
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}

void lbc_in_lobby_03() {
    ((int (**)())lbc_in_lobby_03_jmp_2430)[F(u8, (u8 *)cw, 0x2C34)]();
}

void To_LobbyExit(s32 arg0) {
    s32 temp_v0;
    s32 var_a0;

    var_a0 = arg0;
    temp_v0 = var_a0 & 0xFF;
    if (temp_v0 != 3) {
        var_a0 = 2;
        switch (temp_v0) {                          /* irregular */
        case 0:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 1;
            F(s8, (u8 *)cw, 0x2C35) = 2;
            break;
        case 1:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 1;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            break;
        case 2:
            F(s8, (u8 *)cw, 0x2C31) = 3;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 3;
            F(s8, (u8 *)cw, 0x2C34) = 2;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            break;
        }
    } else {
        F(s8, (u8 *)cw, 0x2C31) = 3;
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 3;
        F(s8, (u8 *)cw, 0x2C34) = 2;
        F(s8, (u8 *)cw, 0x2C35) = 2;
    }
    Init_InterruptFlag(var_a0);
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(u8, (u8 *)cw, 0x2F79) = 0xFF;
    F(s8, (u8 *)cw, 0x2F78) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    cnLbc_EraseDialog(0x4C);
    SoftKeyboard_exit();
    F(s8, (u8 *)cw, 0x2C0A) = 2;
    F(s8, (u8 *)cw, 0x2C0B) = 2;
}

void lbc_in_lobby_03_00() {
    u16 sw;
    sw = Get_sw2(0);
    switch (CWX_c92->x2C35) {
    case 0:
        CWX_c92->x2C35++;
        CWX_c92->x2C4C = 0;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1);
        return;
    case 1:
        if (CWX_c92->x2C4C++ >= 0x258 || (CWX_c92->x2C4C >= 0x3C && (sw & 0x60))) {
            switch (CWX_c92->x32BF) {
            case 0:
                CWX_c92->x2C31 = 3;
                CWX_c92->x2C32 = 0;
                CWX_c92->x2C33 = 0;
                CWX_c92->x2C34 = 1;
                CWX_c92->x2C35 = 0;
                break;
            case 1:
                CWX_c92->x2C31 = 3;
                CWX_c92->x2C32 = 0;
                CWX_c92->x2C33 = 0;
                CWX_c92->x2C34 = 1;
                CWX_c92->x2C35 = 0;
                break;
            case 2:
                CWX_c92->x2C31 = 3;
                CWX_c92->x2C32 = 0;
                CWX_c92->x2C33 = 0;
                CWX_c92->x2C34 = 1;
                CWX_c92->x2C35 = 0;
                break;
            case 3:
                CWX_c92->x2C31 = 3;
                CWX_c92->x2C32 = 0;
                CWX_c92->x2C33 = 0;
                CWX_c92->x2C34 = 1;
                CWX_c92->x2C35 = 0;
                break;
            }
            cnLbc_EraseDialog(0x4C);
        }
    }
}

void lbc_in_lobby_03_01() {
    u8 t;
    t = CWX_c93->x2C35;
    switch (t) {
    case 0:
        CWX_c93->x2C35 = t + 1;
        Lbc_init_network_work();
        CWX_c93->x2C4C = 0x26;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        F(s8, pNet, 0xC) = 1;
        CWX_c93->x2C4C--;
        if (CWX_c93->x2C4C <= 0) {
            CWX_c93->x2C35++;
        case 2:
            CWX_c93->x2C35++;
            CallBackWaitInit();
            CWX_c93->x2C45 = 0x16;
            cnLBS_LobbyExit(&CallBack_Result_Lobby_LobbyExit_o);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        fade_set(1);
        CWX_c93->x2C35++;
        break;
    case 5:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX_c93->x35D5 = 0;
            Lbc_set_prim_k(0, 0, 0);
            To_EnterPlaza2Lobby();
        }
        break;
    }
}

s32 Lbc_SendMiniData() {
    char sp10[0x20];

    Lb_set_mini_data(sp10);
    if (memcmp(sp10, &my_user_mini_data, 0x40) == 0) {
        return 0;
    }
    memcpy((s32)(game_w.master * 0x2FC) + cw + 0x1346, sp10, 0x40);
    memcpy((s32)cw + 0x45A, sp10, 0x40);
    memcpy(&my_user_mini_data, sp10, 0x40);
    memcpy((int)&lbCommer + (game_w.master * 0x5C) + 0x1C, sp10, 0x40);
    cnLBS_Send_UserMiniData(sp10, 0x40, &CallBack_Result_SendUserMiniData_o);
    F(s8, &lb_sys, 0x78) = 0;
    return 0;
}

void CallBack_Result_SendUserMiniData() {

}

void CallBack_NoticeUserMiniData(CNET_RES res) {
    LUSER u;
    s32 i;
    u8 *p;

    cnLBS_Get_NoticeUserMiniData(&u);
    if (softdip_ck(0xF1) == 1) {
        lb_check_mini_data((s8)Lb_get_plID(&u), &u, u.mini);
        return;
    }
    i = 0;
    p = (u8 *)lbCommer;
    do {
        if (memcmp(p, &u, 8) == 0) {
            memcpy((u8 *)&lbCommer[(s8)i] + 0x1C, u.mini, 0x40);
            return;
        }
        i = (s8)(i + 1);
        p += 0x5C;
    } while (i < 8);
}

s32 Lbs_LobbyExit() {
    u8 temp_v1;

    temp_v1 = F(u8, (u8 *)cw, 0x2C35);
    switch (temp_v1) {                              /* irregular */
    case 0:
        Lbc_init_network_work();
        fade_set(1);
        ((CWS_Lbs_LobbyExit *)cw)->x2C35 = (u8) (((CWS_Lbs_LobbyExit *)cw)->x2C35 + 1);
        /* fallthrough */
    case 1:
        ((CWS_Lbs_LobbyExit *)cw)->x2C35 = (u8) (((CWS_Lbs_LobbyExit *)cw)->x2C35 + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x16;
        cnLBS_LobbyExit(&CallBack_Result_Lobby_LobbyExit_o);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            F(u8, (u8 *)cw, 0x2C35) = 0U;
            return 1;
        }
        break;
    }
    return 0;
}

void CallBack_Result_Lobby_LobbyExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C45 == 0x16)) {
        ((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            ((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C35 + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        cnLbc_EraseDialog(0x4C);
    }
}

void lbc_in_lobby_03_02() {
    switch (CWX_c99->x2C35) {
    case 0:
        Lbc_init_network_work();
        CWX_c99->x2C35++;
        CWX_c99->x2C4C = 0x44;
        nwSetEff_FreeDialog(0x14, (u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        CWX_c99->x2C4C--;
        if (CWX_c99->x2C4C <= 0) {
        case 2:
            CWX_c99->x2C35++;
            CallBackWaitInit();
            CWX_c99->x2C45 = 0x18;
            cnLBS_RoomExit(&CallBack_Result_Lobby_RoomExit_o);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
    }
}

s32 Lbs_InRoomCheck() {
    return F(u8, (u8 *)cw, 0x35D3) != 0;
}

s32 Lbs_RoomExit() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        /* fallthrough */
    case 1:
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C35) = (u8) (F(u8, temp_v1_3, 0x2C35) + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x18;
        cnLBS_RoomExit(&CallBack_Result_Lobby_RoomExit_o);
        break;
    case 2:
        Check_CallBackWait(temp_a0);
        break;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 0;
}

void CallBack_Result_Lobby_RoomExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C45 == 0x18)) {
        ((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x35D3) = 0;
            ((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C35 + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x35D3) = 0;
        To_LogOut(4);
    }
}

void To_EnterRoom() {
    int temp_s0;

    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    F(s16, (u8 *)cw, 0x32C8) = 0;
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s8, (u8 *)cw, 0x32C1) = 1;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    F(s16, (u8 *)cw, 0x32CA) = 0;
    temp_s0 = F(u8, &ClassInfo_c103, 8);
    cnLBS_Read_RoomMemberList(temp_s0, &CallBack_Result_InRoom00_Member_o);
    cnLBS_Read_RoomJoinUser(temp_s0, &CallBack_Result_InRoom00_JoinUser_o);
    cnLBS_Read_MatchEntryJoinUser(temp_s0, &CallBack_Result_InRoom00_EntryJoinUser_o);
}

void CallBack_Result_InRoom00_Member(CNET_RES res) {
    int i;
    int off;

    if (((CWS_im *)cw)->x2C31 != 5) {
        memset(cw + 0x4BC, 0, 0xBF0);
        if (res.val == 0) {
            i = 0;
            off = 0;
            do {
                u8 *p = cw + off;
                cnLBS_Get_RoomMemberList((u16)i, p + 0x73C, p + 0x744, p + 0x756);
                i++;
                off += 0x2FC;
            } while (i < 4);
        }
    }
}

void CallBack_Result_InRoom00_JoinUser(CNET_RES res) {
    int n;

    if (F(u8, cw, 0x2C31) != 5) {
        n = ClassInfo_c105[8];
        if (res.val == 0) {
            cnLBS_Get_mhRoomJoinUser(n & 0xFFFF, (u8 *)&RoomInfo_c105[ClassInfo_c105[8] - 1] + 2, (u8 *)&RoomInfo_c105[ClassInfo_c105[8] - 1] + 0xE);
            return;
        }
        *(s16 *)((int)(RoomRule_c105 + 0x29406) + ClassInfo_c105[8] * 0x15C) = 0;
    }
}

void CallBack_Result_InRoom00_EntryJoinUser(CNET_RES res) {
    u8 *temp_v0;
    if (res.val == 0) {
        temp_v0 = (u8 *)cw;
        cnLBS_Get_MatchEntryJoinUser(F(u8, &ClassInfo_c106, 8), temp_v0 + 0x32C6, temp_v0 + 0x32C8);
        return;
    }
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s16, (u8 *)cw, 0x32C8) = 0;
}

s32 Lbs_MatchEntryCancel() {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x1D;
        cnLBS_MatchEntry(0, &CallBack_Result_InRoom_MatchEntryCancel_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}

void CallBack_Result_InRoom_MatchEntryCancel(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x1D)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x35D4) = 0;
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        F(u8, (u8 *)cw, 0x2C35) = 3U;
    }
}

s32 Lbs_MatchEntry() {
    u8 temp_v1;
    int temp_a0;
    int temp_a1;

    temp_a0 = (int)cw;
    temp_v1 = F(u8, temp_a0, 0x2C35);
    temp_a1 = temp_a0 + 0x2C35;
    switch (temp_v1) {                              /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C35) = (u8) (temp_v1 + 1);
        CallBackWaitInit(temp_a0, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 0x1C;
        cnLBS_MatchEntry(1, &CallBack_Result_InRoom_MatchEntry_o);
        break;
    case 1:
        Check_CallBackWait(temp_a0, temp_a1);
        break;
    case 2:
        *(u8 *)0x3F360A = F(u8, temp_a0, 0x32C5);
        F(u8, temp_a0, 0x2C35) = 0U;
        return 0;
    case 3:
        return 1;
    }
    return 2;
}

void CallBack_Result_InRoom_MatchEntry(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x1C)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a1_2 = (u8 *)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x35D4) = 1;
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 2);
    }
}

s32 Lbc_SetPropaty(arg1)
int arg1;
{
    switch (F(u8, (u8 *)cw, 0x2C35)) {
    case 0:
        F(u8, (u8 *)cw, 0x2C35)++;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x1B;
        cnLBS_Set_RoomProperty(arg1, CallBack_Result_SetRoomPropaty);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 0;
    case 3:
        F(u8, (u8 *)cw, 0x2C35) = 0;
        return 1;
    }
    return 2;
}

void CallBack_Result_SetRoomPropaty(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x1B)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        F(u8, (u8 *)cw, 0x2C35) = 3U;
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int Lbs_MatchStart() {
    cnLBS_MatchStart();
    return 1;
}

void lobby_client_game_ready() {
    if (Check_InterruptFlag() == 0) {
        ((int (**)())&lobby_client_game_ready_jmp_2838)[F(u8, (u8 *)cw, 0x2C33)]();
        Disp_NowLoading2(4);
    }
}

void To_ReadyBattle() {
    F(s8, (u8 *)cw, 0x2C31) = 4;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag();
    F(s8, (u8 *)cw, 0x2C30) = 0;
    F(s8, (u8 *)cw, 0x35D5) = 0;
    cnLbc_EraseDialog(0x4C);
}

void lbc_game_ready_00() {
    CNET_W5D4 mi;
    u8 *e;
    s32 o;
    char *id;
    char *hd;
    char *mn;
    s32 i;
    u8 st;
    u8 *stp;

    st = CWX_c115->x2C34;
    stp = &CWX_c115->x2C34;
    switch (st) {
    case 0:
        *stp = st + 1;
        fade_set(2);
    case 1:
        CWX_c115->x2C34++;
    case 2:
        CWX_c115->x2C34++;
        CallBackWaitInit();
        CWX_c115->x2C45 = 0x20;
        cnLBS_Read_MatchInfomation(CallBack_Result_Match_MatchInformation_o);
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        CWX_c115->x2C33++;
        CWX_c115->x2C34 = 0;
        cnLBS_Get_MatchInfomation(&mi);
        CWX_c115->x2C47 = *(u8 *)&mi;
        USER_PL_ID = *((u8 *)&mi + 1);
        memcpy(cw + 0x35E0, (u8 *)&mi + 2, 0x10);
        i = 0;
        if (0 < CWX_c115->x2C47) {
            e = (u8 *)&mi;
            o = 0;
            id = room_member_id;
            hd = room_member_handle;
            mn = room_member_mini_data;
            do {
                strcpy(cw + o + 0x73C, e + 0x114);
                strcpy(cw + o + 0x744, e + 0x11C);
                memcpy(cw + o + 0x756, e + 0x130, 0x40);
                memcpy(id, e + 0x114, 8);
                memcpy(hd, e + 0x11C, 0x11);
                memcpy(mn, e + 0x130, 0x40);
                i++;
                id += 8;
                hd += 0x11;
                mn += 0x40;
                *(s8 *)(cw + o + 0x796) = *(u16 *)(e + 0x190);
                *(s32 *)(cw + o + 0x7B4) = *(s32 *)(e + 0x194);
                e += 0x98;
                o += 0x2FC;
            } while (i < CWX_c115->x2C47);
        }
        break;
    }
}

void CallBack_Result_Match_MatchInformation(CNET_RES res) {
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_a2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (temp_a0 = F(u8, temp_a1, 0x2C45), temp_a2 = temp_a1 + 0x2C45, (temp_a0 == 0x20))) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            all_reset();
            temp_a0_2 = (u8 *)cw;
            F(u8, temp_a0_2, 0x2C34) = (u8) (F(u8, temp_a0_2, 0x2C34) + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x2C0D) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a2);
    }
}

void lbc_game_ready_01() {
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    F(s8, (u8 *)cw, 0x2C34) = 0;
}

void lbc_game_ready_02(int arg0, int arg1, s32 arg2) {
    s32 var_a2;
    s32 var_a3;
    u8 temp_v0;
    u8 temp_v1_2;
    int temp_a0;
    int temp_v1;

    var_a2 = arg2;
    temp_v1 = (int)cw;
    temp_v1_2 = F(u8, temp_v1, 0x2C34);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C34) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C48) = 0;
        F(s8, (u8 *)cw, 0x2C49) = 0;
        var_a3 = 0;
        if (0 < F(u8, (u8 *)cw, 0x2C47)) {
            var_a2 = 0;
            do {
                temp_v0 = F(u8, ((u8 *)cw + var_a2), 0x7B1);
                if (temp_v0 == 1) {
                    F(u8, (u8 *)cw, 0x2C48) = (u8) (F(u8, (u8 *)cw, 0x2C48) + 1);
                } else if (temp_v0 == 2) {
                    F(u8, (u8 *)cw, 0x2C49) = (u8) (F(u8, (u8 *)cw, 0x2C49) + 1);
                }
                var_a3 += 1;
                var_a2 += 0x2FC;
            } while (var_a3 < F(u8, (u8 *)cw, 0x2C47));
        }
        cnLbc_LoadNetModel(2, (u8 *)cw, var_a2, var_a3);
        net_char_change = 1;
        return;
    case 1:
        if (cnLbc_LoadModelWait(2, temp_v1 + 0x2C34) != 0) {
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
            F(u8, (u8 *)cw, 0x2C34) = 0U;
        }
        break;
    }
}

void lbc_game_ready_03() {
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    F(s8, (u8 *)cw, 0x2C34) = 0;
}

void lbc_game_ready_04() {
    u8 temp_a1;
    int temp_a2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;

    temp_v1 = (int)cw;
    temp_a1 = F(u8, temp_v1, 0x2C34);
    temp_a2 = temp_v1 + 0x2C34;
    switch (temp_a1) {
    case 0:
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        cnWrap_BgmFadeOut(0xF, temp_a1, temp_a2);
        /* fallthrough */
    case 1:
        temp_v1_2 = (int)cw;
        F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 1);
        /* fallthrough */
    case 2:
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C34) = (u8) (F(u8, temp_v1_3, 0x2C34) + 1);
        /* fallthrough */
    case 3:
        temp_v1_4 = (int)cw;
        F(u8, temp_v1_4, 0x2C34) = (u8) (F(u8, temp_v1_4, 0x2C34) + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x21;
        cnLBS_LogoutLobbyServer(&CallBack_Result_Match_Logout_o);
        return;
    case 4:
        Check_CallBackWait();
        return;
    case 5:
        F(s32, &CnetWork, 8) = 0;
        net_game_invalid_flag = 1;
        COM_R_No_1_c120 = 0;
        F(s8, &CnetWork, 5) = 3;
        COM_R_No_0 = 4;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        COM_R_No_4 = 0;
        COM_R_No_5 = 0;
        COM_R_No_6 = 0;
        netr_ret = 0;
        F(s32, &CnetWork, 0x10) = System_timer;
        /* fallthrough */
    default:
        return;
    }
}

void CallBack_Result_Match_Logout() {
    if (CWX_c121->x31 != 5 && CWX_c121->x45 == 0x21) {
        CWX_c121->x45 = 0;
        CWX_c121->x34++;
    }
}

void lobby_client_matching_failed() {
    ((int (**)())lobby_client_matching_failed_jmp_2996)[F(u8, (u8 *)cw, 0x2C33)]();
}

void To_MatchingFailed(arg0)
int arg0;
{
    F(s8, (u8 *)cw, 0x2C31) = 6;
    F(s8, (u8 *)cw, 0x2C32) = arg0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    Init_InterruptFlag(arg0);
}

void lbc_matching_failed_00() {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C33) = (u8) (F(u8, temp_v1, 0x2C33) + 1);
    cnWrap_BgmVolume(0);
    Lbc_init_network_work();
    Lbc_set_prim_k(&put_back, 0, 0);
    fade_set(0xA);
}

void lbc_matching_failed_01() {
    int temp_a0;
    int temp_v1;

    temp_a0 = (int)cw;
    F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    F(s32, (u8 *)cw, 0x2C4C) = 0;
    F(s8, pNet, 0xC) = 1;
    temp_v1 = (int)cw;
    if (F(u8, temp_v1, 0x2C32) == 0) {
        SetDialogData(0x33, 3);
    } else {
        SetDialogData_HTML(temp_v1 + 0x32D1);
    }
    F(s32, (u8 *)cw, 0x2C4C) = 0x14;
    fade_set(2);
}

void lbc_matching_failed_02() {
    s32 temp_a0;
    s32 temp_a2;
    void *temp_a1;
    void *temp_v1;

    temp_a2 = Get_sw2(0) & 0xFFFF;
    F(s8, pNet_c126, 0xC) = 1;
    temp_a1 = (u8 *)cw;
    temp_a0 = F(s32, temp_a1, 0x2C4C);
    F(s32, temp_a1, 0x2C4C) = (temp_a0 + 1);
    if ((temp_a0 >= 0x12C) || ((F(s32, (u8 *)cw, 0x2C4C) >= 0x3C) && ((u16)temp_a2 & 0x60))) {
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C33) = (u8) (F(u8, temp_v1, 0x2C33) + 1);
        fade_set(1, temp_a1, temp_a2);
    }
}

void lbc_matching_failed_03() {
    void *temp_a0;

    F(s8, pNet_c127, 0xC) = 1;
    if ((Fade_busy_ck() & 0xFF) != 1) {
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    }
}

void lbc_matching_failed_04() {
    void *temp_a0;

    if (net_char_change_c127 != 0) {
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        return;
    }
    all_reset();
    Lbs_load();
    F(s8, pNet_c127, 0x11) = 1;
    To_GoToTop();
    cnWrap_BgmRequest(0);
}

void lbc_matching_failed_05() {
    if (cnLbc_LoadModelWait(1) != 0) {
        F(s8, (u8 *)cw, 0x2C08) = 1;
        cnWrap_ScreenReset();
        all_reset();
        Lbs_load();
        To_GoToTop();
        F(s8, pNet_c127, 0x11) = 1;
        cnWrap_BgmRequest(0);
    }
}

void To_LogOut(arg0)
u8 arg0;
{
    void *temp_a0;
    void *temp_a2;

    F(s8, (u8 *)cw, 0x2C31) = 5;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = arg0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    temp_a2 = (u8 *)cw;
    F(s8, temp_a2, 0x2C46) = arg0;
    temp_a0 = (u8 *)cw;
    F(s16, temp_a0, 0x35FC) = -1;
    if (arg0 == 7) {
        F(s8, &CnetWork, 6) = 1;
        cnLbc_Set_NgServerId(temp_a0, -1, temp_a2, 5);
        return;
    }
    cnLbc_Init_NgServerId(temp_a0, -1, temp_a2, 5);
}

void Lbs_LogOutRequest() {
    To_LogOut(1);
}

void Set_ErrorDialog(int arg0) {
    int temp_v1;

    temp_v1 = (s8)arg0;
    switch (temp_v1) {
    case 1:
        SetDialogData(9, 5);
        return;
    case 3:
        SetDialogData(8, 3);
        return;
    case 4:
    case 6:
        SetDialogData(0xA, 3);
        return;
    case 5:
    case 7:
        SetDialogData_HTML((s32)cw + 0x32D1);
        /* fallthrough */
    default:
        return;
    }
}

void lobby_client_logout() {
    ((int (**)())lobby_client_logout_jmp_3078)[F(u8, (u8 *)cw, 0x2C33)]();
}

void lbc_logout_00() {
    switch (CWX_c132->x2C34) {
    case 0:
        Lbc_init_network_work();
        if (CWX_c132->x2C46 != 0) {
            CWX_c132->x2C34++;
            Lbc_init_network_work();
            Lbc_set_prim_k(&put_back, 0, 0);
            cnWrap_BgmFadeOut(0xF);
            fade_set(0xA);
            break;
        }
        CWX_c132->x2C34 = 3;
        break;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            if (CWX_c132->x2C46 != 0) {
                str_stop(0);
                str_stop(1);
                if (*((u8 *)&D_3E4C05 + game_w.master * 0xA00) != 0x34) {
                    fade_set(2);
                    cnWrap_BgmStop();
                    Pit_reset();
                    CWX_c132->x2C34 += 2;
                } else {
                    CWX_c132->x2C34 += 2;
                }
            } else {
                CWX_c132->x2C34 += 2;
            }
            SoftKeyboard_exit();
        }
        break;
    case 2:
        CWX_c132->x2C34++;
        break;
    case 3:
        CWX_c132->x2C34++;
        tk_logout_init();
        SoftKeyboard_exit();
        if (CWX_c132->x2C46 == 3) {
            SetDialogData(8, 0);
            fade_set(2);
            break;
        }
        if (CWX_c132->x2C46 != 0) {
            fade_set(2);
            SetDialogData(7, 5);
        }
        break;
    case 4:
        CWX_c132->x2C34++;
    case 5:
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(CWX_c132->x2C46) != 0) {
            CWX_c132->x2C34++;
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
        }
        break;
    case 6:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX_c132->x2C34++;
            if (CWX_c132->x2C46 != 0) {
                F(s8, pNet, 0xC) = 1;
                fade_set(1);
            }
        }
        break;
    case 7:
        if ((Fade_busy_ck() & 0xFF) == 1) {
            if (CWX_c132->x2C46 != 0) {
                F(s8, pNet, 0xC) = 1;
            }
            return;
        }
        if (CWX_c132->x2C46 == 0) {
            game_w.step = 1;
            COM_R_No_0 = 0;
            COM_R_No_1_c132 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
        } else if (CWX_c132->x2C46 == 2) {
            game_w.step = 4;
            COM_R_No_0 = 1;
            COM_R_No_1_c132 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        } else if (CWX_c132->x2C46 == 1) {
            game_w.step = 4;
            COM_R_No_1_c132 = 0;
            COM_R_No_0 = 4;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        }
        break;
    }
}

void lbc_logout_01() {
    s32 sw;

    sw = Get_sw2(0) & 0xFFFF;
    switch (CWX_c133->x2C34) {
    case 0:
        Lbc_init_network_work();
        F(s8, pNet, 0x11) = 1;
        CWX_c133->x2C34++;
        Lbc_set_prim_k(&text_lobby_trans_ot0_o, 0, 0);
        if (CWX_c133->x2C46 != 0) {
            cnWrap_BgmFadeOut(0xF);
        }
        if (net_char_change_c133 != 0) {
            fade_set(0xA);
            break;
        }
        CWX_c133->x2C34 = 4;
        fade_set(0xA);
        break;
    case 1:
        F(s8, pNet, 0x11) = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX_c133->x2C34++;
            if (CWX_c133->x2C46 != 0) {
                str_stop(0);
                str_stop(1);
            }
            SoftKeyboard_exit();
        }
        break;
    case 2:
        F(s8, pNet, 0x11) = 1;
        CWX_c133->x2C34++;
        CWX_c133->x2C08 = 0;
        break;
    case 3:
        F(s8, pNet, 0x11) = 1;
        CWX_c133->x2C08 = 1;
        CWX_c133->x2C34++;
        break;
    case 4:
        F(s8, pNet, 0x11) = 1;
        if ((Fade_busy_ck() & 0xFF) != 1) {
            CWX_c133->x2C34++;
            tk_logout_init();
            fade_set(2);
            SetDialogData(7, 5);
            SoftKeyboard_exit();
        }
        break;
    case 5:
        F(s8, pNet, 0xC) = 1;
        CWX_c133->x2C34++;
        break;
    case 6:
        F(s8, pNet, 0xC) = 1;
        CWX_c133->x2C34 = 7;
    case 7:
        F(s8, pNet, 0xC) = 1;
        if (tk_logout(CWX_c133->x2C46, sw) != 0) {
            Set_ErrorDialog_k((s8)CWX_c133->x2C46);
            if (CWX_c133->x2C46 == 7) {
                CWX_c133->x2C34 = 9;
                CWX_c133->x2C4C = 0x5A;
            } else {
                CWX_c133->x2C34++;
            }
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
        }
        break;
    case 8:
        F(s8, pNet, 0xC) = 1;
        if ((u16)sw & 0x20) {
            CWX_c133->x2C34 = 0xA;
            cnWrap_SoundRequest(0);
            fade_set(1);
        }
        break;
    case 9:
        F(s8, pNet, 0xC) = 1;
        if (--CWX_c133->x2C4C <= 0) {
            CWX_c133->x2C34++;
            fade_set(1);
        }
        break;
    case 10:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            all_reset();
            F(s8, pNet, 0x11) = 1;
            *(s8 *)0x3F33F1 = 4;
            COM_R_No_0 = 1;
            COM_R_No_1_c133 = 0;
            COM_R_No_2 = 0;
            COM_R_No_3 = 0;
            COM_R_No_4 = 0;
            MMBB_LOGIN = 0;
        }
        break;
    }
}

void To_GoToTop() {
    F(s8, (u8 *)cw, 0x2C31) = 7;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C36) = 0;
}

void lobby_client_goto_top() {
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 0x22;
        cnLBS_TopPageJump(&CallBack_Result_GotoTop_o);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, temp_a1);
    }
}

void CallBack_Result_GotoTop(CNET_RES res) {
    if ((((CWS_CallBack_Result_GotoTop *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_GotoTop *)cw)->x2C45 == 0x22)) {
        ((CWS_CallBack_Result_GotoTop *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            F(u8, (u8 *)cw, 0x2C31) = 1U;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 0;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            return;
        }
        To_LogOut(4);
    }
}

#ifdef __MWERKS__
asm int lobby_client_admin_message()
{
#include "lobby_client_admin_message.inc"
}
#endif

void lbc_admin_message_00() {
    char sp10[0x20];
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;
    int temp_a0_3;
    int temp_a1_2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2F6F;
    temp_a0_2 = F(u8, temp_a0, 0x2F6F);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2F6F) = (u8) (temp_a0_2 + 1);
        Lbc_init_network_work();
        Info_Initialization();
        /* fallthrough */
    case 1:
        temp_a0_3 = (int)cw;
        F(u8, temp_a0_3, 0x2F6F) = (u8) (F(u8, temp_a0_3, 0x2F6F) + 1);
        F(s8, (u8 *)cw, 0x2C5C) = 2;
        SetDialogData_HTML((u8 *)cw + 0x2C6E);
        /* fallthrough */
    case 2:
        temp_a1_2 = (int)cw;
        F(u8, temp_a1_2, 0x2F6F) = (u8) (F(u8, temp_a1_2, 0x2F6F) + 1);
        F(s16, (u8 *)cw, 0x2F74) = 0x258;
        return;
    case 3:
        F(s8, pNet, 0xC) = 1;
        temp_v1 = (int)cw;
        F(s16, temp_v1, 0x2F74) = (s16) (F(s16, temp_v1, 0x2F74) - 1);
        temp_v1_2 = (int)cw;
        if (F(s16, temp_v1_2, 0x2F74) == 0) {
            F(u8, temp_v1_2, 0x2F6E) = (u8) (F(u8, temp_v1_2, 0x2F6E) + 1);
            F(u8, (u8 *)cw, 0x2F6F) = 0U;
            F(s16, (u8 *)cw, 0x2F74) = 0;
        }
        font_set_stack_no(3);
        cnWrap_SetFontColor(0);
        cnWrap_SetFontSize(20.0f);
        temp_v1_3 = (int)cw;
        sprintf(sp10, &lit_3323, F(s16, temp_v1_3, 0x2F74) / 60);
        cnWrap_FontDisp(460.0f, 80.0f, 2.0f, sp10);
    }
}

void lbc_admin_message_01() {
    u16 sw;
    u8 st;
    u8 *stp;
    CWS_am1 *c;
    sw = Get_sw2(0);
    c = CWX_c138;
    st = c->x2F6F;
    stp = &c->x2F6F;
    switch (st) {
    case 0:
        if (sw & 0x20) {
            *stp = st + 1;
            cnWrap_SoundRequest(0);
            CWX_c138->x2F74 = 8;
            cnLbc_EraseDialog(0x4C);
        }
        Lb_put_inputMsgForHTML();
        return;
    case 1:
        CWX_c138->x2F74--;
        if (CWX_c138->x2F74 < 0) {
            CWX_c138->x2F76 = 1;
            cnLBS_AnswerAdminMessage(1);
            CWX_c138->x2F6E++;
            CWX_c138->x2F6F = 0;
        }
    }
}

void lbc_admin_message_02() {
    F(s8, (u8 *)cw, 0x2C5C) = 0;
    F(s8, (u8 *)cw, 0x2F6E) = 0;
    F(s8, (u8 *)cw, 0x2F6F) = 0;
    F(s8, (u8 *)cw, 0x2F76) = 0;
    F(s8, (u8 *)cw, 0x2F72) = 0;
}

void Init_InterruptFlag() {
    memset((u8 *)cw + 0x2C09, 0, 3);
    F(s8, (u8 *)cw, 0x2C0C) = 0;
    F(s8, (u8 *)cw, 0x2C0D) = 0;
    F(s8, (u8 *)cw, 0x2C45) = 0;
    cnLBS_Init_LobbyBgProcess();
    cnLBS_Init_LobbyBgBurstProcess();
}

s32 Lbs_CheckMatchingFlag() {
    return F(s8, (u8 *)cw, 0x2C0C) != 0;
}

s32 Check_InterruptFlag() {
    s8 temp_v1;
    u8 temp_v1_2;
    int temp_a1;

    temp_a1 = (int)cw;
    if (F(s8, temp_a1, 0x2C08) != 0) {
        if (F(s8, temp_a1, 0x2C0C) != 0) {
            To_ReadyBattle();
            return 1;
        }
        temp_v1 = F(s8, temp_a1, 0x2C0D);
        if (temp_v1 == 1) {
            To_MatchingFailed(0, temp_a1);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            return 1;
        }
        if (temp_v1 == 2) {
            To_MatchingFailed(1, temp_a1);
            return 1;
        }
        temp_v1_2 = F(u8, temp_a1, 0x2C31);
        if (temp_v1_2 == 2) {
            if (F(s8, temp_a1, 0x2C09) == 1) {
                To_PlazaExit_k(1, temp_a1);
                return 1;
            }
            goto block_20;
        }
        if ((temp_v1_2 == 3) && (F(u8, temp_a1, 0x35D5) == 1)) {
            if (F(u8, temp_a1, 0x35D3) == 0) {
                if (F(s8, temp_a1, 0x2C0A) == 1) {
                    To_LobbyExit_k(1, temp_a1);
                    return 1;
                }
                goto block_20;
            }
            if (F(s8, temp_a1, 0x2C0B) == 1) {
                SetDialogData_HTML(temp_a1 + 0x32D1, temp_a1);
                *(s8 *)0x3F36AB = 0;
                F(s32, &lb_sys, 0x68) = 0x15;
                F(s32, &lb_sys, 0x6C) = 1;
                F(s8, (u8 *)cw, 0x2C45) = 0;
                F(s8, (u8 *)cw, 0x2C35) = 0;
                Lbc_init_network_work();
                Lbc_set_prim_k(0, 0, 0);
                Init_InterruptFlag();
                return 1;
            }
            goto block_20;
        }
        goto block_20;
    }
block_20:
    return 0;
}

void tk_logout_init() {
    COM_R_No_Logout = 0;
    COM_R_No_Disconnect = 0;
}

#ifdef __MWERKS__
static asm int tk_logout_message_sub()
{
#include "tk_logout_message_sub.inc"
}
#endif

typedef struct { u8 pad0000[0x2C45]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; } CWS_tk_logout;
#define TKCW ((CWS_tk_logout *)cw)

s32 tk_logout(arg0)
int arg0;
{
    s32 var_s0;
    u8 st;
    u8 cs;

    var_s0 = 0;
    cs = COM_R_No_Logout_c142;
    switch (cs) {
    case 0:
        COM_R_No_Logout_c142 = cs + 1;
        break;
    case 1:
        TKCW->x2C4C = 0x708;
        if (*(u8 *)0x3F35CC != 0) {
            switch (arg0 & 0xFF) {
            case 0:
            case 2:
            case 7:
                COM_R_No_Logout_c142 = COM_R_No_Logout_t + 1;
                TKCW->x2C45 = 0x26;
                cnLBS_LogoutLobbyServer(CallBack_Logout_ShutDown);
                break;
            case 1:
            case 3:
            case 4:
            case 5:
                COM_R_No_Logout_c142 = COM_R_No_Logout_t + 1;
                TKCW->x2C45 = 0x26;
                cnLBS_ShutDownLobbyServer(CallBack_Logout_ShutDown);
                break;
            case 6:
                COM_R_No_Logout_c142 = 3;
                break;
            }
        } else {
            COM_R_No_Logout_c142 = 3;
        }
        break;
    case 2:
        TKCW->x2C4C = TKCW->x2C4C - 1;
        if (CpInetGetStatus() != 0 || TKCW->x2C4C < 0) {
            COM_R_No_Logout_c142 = COM_R_No_Logout_c142 + 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 3:
        COM_R_No_Logout_c142 = COM_R_No_Logout_c142 + 1;
        if (*(u8 *)0x3F35CC != 0) {
            TKCW->x2C4C = 0x3C;
        } else {
            TKCW->x2C4C = 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 4:
        if (--TKCW->x2C4C < 0) {
            COM_R_No_Disconnect = 0;
            COM_R_No_Logout_c142 = COM_R_No_Logout_t + 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 5:
        switch (arg0 & 0xFF) {
        case 0:
        case 2:
        case 7:
            var_s0 = 1;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            if (disconnect() != 0) {
                var_s0 = 1;
            } else {
                tk_logout_message_sub(1, arg0);
            }
            break;
        }
        break;
    }
    return var_s0;
}

void CallBack_Logout_ShutDown() {
    u8 temp_a0_2;
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    if (F(u8, temp_a0, 0x2C45) == 0x26) {
        F(u8, temp_a0, 0x2C45) = 0U;
        temp_a0_2 = COM_R_No_Logout_c142;
        if (temp_a0_2 == 2) {
            COM_R_No_Logout_c142 = (u8) (temp_a0_2 + 1);
        }
    }
}

