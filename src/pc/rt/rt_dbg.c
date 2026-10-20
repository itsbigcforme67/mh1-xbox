/*
 * rt_dbg.c - the game side of the testers' debug menu (src/pc/dbgmenu.c draws it; docs/pc.md "Debug menu").
 *
 * Everything here reads or writes the game's own data, as the game's code does it:
 *  - quests: the mission files the game loads (MH1 questName[no], G quest_file_tbl[group][n]), title and goal from the
 *    file's own text pointers (header +0x60 / +0x64; G: the string table at the offset in +0x60); the star level from
 *    the Elder's offline board (quest_local_tbl, MH1);
 *  - areas: the current quest map's exit list (Stage_mv_data_get), the warp is the game's own area change (PLW+0x738 /
 *    +0x73A / +0x73C.., as stage_mv_ck sets it; game2 steps 2-6 load the area);
 *  - items: item_str (names) and Item_data (+3 stack limit); the pouch is PLW+0x828 in a quest (the game copies it from
 *    and back to User_data+0x37C), the item box User_data+0x1C4; G's User_data is re-laid out (UDO, lbsys_g.h);
 *  - equipment: Ken_data / Gun_data / Armor_*_Data names; MH1 equips through the game's own equipment box functions
 *    (Warehouse_equip_stack, Warehouse_equip, Set_equip_idx, f_ud.c), G writes its equipped set (User_data+0xC74..,
 *    the layout G's Get_equip_value reads, ud_nm.c);
 *  - toggles: the RT_PL_GOD / RT_DMG_MUL / RT_PL_WARP_EM paths of rt_player.c, made switchable.
 * Untested on G for everything but the quest list and the toggles (G's village is not playable yet).
 */
#include "lbsys_g.h"
#include "gwx.h"
#include "rt.h"
#include "rt_dbg.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rt_dbg_opts rt_dbg = { 0, 0, 1, -1, 0, 0, 0 };

extern u8 player_work[];
extern u8 em_work[];
extern u8 game_w[];
extern u8 quest_w[];
extern u8 User_data[];
size_t rt_table_size(const char *name);
const char *rt_afs_name(int idx);           /* viewer.c: the AFS entry's file name, NULL past the end */
int rt_village_active(void);
int rt_flow_mode(void);

#define PLW_SIZE 0xA00
#define EMW_SIZE 0xA10
static u8 *master(void) { return player_work + PLW_SIZE * (game_w[GWX(0xD1)] < 10 ? game_w[GWX(0xD1)] : 0); }
static s16 quest_no_now(void) { return *(s16 *)(quest_w + 8); }

/* ------------------------------------------------------------ hits (each quest tick, after hit_check) */
void rt_dbg_hits(void)
{
    int i, k;
    u8 *pl = master();
    for (i = 0; i < 20; i++) {
        u8 *e = em_work + EMW_SIZE * i;
        if (!e[0] || *(s16 *)(e + 0x302) <= 0)
            continue;
        if (rt_dbg.kill_req)
            rt_log("debug menu: monster slot %d kind %d area %d hp %d%s", i, e[2], e[0x736], *(s16 *)(e + 0x302),
                   e[0x736] == pl[0x736] ? ": lethal hit" : "");
        if (rt_dbg.kill_req && e[0x736] == pl[0x736]) {     /* as RT_PL_SLAY: a lethal hit, as if hit_check had found one */
            e[0x38D] = 1;
            *(s16 *)(e + 0x766) = 30000;
            continue;
        }
        if (!e[0x38D])
            continue;
        if (rt_dbg.one_hit) {
            *(s16 *)(e + 0x766) = 30000;
        } else if (rt_dbg.dmg_mul > 1) {
            for (k = 0; k < 8; k++) {
                long v = (long)*(s16 *)(e + 0x766 + 2 * k) * rt_dbg.dmg_mul;
                *(s16 *)(e + 0x766 + 2 * k) = (s16)(v > 30000 ? 30000 : v < -30000 ? -30000 : v);
            }
        }
    }
    if (getenv("RT_RETIRE")) {          /* test aid: abandon the quest at that quest tick (the quest menu's "return to the
                                         * village": Quest_retire_set), offline too (rt_np.c's RT_QUEST_RETIRE is co-op only) */
        static int tk;
        void Quest_retire_set(void);
        if (++tk == atoi(getenv("RT_RETIRE"))) {
            rt_log("test aid: quest tick %d: retire", tk);
            Quest_retire_set();
        }
    }
    if (rt_dbg.kill_req) {
        rt_log("debug menu: the monsters of area %d take a lethal hit", pl[0x736]);
        rt_dbg.kill_req = 0;
    }
    if (rt_dbg.timer_stop) {    /* the time left (quest_w+0x10) held where it was */
        static s32 held = -1;
        static int qn;
        if (held < 0 || qn != quest_no_now())
            held = *(s32 *)(quest_w + 0x10);
        qn = quest_no_now();
        *(s32 *)(quest_w + 0x10) = held;
    }
}

