/* f_quest_nm - near-match C of the 13 functions of the quest file (src/main/quest/f_quest.c, 0x226C30-0x22C66C)
 * that are kept as original bytes on the PS2 (config/c_rawfuncs.txt). The PC build links this file weak, the
 * matched functions come from f_quest.c. Meanings are guesses. */
#include "gwx.h"
#include "quest.h"
#include "plf.h"

typedef char *va_list;
void str_gattai(char *dst, char *fmt, ...);
QEM *em_work_serch2(s16, s16);
u16 stolen_item_stack(int, s16);
void Quest_start();
void Quest_retire_set();
void Quest_error_set2();
void Quest_error_set();
void Quest_restart();
void Quest_em_init_set();
void Quest_enemy_die();
void Quest_enemy_escape();
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
EMW * Em_direct_set();
void Quest_next_em_clr();
char * Quest_str_get();
void Item_stolen();
void Item_regained();
void Share_item_conv();
u16 Ext_pick_point_cnt_ck();
void Ext_pick_point_st();
u16 Ext_pick_point_ck();
void Ext_pick_point_st_clr();
s8 Em_hagi_point_set();
void Quest_forfeit_message();
QEM * em_work_serch();
void Quest_timer_reset();
void remuneration_item_set();
u16 * quest_supplies_get();
s16 quest_enemy_ck2();
void quest_presuccess_ptr_set();
void Quest_net_sub();
void quest_em_die();
void quest_timer_send();

extern s16 questName[];
int func_63AF40();
void load_file_mdl();
void Quest_pl_stage_init();
int Quest_time_get();
void quest_em_init();
void Ext_pick_point_init();
unsigned char Get_hunter_rank();
int Online_ck();
void net_send_sys();
void station_em_set();
void Quest_next_em_set();
QEM *em_work_serch();
void q_net_send_em_die();
void q_net_send_em_capture();
int quest_enemy_ck_sub();
int em_capture_conv();
void quest_failed_ptr_set();
int Quest_clear_ck();

extern char lit_656_0036B1F0[];
extern char lit_657_0036B210[];
extern char lit_658_0036B230[];
extern char lit_659_0036B250[];
extern char lit_660_0036B270[];
s16 quest_condition_prog();
void Quest_timer_calc();

void func_63ACA0();
void set01_set();
void set01_set2();

#define EM8(e, o) (*(s8 *)((u8 *)(e) + (o)))
void *pull_enemy_work();
void enemy_mv();

s32 *Em_data_com_adrs_get();
s32 *Em_data_st_adrs_get(s32 *, int, int, s8);
void func_5A8170();
void push_em_work();
void release_enemy_model();
void enemy_insurance_sub();
#define PL8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PL32(p, o) (*(s32 *)((u8 *)(p) + (o)))

void em_next_tbl_ck();
void func_5589F0();
void em_create_model();
void em_herb_set();
EMW *Em_direct_set();

extern s8 item_regained_tbl[8];
char *func_5C5E20();
void adx_se_set();
s16 Share_item_num_ck();
int share_item_ck_ck();
s16 Quest_share_item_num_ck();
int Share_item_stack();

typedef struct HAGI {
    u16 id;             /* 0x00 */
    u16 cnt;            /* 0x02 */
    f32 rad;            /* 0x04 */
    s16 x08;            /* 0x08 */
    u8 x0A;             /* 0x0A */
    u8 joint;           /* 0x0B */
} HAGI;
extern HAGI *em_hagi_type_tbl[];
void get_joint_pos_em();
void ext_pick_point_fifo_ck();
void ext_pick_point_tbl_set();
void ext_pick_point_tbl_clr_ex();
void ext_pick_point_tbl_clr();
void Ext_pick_point_clr();
void Ext_pick_point_pos();
void Em_hagi_point_clr();
f32 flSqrt(f32);
unsigned short Item_get_ck();

int strlen();
int vsprintf();
void set01_set2_use_mem();

extern char lit_1730_0036B290[];

void em_data_st_adrs_set();

void quest_em_init_sub();
void quest_em_init_sub2();

int Quest_f_dra_ck();

extern u16 capture_type_tbl[];
short Pl_item_num_ck();
void em_herb_set();
int quest_enemy_ck_sub2();
void func_535D20();

extern s32 quest_timer_disp_tbl[][2];
extern char lit_2393[];
extern char lit_2394[];
extern char lit_2395[];
extern char lit_2396[];
void set01_set2();
void QuestClearCameraRequest();
int func_53B5C0();

int Net_Share_item_stack();

extern u16 rem_item_calc_tbl[];
extern s8 *rem_item_exit_sel_tbl[];

/* quest_w.x88 entry (8 bytes): kind, unused, offset of its item table in mission_area. */
typedef struct QREM {
    u16 id;             /* 0x00 condition kind, 0xFFFF ends the list */
    u16 x02;            /* 0x02 */
    s32 tbl;            /* 0x04 offset of the REMI table */
} QREM;

/* Reward item table entry (6 bytes), table ends with weight 0xFFFF. */
typedef struct REMI {
    u16 w;              /* 0x00 weight */
    u16 id;             /* 0x02 item */
    u16 num;            /* 0x04 count */
} REMI;


















