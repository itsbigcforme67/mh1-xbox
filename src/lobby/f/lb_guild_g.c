/* lb_guild_g - the quest counter ("guild") of PS2 Monster Hunter G, written from G's asm for the PC build (VERSION_MHG only;
 * not byte-compared, logic believed to follow the asm). G rewrote the counter: quests are 16-bit numbers, a star level's list is
 * 8 u16 at lb_quest_all (MH1: 5 bytes in lb_quest_info), a quest's row comes from a static table by number (G 0x559B40) and the
 * story gates (which levels / key quests are open) are a table of clear-bit tests (get_questLevelNum 0x23..0x3C).
 * G addresses (lobby overlay unless noted): Lb_guild 0x55A2D0, lb_guild_startMsg 0x55B240, lb_get_quest_level 0x55B430,
 * check_questLevelSelect 0x55B5F0, lb_select_quest_level 0x55B7A0 (+_trans 0x55CD70), lb_guild_talk 0x55D410, Lb_guild_trans 0x55D5A0,
 * lb_set_questpage_info 0x55D6B0, lb_questpage_trans 0x55DA10, guild_trans_ot0 0x55DE50, lb_select_quest 0x55E020, get_flag_quest
 * 0x55E9C0, lb_key_quest_ck 0x55ECB0, get_new_quest 0x55EDD0, lobby_func_0055EF90, Lb_make_quest_tbl_local 0x55F210, story helpers
 * 0x542680-0x543B88, quest conditions 0x55E300.
 * Simplifications: online branches (rooms, rule sheet, passwords, the three mission blocks of event quests) are left out; a quest
 * whose condition is a hunter statistic (sub_main 0x4C9480) or a held item counts as met.
 * The counter's state lives where G keeps it, in the lobby overlay's memory (same addresses as on the PS2), reached with GL(addr).
 * The MH1 versions of these functions (lb_u.c, lb_sq01.c, lb_x01.c, lb_v03.c ...) are compiled out for G (#ifndef VERSION_MHG). */
#ifdef VERSION_MHG
#include "lbsys_g.h"
#include "types.h"

#define NULL ((void *)0)
#define UD_WARE_N(u) ((1 + (((u8 *)(u))[0xC97] & 1) + ((((u8 *)(u))[0xC97] >> 1) & 1)) * 100)   /* G: 100 / 200 / 300 stored items */
extern u8 lb_sys[];                                 /* G 0x6548F0 */
#define g_lb_base lb_sys    /* not an asm label: __asm__("lb_sys") skips the "_" prefix of win32 names and the
                             * Xbox link failed with "undefined symbol: lb_sys" (14 Oct 2026) */
#define GL(a) ((u8 *)g_lb_base + ((a) - 0x6548F0))  /* any address of the lobby overlay's data / bss */
#define GS8(a) (*(s8 *)GL(a))
#define GU8(a) (*(u8 *)GL(a))
#define GS16(a) (*(s16 *)GL(a))
#define GU16(a) (*(u16 *)GL(a))
#define GS32(a) (*(s32 *)GL(a))
#define GSTR(a) ((char *)GL(a))
#define GPTR(a) (*(char **)GL(a))

/* the counter's work, G 0x654830 */
#define GW 0x654830
#define W_SEL GS16(GW + 0)      /* quest cursor in the list */
#define W_LEVEL GS16(GW + 2)    /* star level cursor */
#define W_TIMER GS16(GW + 4)
#define W_STEP GS8(GW + 6)      /* Lb_guild step */
#define W_SUB GS8(GW + 7)
#define W_MODE GS8(GW + 8)      /* 0 offline, 1 / 2 online town pages */
#define W_PAGE GS8(GW + 9)      /* detail page of the quest sheet */
#define W_LVSEL GS8(GW + 0xA)
#define MH_QUEST GU16(0x6533C2) /* mhRule+0x52: the quest shown / chosen */
#define MH_LEVEL GU8(0x6533C4)  /* mhRule+0x54: its star level */
#define QCOUNT(l) GU8(0x6534E0 + (l))       /* quests in the list of level l */
#define QDONE(l) GU8(0x6534F0 + (l))        /* all quests of level l cleared */
#define QNEW(l) GU8(0x653500 + (l))         /* level l has no new quest left */
#define QLIST(l, k) GU16(0x653510 + (l) * 0x10 + (k) * 2)   /* lb_quest_all: 8 u16 per level */
#define QKEYLIST(k) GU16(0x653560 + (k) * 2)
#define PIT_POS GS32(0x6548DC)
#define PIT_X8 GS8(0x6548E0)
#define PIT_X9 GS8(0x6548E1)
#define PIT_B GS8(0x6548E3)
#define PIT_X0 GS32(0x6548D8)
#define LBS_X68 (*(s32 *)(lb_sys + LBSYS_OFS(0x68)))
#define LBS_X6C (*(s32 *)(lb_sys + LBSYS_OFS(0x6C)))
#define LBS_X87 (*(s8 *)(lb_sys + LBSYS_OFS(0x87)))
#define MASTER (game_w[0x3A])
#define PLAYER(i) (player_work + (i) * 0xA00)

extern u8 lb_sys[];
extern u8 *pNet;
extern u8 *cw;
extern u8 player_work[];
extern u8 User_data[];
extern u8 my_user_mini_data[];
extern u8 lbCommer[];
extern u8 select_w[];
extern u8 game_w[];
extern s32 quest_price;
extern s32 guildPrice;
extern u16 key_quest;
extern s16 key_quest_num;
extern u8 mhRule[];
extern u8 npc_dialog_table[];
extern u32 quest_lv_tbl[], quest_local_tbl[], flag_quest_tbl[], flag_quest_tbl_local[];
extern char *lb_quest_attribute[];
extern u8 lb_quest_color_tex[];
extern s16 lb_quest_font_color[];
extern char *lb_num_str[];
extern char *lb_rule_msg_etc;
extern char *map_name[];
extern char *pDetail;
extern char quest_title[];
extern u8 D_005FBA80[];
extern u16 System_timer;
extern char *D_00528A80[];
extern u32 D_005E7800[];
extern u16 D_005E7810[];
extern char *hunt_group_tbl[];
extern u16 D_004FEA50[];
extern u16 *equip_set_tbl[];
extern u8 D_0035EF10;
extern u8 D_0035EFEB;

int Online_ck();
int Event_flag_ck();
void Event_flag_set();
int Quest_clear_bit_ck();
int ran_suu();
unsigned short Get_sw2();
void cnWrap_SoundRequest();
void Lbc_set_prim();
void Lbc_init_network_work();
int Lb_talk_check_default();
int NPC_Message();
void Lb_put_set01();
void Gold_add();
void NPCZoomInCameraCancel();
int Lbs_InRoomCheck();
int Lb_put_npc_default();
void Lb_menu_quest_info();
void Lb_set_mini_data();
s8 Lb_get_quest_type();   /* s8 (lb_t01.c): declared int, the Xbox clang read EAX's upper bytes: type 16385, crash (21 Oct 2026) */
void Lb_num_to_str();
int sprintf();
char *strcpy();
char *strcat();
char *strrchr();
char *strchr();
void *memcpy();
void *memset();
void han2zen();
int strlen_sp();
void font_set_stack_no();
void font_set_palette();
void flfntSetSize();
void flfntLocate();
void font_print_double();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Put_2TF();
void Paint_square();
void Lb_put_button();
void Lb_put_icon();
void Lb_put_gold();
int Lb_get_cursor_col();
int Lb_cursorUD();
void SetDialogData_G(int gid);   /* lb/lbuic.c */
int get_questLevelNum(int n);
int lb_get_quest_level(int a);
u8 *lobby_g_quest_row(u16 id);
void lb_guild_end(void);
void Lb_guild_trans(u8 *prim);
void lb_set_questpage_info(void);

/* pNet fields of the counter: in the G online build pNet is shared with G's own lobby code (src/lobby/gonl/gen), which
 * keeps G's layout (Lbc_set_prim writes its prims at +0..+0xF, where the MH1 layout keeps the sub step): G's offsets there */