/* ------------------------------------------------------------ quests */
typedef struct { int no, rank, tried; char *title, *goal; } qent;
static qent *quests;
static int nquest = -1;

#ifndef VERSION_MHG
static int mh1_rank(int no)
{
    extern u8 *quest_local_tbl[];
    int l;
    for (l = 0; l < 5; l++) {
        const u8 *t = quest_local_tbl[l];
        int k;
        for (k = 0; t && k < 64 && t[k]; k++)
            if (t[k] == no)
                return l + 1;
    }
    return 0;
}
#endif

static void quests_find(void)
{
    int cap = 64;
    if (nquest >= 0)
        return;
    nquest = 0;
    quests = calloc((size_t)cap, sizeof *quests);
#ifdef VERSION_MHG
    {   /* groups 0-3: number g*1000 + n, 4: 10000 + n (training), 5: 20000 + n; each list holds AFS indexes of mNNNNN.mib */
        extern s16 *quest_file_tbl[];
        int g, n;
        for (g = 0; g < 6; g++)
            for (n = 0; n < 1000 && quest_file_tbl[g]; n++) {
                int no = g < 4 ? g * 1000 + n : (g == 4 ? 10000 : 20000) + n, idx = quest_file_tbl[g][n];
                const char *nm = idx > 0 ? rt_afs_name(idx) : NULL;
                char want[16];
                snprintf(want, sizeof want, "m%05d.mib", no);
                if (!nm) {
                    if (n > 0)
                        break;
                    continue;
                }
                if (strcmp(nm, want))
                    continue;
                if (nquest == cap)
                    quests = realloc(quests, (size_t)(cap *= 2) * sizeof *quests);
                memset(&quests[nquest], 0, sizeof *quests);
                quests[nquest].no = no;
                quests[nquest++].rank = g;
            }
    }
#else
    {
        extern s16 questName[];
        int no, n = (int)(rt_table_size("questName") / 2);
        for (no = 1; no < n && no < 0xB2; no++) {
            if (questName[no] <= 0)
                continue;
            if (nquest == cap)
                quests = realloc(quests, (size_t)(cap *= 2) * sizeof *quests);
            memset(&quests[nquest], 0, sizeof *quests);
            quests[nquest].no = no;
            quests[nquest++].rank = mh1_rank(no);
        }
    }
#endif
    {   /* the Elder's board first (by star level), then the rest (guild / online quests) by number */
        int i, j;
        for (i = 1; i < nquest; i++)
            for (j = i; j > 0; j--) {
                qent *a = &quests[j - 1], *b = &quests[j], t;
                int ka = a->rank ? a->rank : 99, kb = b->rank ? b->rank : 99;
                if (rt_ver_mhg() || ka < kb || (ka == kb && a->no <= b->no))
                    break;
                t = *a;
                *a = *b;
                *b = t;
            }
    }
    rt_log("debug menu: %d quests found", nquest);
}

int rt_dbg_quest_count(void) { quests_find(); return nquest; }
int rt_dbg_quest_no(int i) { quests_find(); return i >= 0 && i < nquest ? quests[i].no : 0; }

static char *dup_str(const u8 *buf, size_t n, u32 off)
{
    size_t l = 0;
    char *s;
    if (off >= n)
        return NULL;
    while (off + l < n && buf[off + l] && l < 200)
        l++;
    s = malloc(l + 1);
    memcpy(s, buf + off, l);
    s[l] = 0;
    for (l = 0; s[l]; l++)          /* one line: the game's line feeds become spaces */
        if (s[l] == '\n')
            s[l] = ' ';
    return s;
}