#ifdef VERSION_MHG
void em_size_scale_set(EMW *em);
#endif
EMW *Em_direct_set(QEM *q)
{
    EMW *em;
    int i;
    u8 *p;

    for (i = 0, p = (u8 *)&game_w; i < 4; i++, p++) {
        if (q->id == p[0x28]) {
            break;
        }
    }
    if (i >= 4) {
        return 0;
    }
    em = pull_enemy_work(p);
    if (em != 0) {
        em->mdl_no = i;
        em->kind = q->id;
        em->type = q->x02;
        EM8(em, 0x9EB) = q->x2C;
        em->stg = game_w.stage;
        em->hungry = q->x0C;
        em->thirst = q->x10;
        em->x8A0 = q->x14;
        EM8(em, 0x95B) = q->x06;
        em->pos[0] = q->pos[0];
        em->pos[1] = q->pos[1];
        em->pos[2] = q->pos[2];
        em->ang[1] = q->x1C;
#ifdef VERSION_MHG
        em->size = q->x30;      /* G 0x4B5790: the quest's size for this monster */
#endif
        enemy_mv(em);
        q->x0A = em->id;
#ifdef VERSION_MHG
        em_size_scale_set(em);
#endif
        if (q->x08 == -1) {
            q->x08 = em->x302;
            if (q->x05 != 0) {
                quest_w.xB0 = em;
            }
        } else {
            em->x302 = q->x08;
        }
    }
    return em;
}


void Quest_next_em_set(n)
int n;
{
    QEM *l1;
    s32 *l0;
    int i;
    int v;
    u8 *g;
    int j;
    u8 *h;

    em_next_tbl_ck();
    func_5589F0(n);
    if (n == game_w.x2F) {
        for (g = (u8 *)&game_w, i = 0; i < 4; i++, g++) {
            if (g[0x28] <= 0) {
                game_w.x28[i] = 0x12;
                em_create_model(i);
                break;
            }
        }
    }
    if (n == 5 || n == 0x10 || n == 0x29) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (g[0x28] <= 0) {
                game_w.x28[i] = 0xA;
                em_create_model(i);
                break;
            }
        }
    }
    l0 = Em_data_st_adrs_get(quest_w.x74, n, 0, quest_w.x3A);
    l1 = (QEM *)Em_data_st_adrs_get(quest_w.x74, n, 1, quest_w.x3A);
    if (l0 != 0) {
        for (;;) {
            v = *l0;
            if (*l0 == -1) {
                break;
            }
            for (g = (u8 *)&game_w, i = 0; i < 4; i++, g++) {
                if (g[0x28] == v) {
                    break;
                }
            }
            if (i < 4) {
                l0++;
                continue;
            }
            for (j = 0, h = (u8 *)&game_w; j < 4; j++, h++) {
                if (h[0x28] <= 0) {
                    ((u8 *)&game_w)[0x28 + j] = v;
                    em_create_model(j);
                    break;
                }
            }
            l0++;
        }
        for (;;) {
            if (l1->id < 0) {
                break;
            }
            if (!(l1->x2E & 1)) {
                Em_direct_set(l1);
                l1->x07 = game_w.stage;
            }
            l1++;
        }
    }
    if (n == 5 || n == 0x10 || n == 0x29) {
        em_herb_set();
    }
}





void Item_regained(pl, unused)
PLW *pl;
int unused; /* plf.h: callers pass a second argument the body does not read */
{
    int i;
    int found = 0;
    int r;
    u8 *p;

    if (quest_w.xAD != 0) {
    r = item_regained_tbl[(u16)ran_suu(0) & 7];
    if (r != 2) {
        if (r != 1) {
            for (i = 0, p = (u8 *)&quest_w; i < 5; i++, p += 4) {
                if (*(u16 *)(p + 0x98) != 0) {
                    switch ((u16)Pl_item_stack(pl, *(u16 *)(p + 0x98), *(s8 *)(p + 0x9A))) {
                    case 0:
                    case 1:
                    case 2:
                        if (Pl_master_ck(pl) == 1) {
                            set01_set(1, 0, *(s16 *)(p + 0x98));
                        }
                        found = 1;
                        break;
                    case 3:
                        if (Pl_master_ck(pl) == 1) {
                            set01_set(1, 3, *(s16 *)(p + 0x98));
                        }
                        found = 1;
                        break;
                    }
                }
            }
            if (found == 0 && Pl_master_ck(pl) == 1) {
                set01_set(0, 0, 0);
            }
        } else {
            switch ((u16)Pl_item_stack(pl, 0x59, 1)) {
            case 0:
            case 1:
            case 2:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(1, 0, 0x59);
                }
                break;
            case 3:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(1, 3, 0x59);
                }
                break;
            default:
                if (Pl_master_ck(pl) == 1) {
                    set01_set(0, 0, 0);
                }
                break;
            }
        }
    } else if (Pl_master_ck(pl) == 1) {
        set01_set(0, 0, 0);
    }
    quest_w.x98[0].v = 0;
    quest_w.x98[0].f = 0;
    quest_w.x98[1].v = 0;
    quest_w.x98[1].f = 0;
    quest_w.x98[2].v = 0;
    quest_w.x98[2].f = 0;
    quest_w.x98[3].v = 0;
    quest_w.x98[3].f = 0;
    quest_w.x98[4].v = 0;
    quest_w.x98[4].f = 0;
    quest_w.xAC = 0;
    quest_w.xAD = 0;
    return;
    }
    if (Pl_master_ck(pl) == 1) {
        set01_set(0, 0, 0);
    }
}

