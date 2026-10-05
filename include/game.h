#ifndef GAME_H
#define GAME_H
/* game_w (0x3F33F0, 0x224 bytes): global game/session state.
 * Offsets from matched code: master (Pl_master_ck, pl_sw_set). */
#include "types.h"

typedef struct V3S { s16 x, y, z; } V3S;

#ifndef PL_ITEM_DEFINED
#define PL_ITEM_DEFINED
/* One slot of an item list: item id (index into Item_data) and count (also in pl.h). */
typedef struct PL_ITEM {
    u16 id;
    s16 num;
} PL_ITEM;
#endif

typedef struct GAME_W {
    u8 mode;            /* 0x000 game mode (Game_task jumps on it) */
    u8 step;            /* 0x001 step inside the mode */
    u8 sub;             /* 0x002 sub-step (game12 loading sequence) */
    u8 x03;             /* 0x003 */
    s16 x04;            /* 0x004 counter (game3/game4) */
    s16 x06;            /* 0x006 counter (game3) */
    s16 x08;            /* 0x008 timer (gold_main/gold_init) */
    s16 x0A;            /* 0x00A counter (game5) */
    u8 _pad00C[0xD - 0xC];
    u8 pad_on;          /* 0x00D read controllers this frame (swset) */
    u8 _pad00E[0xF - 0xE];
    u8 x0F;             /* 0x00F copied from option_w+7 (game11) */
    u8 x10;             /* 0x010 */
    u8 x11;             /* 0x011 */
    u8 x12;             /* 0x012 */
    u8 _pad013[0x14 - 0x13];
    u8 stage;           /* 0x014 stage number (0x4E, 0x57 in set06) */
    u8 x15;             /* 0x015 copied with stage (Quest_pl_stage_init) */
    u8 _pad016[0x1E - 0x16];
    u8 x1E;             /* 0x01E counter scrolling the smoke screen UVs (eft12_t01) */
    u8 _pad01F[0x20 - 0x1F];
    u8 port[2];         /* 0x020 controller port per player (get_sw) */
    u8 _pad022[0x24 - 0x22];
    u8 sw_mask;         /* 0x024 buttons ignored until released (get_sw) */
    u8 _pad025[0x28 - 0x25];
    u8 x28[4];          /* 0x028 per player quest entry id, cleared by Quest_start (Em_direct_set); pl_demo000 compares with 0x12 (monster-grab demo) */
    u16 quest;          /* 0x02C quest number (SonchoInit: 0x83.. tutorials) */
    u8 x2E;             /* 0x02E 6: em20 fly 9 picks point 1 for monster kind 6 */
    u8 x2F;             /* 0x02F from the mission data (Quest_start); stage start slot index (pl_mv014: stage_start_pos/ang) */
    u8 x30[4];          /* 0x030 per player, copied from select_w+0x0C (game11) */
    u8 _pad034[0x40 - 0x34];
    V3S x40[4]; /* 0x040 per player 3 shorts, from select_w+0x5C (game11) */
    u8 _pad058[0x70 - 0x58];
    u8 x70[4];          /* 0x070 from select_w+0x54 */
    u8 _pad074[0x78 - 0x74];
    s8 x78[4];          /* 0x078 from select_w+0x94 */
    u8 _pad07C[0x80 - 0x7C];
    u8 x80[4];          /* 0x080 per player, copied into the cooking smell (Eft12_set4) */
    u8 _pad084[0xA8 - 0x84];
    struct EFT_MDLW *area_mdlw[9]; /* 0x0A8 model sets by area (eft07_t, Em_area_ck) */
    s16 xCC;            /* 0x0CC stolen item id (Item_stolen) */
    s16 xCE;            /* 0x0CE stolen item count */
    u8 x0D0;            /* 0x0D0 cleared by Quest_start */
    u8 master;          /* 0x0D1 player number of the session master */
    u8 x0D2;            /* 0x0D2 set to 1 by Game_task, passed to AQ_init */
    u8 pl_num;          /* 0x0D3 players in the session (shell_hit_ck loops over them) */
    u8 _pad0D4;
    u8 x0D5;            /* 0x0D5 result screen: 7/8 = special end (result_prog); stage/session state switch in sit_com_ck (4..8) */
    u8 _pad0D6[0x128 - 0xD6];
    PL_ITEM reward_item[32]; /* 0x128 reward item list: 32 entries (remuneration_item_set), the screen pages through 16 (reward_mv) */
    s32 x1A8;           /* 0x1A8 cleared by remuneration_item_set */
    s32 x1AC;           /* 0x1AC */
    u8 _pad1B0[0x1B2 - 0x1B0];
    u8 x1B2;            /* 0x1B2 set05: kind-2 fixtures fire once set */
    u8 flag1B3;         /* 0x1B3 bit 0 hides set04 on stage 28 */
    u8 _pad1B4[0x1DC - 0x1B4];
    u8 x1DC;            /* 0x1DC non-zero in the lobby? eft01_t then reads lobby.bin tables */
    u8 x1DD;            /* 0x1DD aim control mode (gun_adj_sub: non-zero inverts the pad, 2 swaps the turn) */
    u8 info_seq;        /* 0x1DE set01 message sequence number (7 bits) */
    u8 info_now;        /* 0x1DF set01 message being shown, 0xFF = none */
    u8 _pad1E0[0x1E6 - 0x1E0];
    u8 gate_open;       /* 0x1E6 set20 gate state, set from the quest */
    u8 x1E7;            /* 0x1E7 from func_63AF40(quest) (Quest_start); non-zero: pit menu shows func_63B470 instead of the map (trans_pit_1) */
    u8 x1E8[4][8];      /* 0x1E8 per player flags, cleared by game0 */
    u8 pl_state[8];       /* 0x208 per player: 0xFF = not joined? (player_init0, pl_work_clr); guess */
    u8 shl10_num;       /* 0x210 live shell10s owned by the master player */
    u8 meat_num;        /* 0x211 meat on the spit owned by the master player (eft12) */
    u8 trap_num;        /* 0x212 traps set by the master player (shell12) */
    s8 x213;            /* 0x213 copied to player work+0x6A4 for the master (pl_init_sub) */
    s8 x214;            /* 0x214 copied to player work+0x6A8 for the master (pl_init_sub) */
    s8 x215;            /* 0x215 added to max life (pl_init_sub) */
    s16 x216;           /* 0x216 stamina bonus passed to Pl_max_stamina_calc (pl_init_sub) */
    s16 x218;           /* 0x218 monster hit points carried over (em02_init) */
    u8 x21A;            /* 0x21A result screen: nonzero in an online session? (result_prog) */
    u8 x21B;            /* 0x21B cleared by Game_task */
    u8 _pad21C;
    s8 x21D;            /* 0x21D quest rule counter shown by font_print_Bdragon (f_menu, guess) */
    u8 _pad21E;
    u8 info_stop;       /* 0x21F set01 queue paused while set */
    u8 _pad220[0x224 - 0x220];
} GAME_W;

extern GAME_W game_w;

#endif