uint8_t *rt_file_load(int idx, size_t *n);
int rt_dbg_quest_info(int i, int *rank, const char **title, const char **goal)
{
    qent *q;
    quests_find();
    if (i < 0 || i >= nquest)
        return -1;
    q = &quests[i];
    if (!q->tried) {
        size_t n = 0;
        uint8_t *b;
        int idx;
        q->tried = 1;
#ifdef VERSION_MHG
        {
            extern s16 *quest_file_tbl[];
            int g = q->no < 10000 ? q->no / 1000 : q->no < 20000 ? 4 : 5;
            idx = quest_file_tbl[g][q->no % 1000];
        }
#else
        {
            extern s16 questName[];
            idx = questName[q->no];
        }
#endif
        if ((b = rt_file_load(idx, &n)) != NULL && n > 0x70) {
            u32 a, c;
            memcpy(&a, b + 0x60, 4);
#ifdef VERSION_MHG
            if (a + 8 <= n) {               /* G: +0x60 points at the table of the strings' offsets */
                memcpy(&c, b + a + 4, 4);
                memcpy(&a, b + a, 4);
                q->title = dup_str(b, n, a);
                q->goal = dup_str(b, n, c);
            }
#else
            memcpy(&c, b + 0x64, 4);
            q->title = dup_str(b, n, a);
            q->goal = dup_str(b, n, c);
#endif
        }
        free(b);
    }
    *rank = q->rank;
    *title = q->title;
    *goal = q->goal;
    return q->no;
}

/* ------------------------------------------------------------ areas of the current quest's map */
typedef struct {
    u16 id;
    s16 kind;
    f32 pos[3], r, h, box[3], dest[3];
    u16 ang;
    u8 _pad[2];
} DBG_STG_MV;          /* STG_MV (f_stage.c) */
void *Stage_mv_data_get(int n, int pl);
#ifdef VERSION_MHG
#define STAGES 92           /* STG_POS_N (version.h) */
#else
#define STAGES 88
#endif

int rt_dbg_area_list(int *out, float (*dest)[4], int max)
{
    int n = 0, st, k;
    if (rt_flow_mode() != 2 || quest_no_now() == 0)
        return 0;
    for (st = 0; st < STAGES; st++) {
        DBG_STG_MV *p;
        int m = 0;
#ifdef VERSION_MHG
        p = Stage_mv_data_get(st, game_w[GWX(0xD1)]);     /* G: through the map's stage list (0 for a stage not on it) */
#else
        {   /* MH1: the table has a word per stage, an offset into the mission file (Stage_mv_data_get); checked here,
             * as the menu also asks for stages the quest's map does not have */
            extern u8 *mission_area;
            s32 v = (*(s32 **)(quest_w + 0x7C))[st];
            p = v > 0 && v < 0x3FF00 ? (DBG_STG_MV *)(mission_area + v) : NULL;
        }
#endif
        for (; p && p->id != 0xFFFF && p->id < 0x100 && m < 32; p++, m++) {
            for (k = 0; k < n && out[k] != p->id; k++)
                ;
            if (k < n || n >= max)
                continue;
            out[n] = p->id;
            dest[n][0] = p->dest[0];
            dest[n][1] = p->dest[1];
            dest[n][2] = p->dest[2];
            dest[n][3] = (float)(u16)(p->ang + 0x4000);
            n++;
        }
    }
    for (st = 1; st < n; st++)          /* by area number */
        for (k = st; k > 0 && out[k] < out[k - 1]; k--) {
            int t = out[k];
            float d[4];
            out[k] = out[k - 1];
            out[k - 1] = t;
            memcpy(d, dest[k], sizeof d);
            memcpy(dest[k], dest[k - 1], sizeof d);
            memcpy(dest[k - 1], d, sizeof d);
        }
    return n;
}