int Share_item_stack(pl, item, num)
PLW *pl;
int item;
s16 num;
{
    s16 room;
    s16 i;
    int have;
    s16 base;

    have = (s16)Share_item_num_ck(item, share_item_ck_ck());
    room = quest_w.x24[Quest_share_item_num_ck(item)] - have;
    if (room <= 0) {
        return 0;
    }
    i = 0;
    if (Share_item_num_ck(item, 1) == 0) {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == 0) {
                pl->share[i].id = item;
                if (num >= room) {
                    num = room;
                }
                pl->share[i].num = (s8)num;
                Pl_item_stack(pl, item, -num);
                return (u16)num;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == (u16)item) {
                if (num >= room) {
                    num = room;
                }
                pl->share[i].num += (s8)num;
                Pl_item_stack(pl, item, -num);
                return (u16)num;
            }
        }
    }
    return (u16)num;
}

int Net_Share_item_stack(pl, item, num)
PLW *pl;
int item;
s16 num;
{
    s16 room;
    s16 i;
    int have;
    s16 cur;
    int r;

    have = (s16)Share_item_num_ck(item, share_item_ck_ck());
    room = quest_w.x24[Quest_share_item_num_ck(item)] - have;
    if (room <= 0) {
        return 2;
    }
    cur = 0;
    for (i = 0; i < 4; i++) {
        if (pl->share[i].id == (u16)item) {
            cur = pl->share[i].num;
        }
    }
    r = 0;
    if (cur == 0) {
        for (i = 0; i < 4; i++) {
            if (pl->share[i].id == 0) {
                pl->share[i].id = item;
                if (num >= room) {
                    r = 2;
                }
                pl->share[i].num = (s8)num;
                return r;
            }
        }
        return 0;
    }
    r = 1;
    for (i = 0; i < 4; i++) {
        if (pl->share[i].id == (u16)item) {
            if (num >= room) {
                r = 2;
            }
            pl->share[i].num = (s8)num;
            return r;
        }
    }
    return 1;
}

s16 Share_item_num_ck(item, mode)
int item;
int mode;
{
    int i;
    int j;
    s16 sum = 0;
    PLW *pl;

    switch (mode) {
    case 0:
        for (i = 0; i < game_w.pl_num; i++) {
            pl = &player_work[i];
            for (j = 0; j < 4; j++) {
                if (pl->share[j].id == (u16)item) {
                    sum += pl->share[j].num;
                }
            }
        }
        return sum;
    case 1:
        pl = &player_work[game_w.master];
        for (j = 0; j < 4; j++) {
            if (pl->share[j].id == (u16)item) {
                return pl->share[j].num;
            }
        }
        return 0;
    default:
        pl = &player_work[mode - 2];
        for (j = 0; j < 4; j++) {
            if (pl->share[j].id == (u16)item) {
                return pl->share[j].num;
            }
        }
        return 0;
    }
}





int Em_hagi_point_cnt_ck(em)
EMW *em;
{
    s8 n;
    f32 jp[3];
    int r;

    r = -1;
    n = em->x88D;
    if (n != -1) {
        if ((s32)StiEM_data[n].cnt <= 0) {
            Em_hagi_point_clr(em);
            return -1;
        }
        if (em->x876 != 0) {
            get_joint_pos_em(em, em->x876, jp);
            jp[1] = em->x5AC;
            Ext_pick_point_pos(em->x88D, jp);
        } else {
            Ext_pick_point_pos(n, em->pos);
        }
        r = StiEM_data[em->x88D].cnt;
        return r;
    }
    return r;
}













