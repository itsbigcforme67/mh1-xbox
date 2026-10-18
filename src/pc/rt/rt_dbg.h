/* rt_dbg.h - the game side of the testers' debug menu (rt_dbg.c; the menu itself is src/pc/dbgmenu.c, docs/pc.md
 * "Debug menu"). No game headers here: dbgmenu.c (platform side) includes it too. */
#ifndef RT_DBG_H
#define RT_DBG_H

typedef struct {
    int god;            /* can't faint: the hunter's vital back to full each tick (the RT_PL_GOD path, rt_player.c) */
    int one_hit;        /* every hit on a monster is lethal (rt_dbg_hits) */
    int dmg_mul;        /* damage to monsters times this (1: off; the RT_DMG_MUL path, for every monster) */
    int warp_em_slot;   /* -1, or a monster slot the hunter warps next to on the next player tick (the RT_PL_WARP_EM path) */
    int timer_stop;     /* the quest timer does not run (free roaming) */
    int hitboxes;       /* draw the monsters' body volumes and the attack shells (dbgmenu.c) */
    int kill_req;       /* every living monster of the hunter's area takes a lethal hit on the next tick */
} rt_dbg_opts;
extern rt_dbg_opts rt_dbg;

void rt_dbg_hits(void);         /* after hit_check each quest tick (rt_hit_check): multiplier, one-hit, kill request, timer */

/* the game's tables, read for the menu */
int  rt_dbg_quest_count(void);                  /* offline quests found (MH1 1..0xB1, G's mission file groups) */
int  rt_dbg_quest_no(int i);                    /* the quest number of list entry i */
/* rank: MH1 the Elder's star level 1-5 (0 = not on the offline board); G the file group (0-5). title / goal in Shift-JIS
 * (the game's own text, from the mission file), cached; NULL when the file cannot be read */
int  rt_dbg_quest_info(int i, int *rank, const char **title, const char **goal);
int  rt_dbg_area_list(int *out, float (*dest)[4], int max);  /* the current quest map's areas and an arrival spot (x y z ang) */
void rt_dbg_area_warp(int stage, const float dest[4]);       /* the game's own area change (game2 steps 2-6) */

int  rt_dbg_item_count(void);
const char *rt_dbg_item_name(int id);           /* Shift-JIS, or NULL */
int  rt_dbg_item_max(int id);                   /* the stack limit (Item_data +3), 99 if unknown */
enum { DBG_POUCH, DBG_BOX };
int  rt_dbg_inv_slots(int which);               /* 20 / 100; 0 when not available here */
int  rt_dbg_inv_get(int which, int slot, int *count);   /* the item id */
void rt_dbg_inv_set(int which, int slot, int id, int count);
int  rt_dbg_in_quest(void);                     /* the pouch is the hunter's work (quest), else the saved data's */

int  rt_dbg_money(void);
void rt_dbg_set_money(int v);
int  rt_dbg_hr(void);                           /* User_data hunter rank (MH1 +0x37B, G +0xC23) */
void rt_dbg_set_hr(int v);
int  rt_dbg_clear_all_quests(void);             /* the offline quests marked cleared (MH1: the Elder's board; G: village 1-50 + training); returns how many */

/* equipment: kind 0 melee weapon, 1 gun, 2 head, 3 body, 4 arm, 5 waist, 6 leg */
enum { DBG_EQ_MELEE, DBG_EQ_GUN, DBG_EQ_HEAD, DBG_EQ_BODY, DBG_EQ_ARM, DBG_EQ_WAIST, DBG_EQ_LEG, DBG_EQ_N };
int  rt_dbg_equip_count(int kind);
const char *rt_dbg_equip_name(int kind, int id);
int  rt_dbg_equip_get(int kind);                /* the equipped id, -1 when the weapon is of the other kind */
int  rt_dbg_equip_set(int kind, int id);        /* 0 ok, -1 no room in the equipment box */

void rt_dbg_monsters_hp(int pct);               /* the living monsters of the hunter's area: hp = pct % of their full hp */
int  rt_dbg_monster_slot(void);                 /* the biggest living monster of the hunter's area, -1 none */

/* hit volumes for the overlay: kind 0 monster body, 1 attack shell; a sphere when b == a */
typedef struct { int kind; float a[3], b[3], r; } rt_dbg_vol;
int  rt_dbg_volumes(rt_dbg_vol *out, int max);
#endif