void Pl_ofs_set(void *pl, f32 *pos, u16 ang);
void rt_dbg_area_warp(int stage, const float dest[4])
{
    u8 *pl = master();
    pl[0x738] = 1;                      /* as stage_mv_ck does at an exit */
    *(u16 *)(pl + 0x73A) = (u16)stage;
    *(f32 *)(pl + 0x73C) = dest[0];
    *(f32 *)(pl + 0x740) = dest[1];
    *(f32 *)(pl + 0x744) = dest[2];
    *(u16 *)(pl + 0x570) = (u16)dest[3];
    Pl_ofs_set(pl, (f32 *)(pl + 0x73C), (u16)dest[3]);
    rt_log("debug menu: area change to stage %d (%.0f %.0f %.0f)", stage, dest[0], dest[1], dest[2]);
}

/* ------------------------------------------------------------ items */
extern char *item_str[];
extern u8 Item_data[];
#ifdef VERSION_MHG
#define ITEM_ROW 20
#else
#define ITEM_ROW 16
#endif
int rt_dbg_item_count(void)
{
    int a = (int)(rt_table_size("item_str") / 4), b = (int)(rt_table_size("Item_data") / ITEM_ROW);
    return a && b ? (a < b ? a : b) : a ? a : b;
}
const char *rt_dbg_item_name(int id)
{
    return id > 0 && id < (int)(rt_table_size("item_str") / 4) ? item_str[id] : NULL;
}
int rt_dbg_item_max(int id)
{
    int m = id > 0 && id < (int)(rt_table_size("Item_data") / ITEM_ROW) ? Item_data[ITEM_ROW * id + 3] : 0;
    return m == 0 || m == 0xFF ? 99 : m;
}

int rt_dbg_in_quest(void) { return !rt_village_active() && rt_flow_mode() >= 2 && rt_flow_mode() <= 5; }

static u8 *inv(int which, int *n)
{
    if (which == DBG_POUCH) {
        *n = 20;
        return rt_dbg_in_quest() ? master() + 0x828 : User_data + UDO(0x37C);
    }
    *n = 100;
    return User_data + UDO(0x1C4);
}
int rt_dbg_inv_slots(int which) { int n; inv(which, &n); return n; }
int rt_dbg_inv_get(int which, int slot, int *count)
{
    int n;
    u8 *p = inv(which, &n);
    if (slot < 0 || slot >= n) {
        *count = 0;
        return 0;
    }
    *count = *(s16 *)(p + 4 * slot + 2);
    return *(u16 *)(p + 4 * slot);
}
void rt_dbg_inv_set(int which, int slot, int id, int count)
{
    int n;
    u8 *p = inv(which, &n);
    if (slot < 0 || slot >= n)
        return;
    if (id <= 0 || count <= 0)
        id = count = 0;
    *(u16 *)(p + 4 * slot) = (u16)id;
    *(s16 *)(p + 4 * slot + 2) = (s16)count;
    rt_log("debug menu: %s slot %d = item %d x %d", which == DBG_POUCH ? (rt_dbg_in_quest() ? "pouch (quest)" : "pouch (saved data)") : "item box",
           slot, id, count);
}

int rt_dbg_money(void) { return *(s32 *)(User_data + 0x20); }
void rt_dbg_set_money(int v)
{
    *(s32 *)(User_data + 0x20) = v < 0 ? 0 : v > 9999999 ? 9999999 : v;     /* Gold_add's range */
    rt_log("debug menu: money %d", *(s32 *)(User_data + 0x20));
}

/* hunter rank: User_data+0x37B (MH1), +0xC23 (G, include/ud.h UDW). Both rows were MH1-only until the cross-game
 * audit of 18 Oct 2026 (G showed neither) */