#ifdef MHG_ONLINE
#define PN_SUB 0x22         /* the sub step of lb_select_quest (MH1 2) */
#define PN_BUSY 0x2D        /* a dialog / the net is busy (MH1 0xC) */
#define PN_LEVEL 0x32       /* the hunter's highest quest level (MH1 0x12) */
#else
#define PN_SUB PNET_OFS(2)
#define PN_BUSY PNET_OFS(0xC)
#define PN_LEVEL PNET_OFS(0x12)
#endif

#ifdef MHG_ONLINE
/* the G online counter (src/lobby/gonl/gen: G's own code) */
int lb_rule_seet_set(void);
int lb_guild_make_room(void);
void lb_rule_seet_trans(u8 *prim);
int Lbc_SendMiniData(void);
int Lbs_RoomExit(void);
void lobby_func_0055A140(int quiet);
extern u8 g_main_2EF390[];                         /* G's lobby work area pointer (main 0x2EF390) */
static u8 *event_quest_head(int i)                 /* lobby_func_00559D30: event quest i's header (quest_title+0x30, 512 bytes
                                                    * each, a self-relative word) */
{
    u8 *p = (u8 *)quest_title + 0x30 + i * 0x200;
    return p + *(s32 *)p;
}
#endif

/* the PC keeps MH1's mem_tex numbers: the lobby UI sprites are slot 0x157 (G 0x180) */
#define TEX_UI 0x157

/* ---------------------------------------------------------------------------------------------- hunter statistics (sub_main) */
#define UD_HUNT(u, k) (*(u16 *)((u8 *)(u) + 0xD10 + ((k) & 0xFFFF) * 2))   /* sub_main 0x4C85A0 */
#define UD_CAPT(u, k) (*(u16 *)((u8 *)(u) + 0xD76 + ((k) & 0xFF) * 2))     /* sub_main 0x4C8580 */

static int ud_group_max(void *u, int n)             /* sub_main 0x4C87C0: the largest hunt count in a group */
{
    u8 *p = (u8 *)hunt_group_tbl[n & 0xFF];
    int best = 0;
    for (; *p != 0xFF; p++) {
        int v = UD_HUNT(u, *p) & 0xFFFF;
        if (best < v) {
            best = v;
        }
    }
    return best;
}

static int ud_group_sum(void *u, int n)             /* sub_main 0x4C8870: the capture count of a group, capped */
{
    u8 *p = (u8 *)hunt_group_tbl[n & 0xFF];
    int sum = 0;
    for (; *p != 0xFF; p++) {
        sum = (sum + UD_CAPT(u, *p)) & 0xFFFF;
    }
    return sum >= 0x2710 ? 0x270F : sum;
}

static int ud_sum_ge(void *u, int n, int thr) { return (ud_group_sum(u, n) & 0xFFFF) >= (thr & 0xFFFF); }   /* 0x4C85C0 */
static int ud_max_ge(void *u, int n, int thr) { return (ud_group_max(u, n) & 0xFFFF) >= (thr & 0xFFFF); }   /* 0x4C8600 */