#if defined(VERSION_MHG) && !defined(__MWERKS__)
void rt_cap_request(void);              /* src/pc/rt/rt_2d.c: G's quest clear frame copy */
#endif
s16 quest_condition_prog(pl, i)
PLW *pl;
s16 i;
{
    QCMD *p;
    QCMD *q;
    char buf[0x20];
    int n;

    if (game_w.x0D5 == 2 && quest_w.x10 >= 0) {
        if (quest_w.xAF > 0) {
            s32 t;
            t = quest_timer_disp_tbl[quest_w.xAF][0];
            if (!(t < quest_w.x10)) {
                str_gattai(buf, lit_2393, t / 1800);
                n = strlen(buf);
                if (n & 1) {
                    buf[n] = 0x20;
                    buf[n + 1] = 0;
                }
                buf[0x1F] = 0;
                set01_set2_use_mem(buf);
                quest_w.xAF--;
            }
        }
        Quest_timer_calc(1);
        if (quest_w.x10 <= 0) {
            s32 t;
            quest_w.x10 = 0;
            q = (QCMD *)quest_w.x6C;
            i = 0;
            quest_timer_send();
            quest_w.x10 = -1;
            t = quest_w.x0A;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
        } else if (quest_w.x10 % 9000 == 0) {
            quest_timer_send();
        }
    }
    for (;;) {
again:
        if (quest_w.x0B != 0 && quest_share_item_ck(&quest_w)) {
            s32 t;
            i = 0;
            q = (QCMD *)quest_w.x6C;
            t = quest_w.x0B;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
            quest_w.x0B = 0;
        }
        if (quest_w.x14F != 0 && quest_w.x150 != 0) {
            s32 t;
            t = (s16)quest_w.x14F;
            q = (QCMD *)quest_w.x6C;
            i = 0;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    break;
                }
                i++;
                q++;
            }
            quest_w.x14F = 0;
        }
        q = (QCMD *)quest_w.x6C;
        p = q + i;
        switch (p->cmd) {
        case 5:
            quest_w.x0C = p->a;
            i++;
            break;
        case 6: {
            s32 t;
            t = quest_w.x0C - 1;
            quest_w.x0C = t;
            if (t > 0) {
                break;
            }
            i++;
            break;
        }
        case 7:
            set01_set2(quest_w.x70[p->a] + (int)mission_area);
            i++;
            break;
        case 0:
            if (quest_w.x34 > 0) {
                break;
            }
            i++;
            goto again;
        case 1:
            if (quest_enemy_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x24: {
            s16 r;
            r = quest_enemy_ck2(&quest_w, *((u8 *)p + 2));
            if (p->b < r) {
                break;
            }
            i++;
            goto again;
        }
        case 2:
            quest_enemy_set(p->a, p->b);
            i++;
            break;
        case 0x1D:
            if (quest_item_ck2(&quest_w, p) == 0) {
                break;
            }
            i++;
            goto again;
        case 3:
            if (quest_item_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 8:
            if (quest_item_ck(&quest_w) == 0) {
                i = quest_w.x38;
                break;
            }
            if (pl->stg != p->a) {
                break;
            }
            i++;
            goto again;
        case 4:
            quest_item_set(p->a, p->b);
            i++;
            break;
        case 0xA: {
            s32 t;
            t = p->a;
            p = (QCMD *)((u8 *)p + 4);
            switch (t) {
            case -1:
                quest_supplies_get(pl, p);
                break;
            case -3:
                if (pl->kind == 1 || pl->kind == 5) {
                    quest_supplies_get(pl, p);
                }
                break;
            default:
                if (pl->kind == t) {
                    quest_supplies_get(pl, p);
                }
                break;
            }
            i++;
            break;
        }
        case 0xB:
            quest_w.x38 = i + 1;
            i++;
            break;
        case 0xD:
            quest_w.x10 = (u16)p->a;
            i++;
            break;
        case 0xE:
            quest_w.x10 += (u16)p->a;
            i++;
            break;
        case 0xF:
            i++;
            p++;
            quest_w.x04++;
            for (;;) {
                if (p->a == game_w.master || p->a == -4 || p->cmd == 0x12) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case 0x10:
            quest_w.x04++;
            i++;
            p++;
            for (;;) {
                if (p->cmd == 0x12) {
                    break;
                }
                if (p->cmd == 0x11) {
                    s32 t;
                    t = p->a;
                    if (t == -3) {
                        if (pl->kind == 1 || pl->kind == 5) {
                            break;
                        }
                    } else if (t == -2) {
                        if (pl->kind == 0 || (u8)(pl->kind - 2) <= 1 || pl->kind == 4) {
                            break;
                        }
                    } else if (t == -4 || pl->kind == t) {
                        break;
                    }
                }
                i++;
                p++;
            }
            i++;
            goto again;
        case 0x11:
        case 0x12:
            quest_w.x04--;
            for (;;) {
                if (p->cmd == 0x12) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case -1:
            if (Quest_f_dra_ck(*(u8 *)&quest_w.no) != 0 && quest_w.xB0 != 0 && ((EMW *)quest_w.xB0)->kind == 2) {
                quest_w.x14C = ((EMW *)quest_w.xB0)->x302;
            }
#if defined(VERSION_MHG) && !defined(__MWERKS__)
            rt_cap_request();               /* G 0x4B8AA0: system_w+0x33 = 0x81, a copy of the next frame for the reward list
                                             * (G starts the camera once it is taken, the PC at once: rt_2d.c) */
#endif
            QuestClearCameraRequest();
            quest_w.x182 = 0;
            i++;
            quest_w.x06 = 3;
            game_w.x0D5 = 3;
            quest_w.x184 = 0;
            quest_w.x186 = 0;
            quest_w.x181 = 2;
            quest_w.x140 |= 1 << (game_w.master + 8);
            net_send_sys(6, game_w.master);
            goto again;
        case 0x2D:
            if ((quest_w.x140 & 0xF00) != 0xF00 && game_w.pl_state[game_w.master] == 1) {
                break;
            }
            i++;
            break;
        case 0x1E:
            quest_w.x01 = 0;
            i = -1;
            quest_w.x00++;
            break;
        case 0x17:
            i = quest_w.x38;
            quest_w.x36 = i;
            goto again;
        case 0x18:
            if ((s16)act_ck(pl, 4, 0) == 0) {
                break;
            }
            i++;
            goto again;
        case -2:
            quest_w.x182 = 0;
            i++;
            quest_w.x184 = 0;
            game_w.x0D5 = 5;
            quest_w.x06 = 5;
            quest_w.x186 = 0;
            quest_w.x181 = 4;
            net_send_sys(6, game_w.master);
            goto again;
        case 0x1F:
            quest_w.x06 = 6;
            game_w.x0D5 = 6;
            i = -1;
            set01_set2(lit_2394);
            set01_set(0, 0xF, 0);
            break;
        case 0x1B:
            quest_w.x0A = p->a;
            n = Quest_time_get(1);
            str_gattai(buf, lit_2395, n / 1800);
            n = strlen(buf);
            if (n & 1) {
                buf[n] = 0x20;
                buf[n + 1] = 0;
            }
            buf[0x1F] = 0;
            set01_set2_use_mem(buf);
            i++;
            break;
        case 0x20:
            quest_w.x3A = p->a;
            quest_w.x182 = 0;
            quest_w.x181 = 6;
            quest_w.x186 = 0;
            quest_w.x184 = quest_w.x3A;
            net_send_sys(6, game_w.master);
            i++;
            goto again;
        case 0x21:
            if (quest_item_ck3(&quest_w, p) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x22:
            if (quest_share_item_ck(&quest_w) == 0) {
                break;
            }
            i++;
            goto again;
        case 0x23:
            quest_w.x0B = p->a;
            i++;
            break;
        case 0x2C:
            quest_w.x14F = p->a;
            i++;
            break;
#ifdef VERSION_MHG
        case 0x2F:      /* G (sub_main 0x4B9040): the section the time-out jumps to (like 0x1B, no message) */
            quest_w.x0A = p->a;
            i++;
            break;
#endif
        case 0x25:
            game_w.x0D5 = 5;
            quest_w.x06 = 5;
            set01_set2(lit_2396);
            i++;
            quest_w.x182 = 0;
            quest_w.x181 = 0xA;
            quest_w.x184 = 0;
            quest_w.x186 = 0;
            net_send_sys(6, game_w.master);
            goto again;
        case 0x26:
            quest_w.x182 = 0;
            i++;
            quest_w.x184 = 0;
            quest_w.x06 = 3;
            game_w.x0D5 = 3;
            quest_w.x186 = 0;
            quest_w.x181 = 0xB;
            quest_w.x140 |= 1 << (game_w.master + 8);
            net_send_sys(6, game_w.master);
            goto again;
        case 0x28: {
            s32 t;
            t = ((EMW *)quest_w.xB0)->x302;
            if (quest_w.x14C - p->a < t) {
                i++;
                p++;
                for (;;) {
                    if (p->cmd == 0x29) {
                        i++;
                        goto again;
                    }
                    i++;
                    p++;
                }
            }
            quest_w.x14C = t;
            i++;
            goto again;
        }
        case 0x2A:
            if ((u8)func_53B5C0() == 1) {
                i++;
                goto again;
            }
            i++;
            p++;
            for (;;) {
                if (p->cmd == 0x2B) {
                    i++;
                    goto again;
                }
                i++;
                p++;
            }
        case 0x1C: {
            s32 t;
            t = p->a;
            i = 0;
            for (;;) {
                if (q->cmd == 0x1A && t == q->a) {
                    i++;
                    goto again;
                }
                i++;
                q++;
            }
        }
        case 0x1A:
            i++;
            goto again;
        default:
            break;
        }
        return i;
    }
}

#ifdef VERSION_MHG
/* G sub_main 0x4B90A0: the reward list at game_w+0x100 (MH1 +0x128). Conditions by entry id: 241-261
 * kills of the target (20 - left >= id-241), 41-140 kills (99 - left >= id-41), 141-240 delivered
 * items (count >= id-141): only the first of these three kinds that passes counts; 1-20 quest flags
 * (quest_w.x13C bits; 11-15 the time used, game_w+0xFA), 21-25 delivered-item ranges
 * (rem_item_calc_tbl), 26-35 kill ranges (rem_kill_calc_tbl), 36-40 more flag bits, 0x8000 the
 * base reward, other ids always. A drawn item id 0 adds nothing (MH1 wrote an empty entry, which
 * left id 1 / count 0 in G's list after the sort). Item chances: rem_item_exit_sel_tbl, 7 lists in G
 * (base reward: main 0x12D170 (always 0 in the shipped game), skills 0x32 / 0x33 / 0x36 / 0x69). */
int func_0012D170(void);
extern u16 rem_kill_calc_tbl[];
void remuneration_item_set(void)
{
    u8 *gw = (u8 *)&game_w;
    PLW *pl = &player_work[game_w.master];
    QREM *e;
    REMI *tbl;
    REMI *q;
    u16 *q16;
    s8 *sel;
    u16 *tb;
    int n = 0;
    int got = 0;
    int i, j, th, r, acc, ok;
    u16 cnt;

    *(s32 *)(gw + 0x180) = 0;
    *(s32 *)(gw + 0x184) = 0;
    for (i = 0; i < 0x20; i++) {
        *(u16 *)(gw + 0x100 + 4 * i) = 0;
        *(s16 *)(gw + 0x102 + 4 * i) = 0;
    }
    for (e = (QREM *)quest_w.x88; e->id != 0xFFFF && n < 0x20; e++) {
        u16 id = e->id;

        ok = 1;
        if (id >= 241 && id < 262) {
            ok = !got && (u16)(20 - quest_enemy_ck2(&quest_w, (u8)quest_w.x2C[0])) >= id - 241;
            got |= ok;
        } else if (id >= 41 && id < 141) {
            ok = !got && (u16)(99 - quest_enemy_ck2(&quest_w, (u8)quest_w.x2C[0])) >= id - 41;
            got |= ok;
        } else if (id >= 141 && id < 241) {
            ok = !got && (u16)Share_item_num_ck((u16)quest_w.x1C[0], share_item_ck_ck()) >= id - 141;
            got |= ok;
        } else if (id >= 1 && id <= 10) {
            ok = id == 1 ? quest_w.x13C != 0 : (quest_w.x13C & (1 << (id - 2))) != 0;
        } else if (id >= 16 && id <= 20) {
            ok = (quest_w.x13C & (0x4000 << (id - 16))) != 0;
        } else if (id >= 36 && id <= 40) {
            ok = (quest_w.x13C & (0x200 << (id - 36))) != 0;
        } else if (id >= 11 && id <= 15) {
            static const s16 lim[5] = { 0x6400, 0x4B00, 0x3200, 0x1900, 0x1900 };
            ok = *(s16 *)(gw + 0xFA) > lim[id - 11] && quest_w.x14C <= lim[id - 11];
        } else if (id >= 21 && id <= 25) {
            tb = &rem_item_calc_tbl[id - 21];
            cnt = Share_item_num_ck((u16)quest_w.x1C[0], share_item_ck_ck());
            ok = cnt >= tb[0] && cnt < tb[1];
        } else if (id >= 26 && id <= 35) {
            tb = &rem_kill_calc_tbl[id - 26];
            cnt = (u16)(20 - quest_enemy_ck2(&quest_w, (u8)quest_w.x2C[0]));
            ok = cnt >= tb[0] && cnt < tb[1];
        }
        if (!ok) {
            continue;
        }
        tbl = (REMI *)(e->tbl + (u32)mission_area);
        for (i = 0; n < 0x20; i++) {
            if (id == 0x8000) {
                if (func_0012D170() == 1) {
                    sel = rem_item_exit_sel_tbl[5];
                } else if (Pl_Skill_ck(pl, 0x32) == 1) {
                    sel = rem_item_exit_sel_tbl[2];
                } else if (Pl_Skill_ck(pl, 0x33) == 1) {
                    sel = rem_item_exit_sel_tbl[3];
                } else if (Pl_Skill_ck(pl, 0x36) == 1) {
                    sel = rem_item_exit_sel_tbl[4];
                } else if (Pl_Skill_ck(pl, 0x69) == 1) {
                    sel = rem_item_exit_sel_tbl[6];
                } else {
                    sel = rem_item_exit_sel_tbl[0];
                }
            } else {
                sel = rem_item_exit_sel_tbl[1];
            }
            if (!(((u16)ran_suu(0) & 0x1F) < sel[i])) {
                break;
            }
            th = 0;
            for (q = tbl; q->w != 0xFFFF; q = (REMI *)((u8 *)q + 6)) {
                th = (th + q->w) & 0xFFFF;
            }
            if (th == 0) {      /* (an empty table: G divides by zero here) */
                break;
            }
            r = (u16)((u16)ran_suu(0) % (u16)th);
            if (id == 0x8000 && i == 0) {
                r = 0;
            }
            acc = 0;
            q16 = (u16 *)tbl;
            while (*q16 != 0xFFFF) {
                acc = (acc + *q16) & 0xFFFF;
                q16 += 1;
                if (r < acc) {
                    break;
                }
                q16 += 2;
            }
            if (q16[0] != 0) {
                *(u16 *)(gw + 0x100 + 4 * n) = q16[0];
                *(u16 *)(gw + 0x102 + 4 * n) = q16[1];
                n++;
            }
        }
    }
    for (i = 0; i < 0x20; i++) {
        for (j = i + 1; j < 0x20; j++) {
            u16 bi = *(u16 *)(gw + 0x100 + 4 * j);
            u16 ai = *(u16 *)(gw + 0x100 + 4 * i);
            if (bi < ai && bi != 0) {
                s16 an = *(s16 *)(gw + 0x102 + 4 * i);
                *(u16 *)(gw + 0x100 + 4 * i) = bi;
                *(s16 *)(gw + 0x102 + 4 * i) = *(s16 *)(gw + 0x102 + 4 * j);
                *(u16 *)(gw + 0x100 + 4 * j) = ai;
                *(s16 *)(gw + 0x102 + 4 * j) = an;
            }
        }
    }
}
#else
void remuneration_item_set(void)
{
    PLW *pl;
    int n;
    QREM *e;
    REMI *tbl;
    u8 *out;
    int i;
    int th;
    u8 *a;
    u8 *b;
    int j;
    u8 *g;
    REMI *q;
    s8 *sel;
    int r;
    u16 *q16;
    int acc;
    u16 *tb;
    int ck;
    u16 cnt;

    game_w.x1A8 = 0;
    game_w.x1AC = 0;
    pl = &player_work[game_w.master];
    g = (u8 *)&game_w + GWX(0x128) - 0x128;     /* G clears its list at 0x100 (sub_main 0x4B9108): the supply
                                                 * box's items (Start_item_init) must not stay in the reward */
    for (i = 0; i < 0x20; i += 8) {
        *(s16 *)(g + 0x128) = 0;
        *(s16 *)(g + 0x12A) = 0;
        *(s16 *)(g + 0x12C) = 0;
        *(s16 *)(g + 0x12E) = 0;
        *(s16 *)(g + 0x130) = 0;
        *(s16 *)(g + 0x132) = 0;
        *(s16 *)(g + 0x134) = 0;
        *(s16 *)(g + 0x136) = 0;
        *(s16 *)(g + 0x138) = 0;
        *(s16 *)(g + 0x13A) = 0;
        *(s16 *)(g + 0x13C) = 0;
        *(s16 *)(g + 0x13E) = 0;
        *(s16 *)(g + 0x140) = 0;
        *(s16 *)(g + 0x142) = 0;
        *(s16 *)(g + 0x144) = 0;
        *(s16 *)(g + 0x146) = 0;
        g += 0x20;
    }
    e = (QREM *)quest_w.x88;
    n = 0;
    out = (u8 *)&game_w;
    for (; e->id != 0xFFFF; e++) {
        switch (e->id) {
        case 0x8000:
            break;
        case 1:
            if (!(quest_w.x13C != 0)) {
                continue;
            }
            break;
        case 2:
            if (!(quest_w.x13C & 1)) {
                continue;
            }
            break;
        case 3:
            if (!(quest_w.x13C & 2)) {
                continue;
            }
            break;
        case 4:
            if (!(quest_w.x13C & 4)) {
                continue;
            }
            break;
        case 5:
            if (!(quest_w.x13C & 8)) {
                continue;
            }
            break;
        case 6:
            if (!(quest_w.x13C & 0x10)) {
                continue;
            }
            break;
        case 7:
            if (!(quest_w.x13C & 0x20)) {
                continue;
            }
            break;
        case 8:
            if (!(quest_w.x13C & 0x40)) {
                continue;
            }
            break;
        case 9:
            if (!(quest_w.x13C & 0x80)) {
                continue;
            }
            break;
        case 0xA:
            if (!(quest_w.x13C & 0x100)) {
                continue;
            }
            break;
        case 0x10:
            if (!(quest_w.x13C & 0x4000)) {
                continue;
            }
            break;
        case 0x11:
            if (!(quest_w.x13C & 0x8000)) {
                continue;
            }
            break;
        case 0x12:
            if (!(quest_w.x13C & 0x10000)) {
                continue;
            }
            break;
        case 0x13:
            if (!(quest_w.x13C & 0x20000)) {
                continue;
            }
            break;
        case 0x14:
            if (!(quest_w.x13C & 0x40000)) {
                continue;
            }
            break;
        case 0xB:
            if (game_w.x218 > 0x6400) {
                if (quest_w.x14C > 0x6400) {
                    continue;
                }
                break;
            }
            continue;
        case 0xC:
            if (game_w.x218 > 0x4B00) {
                if (quest_w.x14C > 0x4B00) {
                    continue;
                }
                break;
            }
            continue;
        case 0xD:
            if (game_w.x218 > 0x3200) {
                if (quest_w.x14C > 0x3200) {
                    continue;
                }
                break;
            }
            continue;
        case 0xE:
            if (game_w.x218 > 0x1900) {
                if (quest_w.x14C > 0x1900) {
                    continue;
                }
                break;
            }
            continue;
        case 0xF:
            if (game_w.x218 > 0x1900) {
                if (quest_w.x14C > 0x1900) {
                    continue;
                }
                break;
            }
            continue;
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
            tb = &rem_item_calc_tbl[e->id - 0x15];
            ck = share_item_ck_ck();
            cnt = Share_item_num_ck((u16)quest_w.x1C[0], ck);
            if (cnt < tb[0]) {
                continue;
            }
            if (!(cnt < tb[1])) {
                continue;
            }
            break;
        default:
            break;
        }
        tbl = (REMI *)(e->tbl + (u32)mission_area);
        for (i = 0;; i++) {
            if (e->id == 0x8000) {
                if (Pl_Skill_ck(pl, 0x34) == 1) {
                    sel = rem_item_exit_sel_tbl[2];
                } else if (Pl_Skill_ck(pl, 0x35) == 1) {
                    sel = rem_item_exit_sel_tbl[3];
                } else {
                    sel = rem_item_exit_sel_tbl[0];
                }
            } else {
                sel = rem_item_exit_sel_tbl[1];
            }
            th = sel[i];
            if (!(((u16)ran_suu(0) & 0x1F) < th)) {
                break;
            }
            th = 0;
            if (tbl->w != 0xFFFF) {
                q = tbl;
                do {
                    th = (th + q->w) & 0xFFFF;
                    q = (REMI *)((u8 *)q + 6);
                } while (q->w != 0xFFFF);
            }
            r = (u16)((u16)ran_suu(0) % (u16)th);
            if (e->id == 0x8000 && i == 0) {
                r = 0;
            }
            acc = 0;
            q16 = (u16 *)tbl;
            if (*q16 != 0xFFFF) {
                do {
                    acc = (acc + *q16) & 0xFFFF;
                    q16 += 1;
                    if (r < acc) {
                        break;
                    }
                    q16 += 2;
                } while (*q16 != 0xFFFF);
            }
            n++;
            *(u16 *)(out + 0x128) = q16[0];
            *(u16 *)(out + 0x12A) = q16[1];
            if (n >= 0x20) {
                break;
            }
            out += 4;
        }
    }
    a = (u8 *)&game_w;
    for (i = 0; i < 0x20; i++) {
        b = (u8 *)&game_w + (i + 1) * 4;
        for (j = i + 1; j < 0x20; j++) {
            u16 bi = *(u16 *)(b + 0x128);
            u16 ai = *(u16 *)(a + 0x128);
            if (bi < ai && bi != 0) {
                s16 an = *(s16 *)(a + 0x12A);
                *(u16 *)(a + 0x128) = bi;
                *(s16 *)(a + 0x12A) = *(s16 *)(b + 0x12A);
                *(u16 *)(b + 0x128) = ai;
                *(s16 *)(b + 0x12A) = an;
            }
            b += 4;
        }
        a += 4;
    }
}
#endif





int quest_item_ck2(q, em)
QUEST_W *q;
QEM *em;
{
    PLW *pl;
    int i;
    s16 n;
    s16 id;
    u8 *p;
    s16 *a;
    s16 *b;

    pl = &player_work[game_w.master];
    if (pl->stg == em->x02) {
        for (i = 0, p = (u8 *)q; i < 4; i++, p += 2) {
            id = *(s16 *)(p + 0x1C);
            if (id != 0 && (n = Pl_item_num_ck(pl, id & 0xFFFF)) != 0) {
                a = &q->x24[i];
                if (*a < n) {
                    n = *a;
                }
                *a -= n;
                b = &q->x1C[i];
                Pl_item_stack(pl, (u16)*b, -n);
                id = *(s16 *)&Item_data[*b][8];
                if (id != 0) {
                    Share_item_stack(pl, id & 0xFFFF, n);
                }
                set01_set(1, 8, *b);
                quest_w.x182 = 0;
                quest_w.x181 = 1;
                quest_w.x184 = *b;
                quest_w.x186 = n;
                net_send_sys(6, game_w.master);
                if (*a <= 0) {
                    *b = 0;
                }
                break;
            }
        }
    }
    for (i = 0, p = (u8 *)q; i < 4; i++, p += 2) {
        if (*(s16 *)(p + 0x1C) != 0) {
            return 0;
        }
    }
    return 1;
}











void Quest_net_sub(void)
{
    int i;
    s16 id;
    s32 t;

    switch ((u8)quest_w.x181) {
    case 1:
        id = quest_w.x184;
        for (i = 0; i < 4; i++) {
            if (quest_w.x1C[i] == id) {
                quest_w.x24[i] = quest_w.x24[i] - quest_w.x186;
                break;
            }
        }
        break;
    case 2:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 8);
        switch (game_w.x0D5) {
        case 0:
        case 1:
        case 2:
            quest_presuccess_ptr_set(0);
            break;
        }
        break;
    case 4:
        switch (game_w.x0D5) {
        case 5:
        case 6:
            break;
        default:
            quest_failed_ptr_set(0);
            break;
        }
        break;
    case 5:
        t = ((s32)quest_w.x184 << 16 & 0xFFFF0000) | (u16)quest_w.x186;
        if (t < quest_w.x10) {
            quest_w.x10 = t;
        }
        break;
    case 6:
        id = quest_w.x184;
        if (quest_w.x3A < id) {
            quest_w.x3A = id;
        }
        break;
    case 7:
        quest_em_die();
        break;
    case 9:
        Net_Share_item_stack(&player_work[(u8)quest_w.x180], (u16)quest_w.x184, quest_w.x186);
        break;
    case 10:
        switch (game_w.x0D5) {
        case 5:
        case 6:
            break;
        default:
            quest_failed_ptr_set(1);
            break;
        }
        break;
    case 11:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 8);
        switch (game_w.x0D5) {
        case 0:
        case 1:
        case 2:
            quest_presuccess_ptr_set(1);
            break;
        }
        break;
    case 12:
        quest_w.x140 |= 1 << (u8)quest_w.x180;
        quest_w.xB4[(u8)quest_w.x180].a = quest_w.x184;
        break;
    case 13:
        quest_w.x140 |= 1 << ((u8)quest_w.x180 + 4);
        quest_w.x13C |= quest_w.x184 << 16;
        quest_w.x13C |= quest_w.x186;
        break;
    }
}