#ifdef VERSION_MHG
#define UD_RANK 0xC23
#else
#define UD_RANK 0x37B
#endif
int rt_dbg_hr(void) { return User_data[UD_RANK]; }
void rt_dbg_set_hr(int v)
{
    User_data[UD_RANK] = (u8)(v < 0 ? 0 : v > 99 ? 99 : v);
    rt_log("debug menu: hunter rank %d", User_data[UD_RANK]);
}
void Quest_clear_bit_set(int n);
#ifdef VERSION_MHG
int rt_dbg_clear_all_quests(void)
{   /* G's clear bits follow quest_bit_tbl (src/main/ud/ud_nm.c: 0..50, the 1001-3007 online series, 10001.. training, 20001..; 0xEA60
     * ends it): the offline ones are the village quests 1-50 and the training quests 10001.. */
    extern u16 quest_bit_tbl[];
    const u16 *p;
    int n = 0;
    for (p = quest_bit_tbl; *p != 0xEA60; p++)
        if ((*p >= 1 && *p < 1000) || (*p >= 10000 && *p < 20000)) {
            Quest_clear_bit_set(*p);
            n++;
        }
    rt_log("debug menu: %d offline quests marked cleared", n);
    return n;
}
#else
int rt_dbg_clear_all_quests(void)
{
    extern u8 *quest_local_tbl[];
    int l, k, n = 0;
    for (l = 0; l < 5; l++)
        for (k = 0; quest_local_tbl[l] && k < 64 && quest_local_tbl[l][k]; k++, n++)
            Quest_clear_bit_set(quest_local_tbl[l][k]);
    rt_log("debug menu: %d offline quests marked cleared", n);
    return n;
}
#endif

/* ------------------------------------------------------------ equipment */
extern u8 Ken_data[], Gun_data[], Armor_Head_Data[], Armor_Body_Data[], Armor_Arm_Data[], Armor_Waist_Data[], Armor_Leg_Data[];
#ifdef VERSION_MHG
#define GUNROW 0x18
#define GUNNAME 0x10
#define ARMROW 0x20
#else
#define GUNROW 0x14
#define GUNNAME 0x0C
#define ARMROW 0x14
#endif
static const struct { const char *sym; u8 *tbl; int row, name; } eq[DBG_EQ_N] = {
    { "Ken_data", Ken_data, 0x18, 0x14 }, { "Gun_data", Gun_data, GUNROW, GUNNAME },
    { "Armor_Head_Data", Armor_Head_Data, ARMROW, 0x10 }, { "Armor_Body_Data", Armor_Body_Data, ARMROW, 0x10 },
    { "Armor_Arm_Data", Armor_Arm_Data, ARMROW, 0x10 }, { "Armor_Waist_Data", Armor_Waist_Data, ARMROW, 0x10 },
    { "Armor_Leg_Data", Armor_Leg_Data, ARMROW, 0x10 },
};
int rt_dbg_equip_count(int kind)
{
    return kind >= 0 && kind < DBG_EQ_N ? (int)(rt_table_size(eq[kind].sym) / (size_t)eq[kind].row) : 0;
}
const char *rt_dbg_equip_name(int kind, int id)
{
    if (id < 0 || id >= rt_dbg_equip_count(kind))
        return NULL;
    return *(const char **)(eq[kind].tbl + eq[kind].row * id + eq[kind].name);
}

/* the equipped set: MH1 User_data+0x3CD weapon kind (6 melee, 7 gun), +0x3CE id, +0x3D2 armour leg, head, body, arm,
 * waist; G +0xC75, +0xC76, +0xC7A.. (same order) */
#ifdef VERSION_MHG
#define EQ_BASE 0xC74
#else
#define EQ_BASE 0x3CC
#endif
static const int arm_ofs[DBG_EQ_N] = { 0, 0, 1, 2, 3, 4, 0 };      /* head body arm waist leg -> armour byte (leg first) */
int rt_dbg_equip_get(int kind)
{
    u8 *e = User_data + EQ_BASE;
    if (kind == DBG_EQ_MELEE || kind == DBG_EQ_GUN)
        return e[1] == (kind == DBG_EQ_GUN ? 7 : 6) ? *(u16 *)(e + 2) : -1;
    return e[6 + arm_ofs[kind]];
}