static int ud_fifty_cleared(void)                   /* sub_main 0x4C9410: all 0x32 quests of a list cleared */
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (Quest_clear_bit_ck(D_004FEA50[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

static int ud_equip_set_ck(void *u, int n)          /* sub_main 0x4C8A10: one piece of an equipment set is in the warehouse */
{
    u16 *t;
    int cnt, j;
    u8 *w;
    if ((n & 0xFF) >= 4) {
        return 0;
    }
    t = equip_set_tbl[n & 0xFF];
    cnt = UD_WARE_N(u);
    for (; (t[0] & 0xFF) != 0xFF; t += 2) {
        w = (u8 *)u;
        for (j = 0; j < cnt; j++, w += 6) {
            if (w[0x44] != 0 && w[0x45] == (t[0] & 0xFF) && *(u16 *)(w + 0x46) == t[1]) {
                return 1;
            }
        }
    }
    return 0;
}

/* ---------------------------------------------------------------------------------------------- the quest rows */
/* lobby_func_00559C30 (+ B40): a quest number -> its row. Below 50000 the row is in the static table D_005FBA80 (a bucket by
 * thousands: 10000s -> 4, 20000s -> 5, then the number mod 1000); event quests live in loaded blocks (not on the PC). */
u8 *lobby_g_quest_row(u16 id)
{
    int bucket;
    u32 *tab = (u32 *)D_005FBA80;
    u32 *b;
    if (id >= 50000) {
        return NULL;
    }
    if (id < 10000) {
        bucket = id / 1000;
    } else if (id < 20000) {
        bucket = 4;
    } else {
        bucket = 5;
    }
    b = (u32 *)tab[bucket];
    return (u8 *)b[id % 1000];
}

/* ---------------------------------------------------------------------------------------------- story gates */
static int quest_list_has(u16 id)                     /* lobby_func_005434A0: id is the 2nd (1) or 3rd (2) member of a triple */
{
    int i;
    if (D_005E7810[0] == 0xFFFF) {
        return 0;
    }
    for (i = 0; D_005E7810[i] != 0xFFFF; i += 3) {
        if (D_005E7810[i + 1] == id) {
            return 1;
        }
        if (D_005E7810[i + 2] == id) {
            return 2;
        }
    }
    return 0;
}

static int quest_blocked(u16 id)                      /* lobby_func_005437F0 */
{
    int i;
    ran_suu(1);
    if (D_005E7810[0] == 0xFFFF) {
        return 0;
    }
    for (i = 0; D_005E7810[i] != 0xFFFF; i += 3) {
        if (id == D_005E7810[i]) {
            return 0;
        }
        if (id == D_005E7810[i + 1]) {
            return Quest_clear_bit_ck(D_005E7810[i]) != 1;
        }
        if (id == D_005E7810[i + 2]) {
            if (Quest_clear_bit_ck(D_005E7810[i]) == 1 && Quest_clear_bit_ck(D_005E7810[i + 1]) == 1) {
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

static int quest_locked(u16 id)                       /* lobby_func_005438F0: 1 = not open yet (story gate), 2 = no gate */
{
    if (quest_blocked(id) == 1) {
        return 1;
    }
    switch (id) {
    case 0x271B: return Quest_clear_bit_ck(0x271F) != 1;
    case 0x2731: case 0x3FD: return get_questLevelNum(0x2F) != 1;
    case 0x2738: case 0x2736: case 0x409: return get_questLevelNum(0x30) != 1;
    case 0x7DD: case 0x7D4: case 0x7D2: return get_questLevelNum(0xE) != 1;
    case 0x7DE: case 0x7DA: case 0x7D8: case 0x7D6: return get_questLevelNum(0xF) != 1;
    case 0x7F0: case 0x7E5: case 0x7E3: case 0x7E1: return get_questLevelNum(0x13) != 1;
    case 0x7ED: case 0x7E9: case 0x7E7: return get_questLevelNum(0x14) != 1;
    case 0x7F6: case 0x7F3: return get_questLevelNum(0x17) != 1;
    case 0x7FF: case 0x7F8: return get_questLevelNum(0x18) != 1;
    case 0x7FD: case 0x7FA: return get_questLevelNum(0x19) != 1;
    case 0x7FE: case 0x7FC: return get_questLevelNum(0x1A) != 1;
    case 0x801: case 0x7F4: case 0x7F1: return get_questLevelNum(0x1B) != 1;
    default: return 2;
    }
}

static int story_flag_list(int local, int l)          /* lobby_func_00542790: all quests of flag_quest_tbl[_local][l] cleared */
{
    u16 *p = (u16 *)(local ? flag_quest_tbl_local[l & 0xFF] : flag_quest_tbl[l & 0xFF]);
    for (; *p != 0; p++) {
        if (Quest_clear_bit_ck(*p) == 0) {
            return 0;
        }
    }
    return 1;
}

static int story_all_event_done(void)                 /* lobby_func_005428F0 */
{
    int l;
    for (l = 0; l < 6; l++) {
        u16 *p = (u16 *)quest_lv_tbl[l];
        for (; *p != 0; p++) {
            if (*p >= 0x2710 && *p < 0x4E20 && Quest_clear_bit_ck(*p) == 0 && *p != 0x2740) {
                return 0;
            }
        }
    }
    return 1;
}

static int story_equip_all(void)                      /* lobby_func_005429A0 */
{
    u8 buf[12];
    u8 *p;
    memcpy(buf, (u8 *)D_005E7800, 12);
    for (p = buf; *p != 0; p++) {
        if (ud_sum_ge(User_data, *p, 0x14) == 0) {
            return 0;
        }
    }
    return 1;
}

int get_questLevelNum(int n)                          /* lobby_func_00543180 (offline) */
{
    if (Online_ck() == 1) {
#ifdef MHG_ONLINE
        int lobby_func_00542A60(int n, int a1, int a2, int a3);   /* G's online table, src/lobby/gonl/gen (the guild
                                                                    * hall counter: registration = event flag 4) */
        return lobby_func_00542A60(n, 0, 0, 0);
#else
        return 0;                                    /* the online table is in the G online build only */
#endif
    }
    switch (n & 0xFF) {
    case 0x23: return Event_flag_ck(2);
    case 0x24: return Event_flag_ck(0);
    case 0x25: return Quest_clear_bit_ck(0x2711);
    case 0x26: return story_flag_list(1, 0);
    case 0x27: return Quest_clear_bit_ck(0x2716);
    case 0x28: return story_flag_list(1, 1);
    case 0x29: return Quest_clear_bit_ck(0x271E);
    case 0x2A: return story_flag_list(1, 2);
    case 0x2B: return Quest_clear_bit_ck(0x2726);
    case 0x2C: return story_flag_list(1, 3);
    case 0x2D: return Quest_clear_bit_ck(0x2730);
    case 0x2E: return Event_flag_ck(1);
    case 0x2F: return ud_max_ge(User_data, 0xB, 0x72);
    case 0x30: return ud_max_ge(User_data, 1, 0x76);
    case 0x31: return story_flag_list(1, 4);
    case 0x32: return Quest_clear_bit_ck(0x2739);
    case 0x33: return ud_max_ge(User_data, 0x1A, 0x74);
    case 0x34: return get_questLevelNum(0x32) == 1 ? ud_equip_set_ck(User_data, 0) : 0;
    case 0x35: return get_questLevelNum(0x32) == 1 ? ud_equip_set_ck(User_data, 1) : 0;
    case 0x36: return get_questLevelNum(0x32) == 1 ? ud_equip_set_ck(User_data, 2) : 0;
    case 0x37: return (Quest_clear_bit_ck(0x273E) == 1 && Quest_clear_bit_ck(0x273D) == 1 && Quest_clear_bit_ck(0x273B) == 1) ? 1 : 0;
    case 0x38: return Quest_clear_bit_ck(0x273C);
    case 0x39: return (story_all_event_done() == 1 && ud_fifty_cleared() == 1) ? 1 : 0;
    case 0x3A: return Event_flag_ck(0x52) == 0 ? Quest_clear_bit_ck(0x2740) : 0;
    case 0x3B: return Event_flag_ck(0x5A) == 0 ? story_equip_all() : 0;
    case 0x3C: return 0;                              /* (Event_flag_ck(0x5B) == 0 and the current stage is 0x58) */
    default: return 0;
    }
}

/* ---------------------------------------------------------------------------------------------- the quest tables */
int lb_key_quest_ck(u16 id)
{
    switch (id) {
    case 0xBBF: case 0xBBE: case 0xBBD: case 0xBBC: case 0x2740: case 0x273C: case 0x273B: case 0x273D: case 0x273E:
    case 0x273A: case 0x2739: case 0x2730: case 0x2726: case 0x271E: case 0x2716: case 0x2711: case 0xBBB: case 0x802:
    case 0x7EC: case 0xBBA: case 0x40B: case 0xBB9: case 9: case 0x27:
        return 1;
    }
    return 0;
}

int lb_guild_check_keyQuest(u32 n)
{
    int rank = *(u8 *)(User_data + 0xC23);            /* G 0x32CE33 */
    switch (n) {
    case 0: return rank >= 4;
    case 1: return rank >= 8;
    case 2: return rank >= 0xC;
    case 3: return rank >= 0x10;
    case 4: return rank >= 0x14;
    case 5: return rank >= 0x17;
    case 6: return rank >= 0x1A;
    case 7: return rank >= 0x1D;
    default: return 1;
    }
}

int lb_get_quest_level(int a)
{
    int rank;
    if (Online_ck() == 1) {
        rank = *(u8 *)(User_data + 0xC23);
        if (rank < 5) return 0;
        if (rank < 9) return 1;
        if (rank < 0xD) return 2;
        if (rank < 0x11) return 3;
        if (rank < 0x15) return 4;
        if (rank < 0x18) return 5;
        if (rank < 0x1B) return 6;
        return rank < 0x1F ? 7 : 0;
    }
    if (Quest_clear_bit_ck(0x2739) == 1) {
        return (s8)a == 1 ? 5 : 4;
    }
    if (Quest_clear_bit_ck(0x2730) == 1) return 4;
    if (Quest_clear_bit_ck(0x2726) == 1) return 3;
    if (Quest_clear_bit_ck(0x271E) == 1) return 2;
    if (Quest_clear_bit_ck(0x2716) == 1) return 1;
    return -(Quest_clear_bit_ck(0x2711) != 1);
}

static int levels_shown(void)                         /* lobby_func_0055CD10 */
{
    return W_MODE >= 2 ? 3 : 5;
}

int check_questLevelSelect(int lv, int pad)
{
    int cur = (s8)lb_get_quest_level(0);
    int base, s;
    (void)pad;
    if (cur == -1) {
        if ((s8)lv == 5 && Online_ck() == 0) {
            return 0;
        }
        return 1;
    }
    base = Online_ck() != 0 ? 8 : 5;
    if (Online_ck() == 0 && key_quest == 0x2711 && (s8)lv == 0) {
        return 1;
    }
    s = (s8)lv;
    if (cur >= s) {
        if ((s8)lb_get_quest_level(0) >= s) {
            return 0;
        }
        return 1;
    }
    if (s == base) {
        if (W_MODE == 1 && get_questLevelNum(0xC) == 1) {
            return 1;
        }
        if (key_quest_num != 0) {
            return 0;
        }
        return 1;
    }
    if (Online_ck() == 1 && *(s8 *)(cw + 0xB97) != 0 && s == base + 1) {
        return 0;
    }
    return 1;
}

u16 get_flag_quest(int level)                         /* 0x55E9C0: a "flag" quest of the level that is open and not cleared */
{
    u16 *t;
    int n, start, i;
    if (level == -1) {
        return 0;
    }
    t = (u16 *)(Online_ck() == 1 ? flag_quest_tbl[level] : flag_quest_tbl_local[level]);
    for (n = 0; t[n] != 0; n++) {
    }
    if (n == 0) {
        return 0;
    }
    start = (ran_suu(1) & 0xFFFF) % n;
    for (i = 0; i < n; i++) {
        u16 id = t[start];
        if (id < 0xBBC || id >= 0xBC0) {
            if (Quest_clear_bit_ck(id) == 0 && quest_locked(id) != 1) {
                return id;
            }
        }
        if (++start >= n) {
            start = 0;
        }
    }
    return 0;
}

u16 get_new_quest(int level, int any)                 /* 0x55EDD0: a quest of the level's table that is not cleared and open */
{
    u16 *t;
    int n, start, i;
    if (level == -1) {
        return 0;
    }
    t = (u16 *)(Online_ck() == 1 ? quest_lv_tbl[level] : quest_local_tbl[level]);
    for (n = 0; t[n] != 0; n++) {
    }
    if (n == 0) {
        return 0;
    }
    start = (ran_suu(1) & 0xFFFF) % n;
    for (i = 0; i < n; i++) {
        u16 id = t[start];
        if (Quest_clear_bit_ck(id) == 0) {
            if (Online_ck() == 0 || id < 0x2710) {
                if (lb_key_quest_ck(id) == 0) {
                    if (any != 0) {
                        return id;
                    }
                    if (quest_locked(id) != 1) {
                        return id;
                    }
                }
            }
        }
        if (++start >= n) {
            start = 0;
        }
    }
    return 0;
}

/* lobby_func_0055EF90: the key quests of the last star level in the order they are offered; returns how many */
static int key_quests_final(void)
{
    int n = 0;
    if (get_questLevelNum(0x39) == 1) {
        QKEYLIST(0) = 0x2740;
        n = 1;
    }
    if (get_questLevelNum(0x38) == 1) { QKEYLIST(n) = 0x273F; n++; }
    if (get_questLevelNum(0x37) == 1) { QKEYLIST(n) = 0x273C; n++; }
    if (get_questLevelNum(0x36) == 1) { QKEYLIST(n) = 0x273E; n++; }
    if (get_questLevelNum(0x35) == 1) { QKEYLIST(n) = 0x273D; n++; }
    if (get_questLevelNum(0x34) == 1) { QKEYLIST(n) = 0x273B; n++; }
    if (get_questLevelNum(0x33) == 1) {
        if ((ran_suu(1) & 0xFFFF) % 100 >= 0x1F && Quest_clear_bit_ck(0x273A) == 1) {
            QKEYLIST(n) = 0x2739;
        } else if (Quest_clear_bit_ck(0x2739) == 0) {
            QKEYLIST(n) = 0x2739;
        } else {
            QKEYLIST(n) = 0x273A;
        }
    } else {
        QKEYLIST(n) = 0x2739;
    }
    return n + 1;
}

/* lobby_func_00543520(list, id, slot, swap, count): put id into slot of a level's list. A quest of a story triple takes the place of
 * its companions already in the list; slot 0 (the "new quest" slot) is only taken when its quest is cleared or empty. */
static void quest_put(u16 *list, u16 id, int slot, int swap, int count)
{
    int chance = (ran_suu(1) & 0xFFFF) % 100;
    int comp1 = 0, comp2 = 0, i;
    int pos = slot & 0xFF;
    if (id == 0xBBF || id == 0xBBE || id == 0xBBD || id == 0xBBC || id == 0xBBB) {
        return;
    }
    if (D_005E7810[0] != 0xFFFF) {
        for (i = 0; D_005E7810[i] != 0xFFFF; i += 3) {
            if (id == D_005E7810[i]) {
                comp1 = D_005E7810[i + 1];
                comp2 = D_005E7810[i + 2];
                break;
            }
            if (id == D_005E7810[i + 1]) {
                if (chance >= 0x33 && Quest_clear_bit_ck(id) == 1) {
                    return;
                }
                comp1 = D_005E7810[i];
                comp2 = D_005E7810[i + 2];
                break;
            }
            if (id == D_005E7810[i + 2]) {
                if (chance >= 0x1F && Quest_clear_bit_ck(id) == 1) {
                    return;
                }
                comp1 = D_005E7810[i];
                comp2 = D_005E7810[i + 1];
                break;
            }
        }
    }
    for (i = 0; i < (count & 0xFF); i++) {
        if (list[i] != 0 && (list[i] == id || list[i] == comp1 || list[i] == comp2)) {
            pos = i;
            break;
        }
    }
    if (pos == 0) {
        if (Quest_clear_bit_ck(list[0]) == 0 && list[0] != 0) {
            return;
        }
    }
    if ((swap & 0xFF) == 0 || pos == (slot & 0xFF)) {
        list[pos] = id;
    } else {
        u16 t = list[slot & 0xFF];
        list[slot & 0xFF] = id;
        list[pos] = t;
    }
}

void Lb_make_quest_tbl_local(void)
{
    int l, k, n, lvl, flagged = 0, key = 0;
    u32 rnd = 0;
    u16 cur = 0;
    key_quest = 0;
    key_quest_num = 0;
    memset(GL(0x6534E0), 0, 0xA0);
    lvl = (s8)lb_get_quest_level(1);
    for (l = 0; l < 5; l++) {
        u16 *src = (u16 *)quest_local_tbl[l];
        u16 *list = (u16 *)GL(0x653510 + l * 0x10);
        QNEW(l) = 0;
        QDONE(l) = 0;
        k = 0;                                        /* the list keeps the table's order, holes where a triple hides a quest */
        for (n = 0; src[n] != 0; n++) {
            if (quest_list_has(src[n]) == 0) {
                k++;
                list[n] = src[n];
            }
            if (k >= 5) {
                break;
            }
        }
        k = 0;                                        /* the open event quests (10000 up) come first */
        for (n = 0; k < 5 && src[n] != 0; n++) {
            if (src[n] >= 0x2710 && quest_locked(src[n]) != 1 && quest_list_has(src[n]) == 0) {
                list[k++] = src[n];
            }
        }
        QCOUNT(l) = 5;
    }
    for (k = 0; k < 5; k++) {
        QKEYLIST(k) = 0;
    }
    if (lvl == -1) {
        key_quest = 0x2711;
    } else {
        for (l = 0; l < 5; l++) {
            u16 *src = (u16 *)quest_local_tbl[l];
            u16 *list = (u16 *)GL(0x653510 + l * 0x10);
            u16 newq = get_new_quest(l, 0);
            if (newq == 0 && get_new_quest(l, 1) == 0) {
                QNEW(l) = 1;
            }
            if (lvl == 5) {
                key_quest = 0x2739;
            } else if (lvl == l) {
                flagged = 1;
                cur = get_flag_quest(lvl);
                if (cur == 0) {
                    flagged = 0;
                    switch (lvl) {
                    case 0: key_quest = 0x2716; break;
                    case 1: key_quest = 0x271E; break;
                    case 2: key_quest = 0x2726; break;
                    case 3: key_quest = 0x2730; break;
                    case 4: key_quest = 0x2739; break;
                    }
                } else if (l != 0 && l != 1) {
                    quest_put(list, cur, 4, 1, QCOUNT(l));
                }
            }
            if (newq != 0 && l != 0 && l != 1) {
                flagged = 1;
                quest_put(list, newq, 0, 1, QCOUNT(l));
            }
            if (l != 0 && l != 1) {
                int room = 5 - flagged;
                for (k = 0; src[k] != 0; k++) {
                    if (rnd == 0) {
                        rnd = ran_suu(1) & 0xFFFF;
                    }
                    if (src[k] == 0x2739 && Quest_clear_bit_ck(0x2739) != 1) {
                        continue;
                    }
                    if ((rnd >> (k % 32)) & 1) {
                        if (cur != src[k] && newq != src[k]) {
                            int pos = flagged + (ran_suu(1) & 0xFFFF) % room;
                            if (quest_locked(src[k]) != 1 && cur != list[pos]) {
                                if (pos != 0 || newq == 0) {
                                    quest_put(list, src[k], pos & 0xFF, 0, QCOUNT(l));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    (void)key;
    if (key_quest == 0x2739) {
        key_quest_num = (s8)key_quests_final();
    } else if (key_quest != 0) {
        QKEYLIST(0) = key_quest;
        key_quest_num = 1;
    }
    GU8(0x6534E5) = (u8)key_quest_num;
}

static void guild_mark_done(void)                     /* lobby_func_0055A1E0 */
{
    int l, k;
    for (l = 0; l < 10; l++) {
        int notdone = 0, have = 0;
        QDONE(l) = 0;
        for (k = 0; k < 8; k++) {
            u16 id = QLIST(l, k);
            if (id != 0) {
                if (Quest_clear_bit_ck(id) == 0) {
                    notdone = 1;
                }
                have++;
            }
        }
        if (notdone == 0 && have != 0) {
            QDONE(l) = 1;
        }
    }
}

int Lb_check_money(u32 id)                            /* 0x559BD0: the quest's fee is paid at once */
{
    u8 *row = lobby_g_quest_row((u16)id);
    s32 fee = *(s32 *)(row + 4);
    if (*(s32 *)(User_data + 0x20) < fee) {
        return 1;
    }
    Gold_add(-fee);
    cnWrap_SoundRequest(8);
    return 0;
}

/* lobby_func_0055E300(kind, online): may the hunter take a quest with this condition? */
static int quest_cond(int kind, int online)
{
    int rank = my_user_mini_data[1];
    u8 *pl = PLAYER(MASTER);
    int wk = pl[2];
    int gold = *(s32 *)(User_data + 0x20);
    online &= 0xFF;
    switch (kind & 0xFF) {
    case 1: return rank >= 5 || !online;
    case 2: return rank >= 9 || !online;
    case 3: return rank >= 0xD || !online;
    case 4: return rank >= 0x11 || !online;
    case 5: return rank >= 0x15 || !online;
    case 6: return rank >= 0x18 || !online;
    case 7: return rank >= 0x1B || !online;
    case 8: return rank >= 0x1D || !online;
    case 9: return rank == 0x1E || !online;
    case 0xA: return rank < 5;
    case 0xB: return rank < 9;
    case 0xC: return rank < 0xD;
    case 0xD: return rank < 0x11;
    case 0xE: return rank < 0x15;
    case 0xF: return rank < 0x18;
    case 0x10: return rank < 0x1B;
    case 0x11: return rank > 0 && rank < 0xD;
    case 0x12: return rank >= 0xD && rank < 0x15;
    case 0x13: return rank >= 0x15 && rank < 0x1F;
    case 0x14: return rank >= 0xD && rank < 0x1F;
    case 0x15: return rank == 0x1E;
    case 0x16: return my_user_mini_data[0x19] == 0;
    case 0x17: return my_user_mini_data[0x19] == 1;
    case 0x18: return wk == 0;
    case 0x19: return wk == 3;
    case 0x1A: return wk == 2;
    case 0x1B: return wk == 4;
    case 0x1C: return wk == 1 || wk == 5;
    case 0x1E: return *(u16 *)(User_data + 0xDAA) != 0;      /* G 0x32CFBA */
    case 0x1F: return Quest_clear_bit_ck(0x2740);
    case 0x20: return rank >= 0x11 && rank < 0x1F;
    case 0x24: return User_data[0xC75] == 6 && *(u16 *)(User_data + 0xC76) == 0x9C;
    case 0x25: return gold < 0x65;
    case 0x30: return *(u16 *)(User_data + 0xD84) != 0;      /* G 0x32CF94 */
    case 0x31: return *(u16 *)(User_data + 0xD7A) != 0;      /* G 0x32CF8A */
    case 0x33: return wk == 5;
    case 0x34: return wk == 1;
    case 0x35: return wk == 6;
    case 0x36: return gold >= 0xC350 || online == 1;
    case 0x37: return gold >= 0x186A0 || online == 1;
    case 0x38: return gold >= 0x30D40 || online == 1;
    case 0x3B: return rank >= 0x1B && rank < 0x1F;
    default: return 1;                                        /* includes the statistic / item conditions (see the header) */
    }
}

#ifdef MHG_ONLINE
int lobby_func_0055E300(int kind, int online)         /* G's own code calls the quest conditions by address (the room list's
                                                       * check_room_require; a stand-in returning 0 refused every room) */
{
    return quest_cond(kind & 0xFF, online & 0xFF);
}
#endif

/* ---------------------------------------------------------------------------------------------- the quest sheet text */
void lb_set_questpage_info(void)
{
    u16 id;
    u8 *row;
    char **s0;
    int type, n;
    char tmp[0x40], tmp2[0x40];
    u8 *tex;
    if (*(u8 *)(cw + 0xFA5F) != 0) {
        id = MH_QUEST;
    } else {
        id = QLIST(MH_LEVEL, W_SEL);
    }
    row = lobby_g_quest_row(id);
    type = Lb_get_quest_type(row);
    tex = lb_quest_color_tex + type * 8;
    GS16(0x6356FC) = *(s16 *)tex;
    GS16(0x6356FE) = *(s16 *)(tex + 2);
    GS16(0x635700) = GS16(0x6356D4 + type * 8);
    GS16(0x635702) = GS16(0x6356D6 + type * 8);
    s0 = *(char ***)(row + 0x18);
    sprintf(GSTR(0x6360D4), lb_quest_attribute[type]);
    strcpy(GSTR(0x636138), s0[0]);
    strcpy(quest_title, s0[0]);
    Lb_num_to_str(*(s32 *)(row + 8), GSTR(0x63619C));
    strcat(GSTR(0x63619C), GPTR(0x6364C8));
    Lb_num_to_str(*(s32 *)(row + 4), GSTR(0x636200));
    strcat(GSTR(0x636200), GPTR(0x6364C8));
    guildPrice = *(s32 *)(row + 4);
    n = *(s32 *)(row + 0x10) / 60 / 30;
    sprintf(tmp, GSTR(0x63DD30), n, lb_rule_msg_etc);
    han2zen(tmp, tmp2);
    strcpy(GSTR(0x636264), tmp2);
    sprintf(GSTR(0x6362C8), map_name[*(s32 *)(row + 0x14)]);
    if (row[0x1C] < 0x3C) {
        strcpy(GSTR(0x63632C), D_00528A80[row[0x1C]]);
    }
    GSTR(0x636390)[0] = 0;
    for (n = 0; n < row[2]; n++) {
        strcat(GSTR(0x636390), GPTR(0x6365C8));
    }
    strcpy(GSTR(0x6363F4), s0[1]);
    strcpy(GSTR(0x636458), s0[2]);
    pDetail = s0[3];
}

/* ---------------------------------------------------------------------------------------------- the screens */
int lb_select_quest_level(void)                       /* 0x55B7A0 */
{
    int sw = Get_sw2(0) & 0xFFFF;
    s16 v;
    *(s8 *)(pNet + PN_LEVEL) = (s8)lb_get_quest_level(0);
    if (sw & 0x20) {
        v = W_LEVEL;
        switch (W_MODE) {
        case 1:
            if (v >= 5) {
                v += 3;
            }
            break;
        case 2:
            v += 5;
            break;
        }
        if ((s8)check_questLevelSelect((s8)v, sw) == 0) {
            W_LVSEL = (s8)v;
            MH_LEVEL = (u8)v;
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(7);
        return 2;
    }
    if (sw & 0x40) {
        cnWrap_SoundRequest(3);
        return 3;
    }
    W_LEVEL = (s8)Lb_cursorUD(W_LEVEL, levels_shown() + (W_MODE != 0 ? 2 : 1));
    return 2;
}

int lb_select_quest(void)                             /* 0x55E020 */
{
    int sw = Get_sw2(0) & 0xFFFF;
    u8 *row = lobby_g_quest_row(QLIST(MH_LEVEL, W_SEL));
    u8 *st = pNet + PN_SUB;
    switch (*st) {
    case 0:
        (*st)++;
        lb_set_questpage_info();
        Lbc_set_prim(0, Lb_guild_trans, 0, 0);
        return 2;
    case 1:
        if (sw & 0x20) {
            if (quest_cond(row[0x1C], 0) == 1) {
                GU16(0x6533C2) = *(u16 *)(row + 0x1E);       /* (the two other copies of G are not read on the PC) */
                *(u16 *)(game_w + 0x2C) = *(u16 *)(row + 0x1E);                /* G 0x35ED4C */
                cnWrap_SoundRequest(0);
                return 0;
            }
            SetDialogData_G(0x55);
            *(s8 *)(pNet + PN_BUSY) = 1;
            (*st)++;
            D_0035EFEB = 0;
            cnWrap_SoundRequest(6);
            return 2;
        }
        if (sw & 0x40) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        if (sw & 0x400) {
            W_SEL++;
            if (W_SEL >= QCOUNT(MH_LEVEL)) {
                W_SEL = 0;
            }
            lb_set_questpage_info();
            MH_QUEST = *(u16 *)(row + 0x1E);
            cnWrap_SoundRequest(1);
        } else if (sw & 0x800) {
            W_SEL = W_SEL == 0 ? QCOUNT(MH_LEVEL) - 1 : W_SEL - 1;
            lb_set_questpage_info();
            MH_QUEST = *(u16 *)(row + 0x1E);
            cnWrap_SoundRequest(1);
        } else if (sw & 0x200) {
            if (++W_PAGE >= 3) {
                W_PAGE = 0;
            }
            cnWrap_SoundRequest(6);
        }
        return 2;
    case 2:
        *(s8 *)(pNet + PN_BUSY) = 1;
        if (sw & 0x20) {
            D_0035EFEB = 1;
            *st = 1;
            cnWrap_SoundRequest(6);
        }
        return 2;
    }
    return 2;
}

void lb_guild_talk(void)                              /* 0x55D410 */
{
    u8 *ent;
    char *p;
    if (GS8(0x654836) >= 2) {
        ent = (u8 *)(PIT_POS + PIT_X8 * 8);
        font_set_palette(0);
        switch (W_STEP) {
        case 6:
            strcpy(GSTR(0x6533E0), *(char **)(ent + 4));      /* guildStr */
            p = strrchr(GSTR(0x6533E0), 0x24);
            if (p != NULL) {
                Lb_num_to_str(guildPrice, p);          /* G 0x5935A0 (fixed 15 Oct: the old sprintf read its format from a made-up address and printed nothing) */
                strcat(GSTR(0x6533E0), strrchr(*(char **)(ent + 4), 0x24) + 1);
            }
            PIT_B = NPC_Message(GSTR(0x6533E0), PIT_X0, *(u16 *)ent, PIT_X9);
            return;
        case 17:
        case 9:
        case 0:
            NPC_Message(NULL, PIT_X0, 3, PIT_X9);
            return;
        default:
            PIT_B = NPC_Message(*(char **)(ent + 4), PIT_X0, *(u16 *)ent, PIT_X9);
            break;
        }
    }
}

int lb_guild_startMsg(void)                           /* 0x55B240 */
{
    u8 *ent = (u8 *)(PIT_POS + PIT_X8 * 8);
    switch (GS8(0x654837)) {
    case 0:
        W_SUB++;
        if (Online_ck() == 0) {
            if (get_questLevelNum(0x2E) == 1) {
                PIT_X0 = 0;
                PIT_X8 = 0xB;
                return 0;
            }
            if (get_questLevelNum(0x2D) == 1) {
                PIT_X0 = 0;
                PIT_X8 = 0x14;
                PIT_POS += PIT_X8 * 8;
                return 0;
            }
            if (Quest_clear_bit_ck(0x2716) == 1) {
                PIT_X0 = 0;
                PIT_X8 = 0xA;
                return 0;
            }
        }
        PIT_X8 = 0;
        return 0;
    case 1:
        if (Online_ck() == 0 && get_questLevelNum(0x2E) == 0 && get_questLevelNum(0x2D) == 1) {
            if (Lb_put_npc_default() == 0) {
                W_SUB = 0;
                return 1;
            }
            return 0;
        }
        PIT_B = NPC_Message(*(char **)(ent + 4), PIT_X0, *(u16 *)ent, PIT_X9);
        if ((s8)Lb_talk_check_default(2) != 0) {
            W_SUB = 0;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

void lb_guild_end(void)                               /* 0x55A190 */
{
    LBS_X6C = 0;
    W_STEP = 0;
    LBS_X87 = 0x14;
    LBS_X68 = 0;
    Lbc_init_network_work();
    NPCZoomInCameraCancel();
}

void Lb_guild(void)                                   /* 0x55A2D0 */
{
    u8 *tgt = *(u8 **)(PLAYER(MASTER) + 0x3B0);
    u8 *em = tgt + 0x444;
    u8 *row;
    int r;
    Get_sw2(0);
    switch (GS8(0x654836)) {
    case 0:
        GU8(0x635ED4) = 0;
        GU8(0x635F38) = 0;
        GU8(0x635F9C) = 0;
        GU8(0x636000) = 0;
        GU8(0x636064) = 0;
        PIT_POS = *(s32 *)(npc_dialog_table + em[0xE] * 4);       /* the NPC kind: this port keeps MH1's NPC work layout (G +0x34) */
        W_STEP++;
        W_SUB = 0;
        W_SEL = 0;
        guild_mark_done();
        if (Online_ck() == 0) {
            if (key_quest == 0x2739) {
                key_quest_num = (s8)key_quests_final();
                GU8(0x6534E5) = (u8)key_quest_num;
            }
        }
#ifdef MHG_ONLINE
        else {
            if (get_questLevelNum(1) == 0) {        /* not registered with the guild: the talk mode 6, no counter */
                LBS_X68 = 6;
                W_STEP = 0;
                return;
            }
            if (cw[CW_OFS(0x2C2F)] != 0) {          /* cw+0xB97: event quests downloaded: their numbers make level 9's list */
                int i, k = 0;
                for (i = 0; i < 3; i++) {
                    if (*(s32 *)(*(u8 **)&g_main_2EF390 + 0x23F4 + i * 8 + 8) != 0) {
                        QLIST(9, k) = *(u16 *)(event_quest_head(i) + 0x1E);
                        k++;
                    }
                }
                QCOUNT(9) = (u8)k;                  /* guildStr+0x109 */
            }
        }
#endif
        break;
    case 1:
        Lbc_init_network_work();
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, Lb_guild_trans, 0, 0);
#ifdef MHG_ONLINE
        if (Online_ck() == 1) {                     /* G 0x55A530 */
            if (Lbs_InRoomCheck() == 0) {
                PIT_X8 = 0;
                PIT_X0 = 0;
                W_STEP++;
                memset(GL(0x653370), 0, 0x64);      /* mhRule: a new room's rules */
            } else {                                /* already in a room: its quest sheet */
                PIT_X0 = 0;
                PIT_X9 = 0;
                PIT_X8 = 0xA;
                W_STEP = 0xF;
                W_TIMER = 0x14;
                MH_QUEST = *(u16 *)(game_w + 0x2C);
                lb_set_questpage_info();
            }
            break;
        }
#endif
        W_STEP++;                                   /* (offline, nobody in a room) */
        break;
    case 2:
        if (lb_guild_startMsg() == 1) {
            if (Online_ck() == 0 && get_questLevelNum(0x2E) == 0 && get_questLevelNum(0x2D) == 1) {
                Event_flag_set(3);
                lb_guild_end();
                return;
            }
            cnWrap_SoundRequest(9);
            if (key_quest_num == 0) {
                W_LEVEL = (s8)lb_get_quest_level(0);
                switch (W_MODE) {
                case 1:
                    if (W_LEVEL >= 5) {
                        W_LEVEL = 4;
                    }
                    break;
                case 2:
                    W_LEVEL = W_LEVEL - 5;
                    break;
                }
            } else {
                switch (W_MODE) {
                case 0: W_LEVEL = 5; break;
                case 1: W_LEVEL = get_questLevelNum(0xC) == 0 ? 5 : 4; break;
                case 2: W_LEVEL = 3; break;
                }
            }
            if ((s8)W_LEVEL < 0) {
                W_LEVEL = 5;
            }
            W_STEP++;
        }
        break;
    case 3:
        r = lb_select_quest_level();
        if (r == 0) {
            PIT_X0 = 0;
            PIT_X8 = 1;
            W_STEP++;
        } else if (r == 3) {
            PIT_X0 = 0;
            PIT_X8 = 6;
            W_STEP = 0xD;
        }
        break;
    case 4:
        if ((s8)Lb_talk_check_default(2) != 0) {
            W_STEP++;
            W_SEL = 0;
            W_PAGE = 0;
            lb_set_questpage_info();
            cnWrap_SoundRequest(9);
        }
        break;
    case 5:
        r = lb_select_quest();
        if (r == 0) {
            if (guildPrice > 0) {
                PIT_X0 = 0;
                PIT_X8 = 2;
            } else {
                PIT_X0 = 0;
                PIT_X8 = 9;
            }
            W_STEP++;
        } else if (r == 3) {
            W_STEP = 3;
            W_SEL = 0;
        }
        break;
    case 6:
        if ((s8)Lb_talk_check_default(2) != 0) {
            if (PIT_X9 == 0) {
                if (Lb_check_money(MH_QUEST) == 0) {
                    if (Online_ck() == 0) {
                        row = lobby_g_quest_row(MH_QUEST);
                        *(u8 *)(cw + 0xFA5F) = 1;
                        *(u8 *)(cw + 0xF751) = 1;
                        D_0035EF10 = *(u8 *)(cw + 0xF751);
                        Lb_menu_quest_info(row);
                        PIT_X0 = 0;
                        PIT_X8 = 4;
                        W_STEP = 0xA;
                        *(u16 *)(select_w + 0xD0) = MH_QUEST;
                        {
                            u8 ty = (u8)(Lb_get_quest_type(row) | 0x40);
                            PLAYER(MASTER)[0x916] = ty;                        /* G 0x34DD66 */
                            Lb_set_mini_data(cw + MASTER * 0x5C + 0x3A8, ty, MASTER);
                            memcpy(lbCommer + MASTER * 0x5C + 0x1C, cw + MASTER * 0x5C + 0x3A8, 0x40);
                        }
                    }
#ifdef MHG_ONLINE
                    else {                          /* online: the fee is held until the quest; "recruit members?" */
                        quest_price = *(s32 *)(lobby_g_quest_row(MH_QUEST) + 4);
                        PIT_X8 = 3;
                        PIT_X0 = 0;
                        W_STEP++;
                    }
#endif
                } else {
                    PIT_X0 = 0;
                    PIT_X8 = 7;
                    W_STEP = 0xB;
                    cnWrap_SoundRequest(7);
                }
            } else {
                W_STEP = 0xC;
                PIT_X8 = 5;
                PIT_X9 = 0;
                PIT_X0 = 0;
                cnWrap_SoundRequest(3);
            }
        }
        break;
    case 10:
        if ((s8)Lb_talk_check_default(0) != 0) {
            if (Online_ck() == 0) {
                memcpy(cw + 0x1DC, my_user_mini_data, 0x40);
                memcpy(lbCommer + MASTER * 0x5C + 0x1C, cw + MASTER * 0x5C + 0x3A8, 0x40);
            }
            lb_guild_end();
        }
        break;
    case 11:
        if ((s8)Lb_talk_check_default(0) != 0) {
            PIT_X0 = 0;
            PIT_X8 = 5;
            PIT_X9 = 0;
            W_STEP = 0xC;
        }
        break;
    case 12:
        if ((s8)Lb_talk_check_default(2) != 0) {
            if (PIT_X9 == 0) {
                W_STEP = 5;
                cnWrap_SoundRequest(9);
                Lbc_set_prim(0, Lb_guild_trans, 0, 0);
                cnWrap_SoundRequest(9);
            } else {
                W_STEP = 0xD;
                PIT_X8 = 6;
                PIT_X0 = 0;
                cnWrap_SoundRequest(3);
            }
        }
        break;
    case 13:
        if ((s8)Lb_talk_check_default(0) != 0) {
            lb_guild_end();
        }
        break;
#ifdef MHG_ONLINE
    /* online only (G 0x55AA9C..0x55B1A0): the room. 7 "recruit members?" (yes: the rule sheet, no: a room as it is),
     * 8 the rule sheet, 9 the room made on the server, 15 / 16 the quest sheet of the room one is in, 17 / 18 leaving it */
    case 7:
        if ((s8)Lb_talk_check_default(2) != 0) {
            if (PIT_X9 == 0) {
                W_STEP++;
                cnWrap_SoundRequest(9);
            } else {
                W_STEP = 9;
                SetDialogData_G(0x67);
                cnWrap_SoundRequest(3);
            }
        }
        break;
    case 8:
        if (lb_rule_seet_set() == 1) {
            W_STEP++;
            SetDialogData_G(0x67);
            W_SUB = 0;
        }
        break;
    case 9:
        *(s8 *)(pNet + PN_BUSY) = 1;
        r = lb_guild_make_room();
        if (r == 0) {
            u8 ty;
            row = lobby_g_quest_row(MH_QUEST);
            Lb_menu_quest_info(row);
            ty = (u8)(Lb_get_quest_type(row) | 0x40);
            PLAYER(MASTER)[0x916] = ty;
            Lbc_SendMiniData();
            Lb_set_mini_data(cw + MASTER * 0x5C + 0x3A8, ty, MASTER);
            Lb_set_mini_data(lbCommer + MASTER * 0x5C + 0x1C, ty, MASTER);
            PIT_X0 = 0;
            PIT_X8 = GS8(0x653370) == 0 ? 8 : 4;    /* mhRule+0: members wanted */
            W_STEP++;
            *(u16 *)(select_w + 0xD0) = MH_QUEST;
            *(u16 *)(game_w + 0x2C) = MH_QUEST;
        } else if (r == 1) {
            PIT_X0 = 0;
            PIT_X8 = 6;
            W_STEP = 0xD;
            *(u16 *)(select_w + 0xD0) = MH_QUEST;
            lobby_func_0055A140(0);                 /* the held fee back */
        }
        break;
    case 15:
        if (W_TIMER != 0) {
            W_TIMER--;
            PIT_B = 1;                              /* lb_pit+0xB */
        }
        if ((s8)Lb_talk_check_default(2) != 0) {
            if (PIT_X9 != 0) {
                PIT_X0 = 0;
                PIT_X8 = 6;
                W_STEP = 0xD;
            } else if (*(u8 *)(cw + 0xF751) == 1) {   /* the room is ours (lb_guild_make_room): leave it? */
                PIT_X0 = 0;
                PIT_X8 = 0xB;
                PIT_X9 = 0;
                W_TIMER = 0x14;
                W_STEP++;
                cnWrap_SoundRequest(0);
            } else {
                W_STEP = 0x11;
                SetDialogData_G(0x66);
                cnWrap_SoundRequest(0);
            }
        } else if (Get_sw2(0) & 0x200) {
            if (++W_PAGE >= 3) {
                W_PAGE = 0;
            }
            cnWrap_SoundRequest(6);
        }
        break;
    case 16:
        if (W_TIMER != 0) {
            W_TIMER--;
            PIT_B = 1;
        }
        if ((s8)Lb_talk_check_default(0) != 0) {
            if (PIT_X9 == 0) {
                PIT_X0 = 0;
                PIT_X8 = 0xB;
                PIT_X9 = 1;
                W_STEP++;
                SetDialogData_G(0x66);
            } else {
                PIT_X0 = 0;
                PIT_X8 = 6;
                W_STEP = 0xD;
            }
        }
        break;
    case 17:
        *(s8 *)(pNet + PN_BUSY) = 1;
        if (Lbs_RoomExit() == 1) {
            if (*(u8 *)(cw + 0xF751) == 1) {
                lobby_func_0055A140(0);
                quest_price = 0;
                *(u8 *)(cw + 0xF751) = 0;
            }
            PIT_X0 = 0;
            PIT_X8 = 0xC;
            W_STEP = 0x12;
        }
        break;
    case 18:
        if ((s8)Lb_talk_check_default(0) != 0) {
            lb_guild_end();
        }
        break;
#endif
    default:
        break;
    }
    lb_guild_talk();
}

/* ---------------------------------------------------------------------------------------------- drawing */
typedef struct GSPR { s16 x, y, w, h; s32 col; s16 u0, v0, u1, v1; } GSPR;     /* Put_2TF's sprite: 0x14 bytes */

static int centre_x(const char *s, int x, int size)  /* lobby_func_005935C0: x minus a quarter of the text width */
{
    return (s16)(x - (size * strlen_sp(s)) / 4);
}

void lb_select_quest_level_trans(void)               /* 0x55CD70 (offline part) */
{
    GSPR spr;
    char **names = (char **)GL(W_MODE != 2 ? 0x63687C : 0x6368A4);
    int base = W_MODE != 2 ? 0 : 5;
    int i, y = 0x20, dx, col, n = levels_shown(), y2;
    char *label;
    memcpy(&spr, GL(0x635718), 0x14);
    reload_tex(1, TEX_UI);
    SetTextureStage(TEX_UI);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    flfntSetSize(0x12, 0x12);
    for (i = 0; i < n; i++) {
        dx = i == W_LEVEL ? -0xC : 0;
        col = (*(s8 *)(pNet + PN_LEVEL) >= base + i && (i != 0 || Online_ck() != 0 || key_quest != 0x2711)) ? 5 : 9;
        spr.x += dx;
        Put_2TF(&spr);
        if (i == W_LEVEL) {
            Lb_put_icon(spr.x - 0xA, spr.y + 0xA, 1, Lb_get_cursor_col());
        }
        spr.y += 0x30;
        font_print_double((s16)(dx + 0x157), (s16)(y + 0xA), 1, col, names[i]);
        y2 = y + 0x13;
        if (W_MODE == 2 ? QNEW(base + i) != 0 : QNEW(i) != 0) {
            font_print_double((s16)(dx + 0x222), (s16)y2, 1, 7, GPTR(0x636738));
        } else if (W_MODE == 2 ? QDONE(base + i) != 0 : QDONE(i) != 0) {
            font_print_double((s16)(dx + 0x222), (s16)y2, 1, 2, GPTR(0x636738));
        }
        y = y2 + 0x1D;
        spr.x -= dx;
    }
    /* the key quest row */
    if (key_quest_num != 0) {
        col = (W_MODE == 1 && get_questLevelNum(0xC) == 1) ? 9 : 2;
        label = ((char **)GL(0x6368BC))[1];
    } else {
        col = 9;
        label = ((char **)GL(0x6368BC))[0];
    }
    dx = W_LEVEL == n ? -0xC : 0;
    spr.x += dx;
    Put_2TF(&spr);
    if (W_LEVEL == n) {
        Lb_put_icon(spr.x - 0xA, spr.y + 0xA, 1, Lb_get_cursor_col());
    }
    flfntSetSize(0x12, 0x12);
    font_print_double(centre_x(label, spr.x + spr.w / 2, 0x12), (s16)(y + 9), 1, col, label);
    spr.x -= dx;
}

void guild_trans_ot0(u8 *prim)                        /* 0x55DE50 */
{
    int cc = Lb_get_cursor_col();
    font_set_stack_no(*(s32 *)(prim + 0x18));
    reload_tex(1, TEX_UI);
    SetTextureStage(TEX_UI);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    if (Lbs_InRoomCheck() == 0) {
        Paint_square(0x14A, 0x1B, 0x4E, 0x18, 0x80202020);
    }
    {   /* the frame of the sheet: two sprites of the UI texture (G func_0012E0C0 / E140 on the PS2) */
        GSPR f;
        f.x = 0x146; f.y = 0x32; f.w = 0x122; f.h = 0x132;
        f.col = -1;
        f.u0 = 8; f.v0 = 0x38; f.u1 = 0x86; f.v1 = 0xB8;
        reload_tex(1, TEX_UI);
        SetTextureStage(TEX_UI);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        Put_2TF(&f);
    }
    Put_2TF(GL(0x6356F0));
    Paint_square(0x156, 0x134, 0xF6, 0x18, 0x80000000);
    if (System_timer & 0x10) {
        Lb_put_button(0x1E6, 0x133, 3);
    }
    if (LBS_X68 == 2 && 1) {
        if (*(u8 *)(cw + 0xFA5F) == 0 && QCOUNT(MH_LEVEL) >= 2) {
            Lb_put_icon(0x13E, 0x1A, 0, cc);
            Lb_put_icon(0x18C, 0x1A, 1, cc);
        }
    }
}

void lb_questpage_trans(u8 *prim)                     /* 0x55DA10 */
{
    u8 *row;
    char tmp[0x40];
    int type, i, from = 0, to = 0;
    if (Lbs_InRoomCheck() == 0) {
        row = lobby_g_quest_row(QLIST(MH_LEVEL, W_SEL));
    } else {
        row = lobby_g_quest_row(MH_QUEST);
    }
    font_set_stack_no(*(s32 *)(prim + 0x18));
    if (Quest_clear_bit_ck(*(u16 *)(row + 0x1E)) == 1) {
        flfntSetSize(0x12, 0x12);
        font_set_palette(5);
        flfntLocate(0x154, 0x42);
        font_print_double(0x150, 0x42, 1, 5, GPTR(0x636738));
    }
    Lb_put_gold(0);
    flfntSetSize(0x12, 0x12);
    type = Lb_get_quest_type(row);
    font_print_double(centre_x(GSTR(0x6360D4), 0x1D4, 0x12), GS16(0x6360D2), 1, lb_quest_font_color[type], GSTR(0x6360D4));
    font_print_double(centre_x(GSTR(0x636138), 0x1D4, 0x12), GS16(0x636136), 1, 0, GSTR(0x636138));
    if (Lbs_InRoomCheck() == 0) {
        sprintf(tmp, GSTR(0x63DD38), lb_num_str[(W_SEL + 1) % 10], GS32(0x6365CC), lb_num_str[QCOUNT(MH_LEVEL)]);
        font_print_double(0x154, 0x1E, 1, 6, tmp);
    }
    font_print_double(0x161, 0x136, 1, 6, GPTR(0x63686C));
    switch (W_PAGE) {
    case 0:
        from = 2;
        to = 7;
        font_print_double(0x157, 0x8C, 1, 6, GPTR(0x636848));
        font_print_double(0x157, 0xA0, 1, 6, GPTR(0x63684C));
        font_print_double(0x157, 0xBE, 1, 6, GPTR(0x636850));
        font_print_double(0x157, 0xD2, 1, 6, GPTR(0x636854));
        font_print_double(0x157, 0xF0, 1, 6, GPTR(0x636858));
        font_print_double(0x204, 0x136, 1, 6, GPTR(0x636838));
        break;
    case 1:
        from = 7;
        to = 0xA;
        font_print_double(0x157, 0x8C, 1, 6, GPTR(0x636870));
        font_print_double(0x157, 0xAA, 1, 6, GPTR(0x636874));
        font_print_double(0x157, 0xF0, 1, 6, GPTR(0x636878));
        font_print_double(0x204, 0x136, 1, 6, GPTR(0x636844));
        break;
    case 2:
        from = 0xA;
        to = 0xA;
        font_print_double(0x157, 0xA0, 1, 0, pDetail);
        font_print_double(0x204, 0x136, 1, 6, GPTR(0x636860));
        break;
    }
    for (i = from; i < to; i++) {
        u8 *rec = GL(0x6360D0 + i * 0x64);
        font_print_double(*(s16 *)rec, *(s16 *)(rec + 2), 1, 0, (char *)(rec + 4));
    }
    font_set_palette(0);
    guild_trans_ot0(prim);
}

void Lb_guild_trans(u8 *prim)                         /* 0x55D5A0 */
{
    if (GS8(0x654836) >= 2) {
        font_set_stack_no(*(s32 *)(prim + 0x18));
        font_set_palette(0);
        switch (W_STEP) {
        case 3:
            lb_select_quest_level_trans();
            return;
        case 5:
        case 15:
            lb_questpage_trans(prim);
            return;
#ifdef MHG_ONLINE
        case 8:
            lb_rule_seet_trans(prim);               /* G's (src/lobby/gonl/gen) */
            return;
#endif
        }
    }
}


#endif /* VERSION_MHG */