#ifndef VERSION_MHG
u8 Warehouse_equip_stack(void *u, s8 kind, s16 id, s16 opt);
int Warehouse_equip(void *u, u8 idx);
void Set_equip_idx(void *u);
#endif
int rt_dbg_equip_set(int kind, int id)
{
    static const int game_kind[DBG_EQ_N] = { 6, 7, 2, 3, 4, 5, 0 };     /* the equipment box's kinds */
#ifdef VERSION_MHG
    u8 *e = User_data + EQ_BASE;
    if (kind == DBG_EQ_MELEE || kind == DBG_EQ_GUN) {
        e[1] = (u8)game_kind[kind];
        *(u16 *)(e + 2) = (u16)id;
        *(u16 *)(e + 4) = 0;
    } else
        e[6 + arm_ofs[kind]] = (u8)id;
#else
    u8 idx = Warehouse_equip_stack(User_data, (s8)game_kind[kind], (s16)id, 0);
    if (idx == 0xFF) {
        rt_log("debug menu: the equipment box is full (64)");
        return -1;
    }
    Warehouse_equip(User_data, idx);
    Set_equip_idx(User_data);
#endif
    rt_log("debug menu: equipped kind %d id %d", game_kind[kind], id);
    return 0;
}

/* ------------------------------------------------------------ monsters */
static int em_live_here(const u8 *e)
{
    return e[0] && *(s16 *)(e + 0x302) > 0 && e[0x736] == master()[0x736];
}
void rt_dbg_monsters_hp(int pct)
{
    int i;
    for (i = 0; i < 20; i++) {
        u8 *e = em_work + EMW_SIZE * i;
        int full = *(s16 *)(e + 0x792), v;
        if (!em_live_here(e))
            continue;
        if (full < *(s16 *)(e + 0x302))
            full = *(s16 *)(e + 0x302);
        v = full * pct / 100;
        *(s16 *)(e + 0x302) = (s16)(v < 1 ? 1 : v);
        rt_log("debug menu: monster slot %d (kind %d) hp %d", i, e[2], *(s16 *)(e + 0x302));
    }
}
int rt_dbg_monster_slot(void)
{
    int i, best = -1, bhp = 0;
    for (i = 0; i < 20; i++) {
        u8 *e = em_work + EMW_SIZE * i;
        int full = *(s16 *)(e + 0x792);
        if (!em_live_here(e))
            continue;
        if (full < *(s16 *)(e + 0x302))
            full = *(s16 *)(e + 0x302);
        if (best < 0 || full > bhp) {
            best = i;
            bhp = full;
        }
    }
    return best;
}

/* ------------------------------------------------------------ hit volumes */
int hit_data_expand(void *chr, void *body, f32 *cap, f32 *sph);
int rt_dbg_volumes(rt_dbg_vol *out, int max)
{
    extern u8 *em_body_tbl[];
    extern u8 *shell_w_top;
    int n = 0, i;
    f32 cap[8], sph[4];
    u8 *b, *sh;
    if (rt_village_active())
        return 0;
    for (i = 0; i < 20 && n < max; i++) {
        u8 *e = em_work + EMW_SIZE * i;
        if (!e[0] || e[0x736] != master()[0x736] || !em_body_tbl[e[2]])
            continue;
        for (b = em_body_tbl[e[2]]; *(s16 *)b != -1 && n < max; b += 0x28) {
            int k = hit_data_expand(e, b, cap, sph);
            out[n].kind = 0;
            if (k == 0 && sph[3] > 0) {
                memcpy(out[n].a, sph, 12);
                memcpy(out[n].b, sph, 12);
                out[n++].r = sph[3];
            } else if (k == 1) {
                memcpy(out[n].a, cap, 12);
                memcpy(out[n].b, cap + 3, 12);
                out[n++].r = cap[6];
            }
        }
    }
    for (sh = shell_w_top; sh && n < max; sh = *(u8 **)(sh + 0x10)) {     /* live attack shells (as RT_HIT_DM=2 lists them) */
        if (!sh[0] || sh[0xB] != 2 || !*(u8 **)(sh + 0x88))
            continue;
        for (b = *(u8 **)(sh + 0x88); *(s16 *)b != -1 && n < max; b += 0x28) {
            int k = hit_data_expand(player_work + PLW_SIZE * sh[0xA], b, cap, sph);
            out[n].kind = 1;
            if (k == 0 && sph[3] > 0) {
                memcpy(out[n].a, sph, 12);
                memcpy(out[n].b, sph, 12);
                out[n++].r = sph[3];
            } else if (k == 1) {
                memcpy(out[n].a, cap, 12);
                memcpy(out[n].b, cap + 3, 12);
                out[n++].r = cap[6];
            }
        }
    }
    return n;
}
