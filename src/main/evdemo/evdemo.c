/* Event demos (cut scenes triggered by quest events), SLPM_654.95 main
 * 0x2862F0-0x286990: EvDemoInitialize picks up to three demo slots by quest
 * number, EvDemoMove runs them (a slot is started when its check function
 * returns > 0, then its event function runs until it returns non-zero),
 * check000/event000 are the generic check/event functions used by the demo
 * tables evdemo_NN (data, not in C), evdemo_camera_request starts the demo
 * camera on the monster with the given id. Field meanings are guesses. */
#include "g_alias.h"
#include "types.h"
#ifndef __MWERKS__
signed char DemoCameraCheck();   /* (declared before its first use) */
#endif

typedef struct EVD {
    u8 x0;          /* 0x0 */
    u8 flag;        /* 0x1 event flag number */
    u8 cam;         /* 0x2 demo camera number */
    u8 em;          /* 0x3 monster id the camera looks at (0 = none) */
    u8 kind;        /* 0x4 0..3: pause/bgm behaviour */
    u8 se;          /* 0x5 */
    u8 bgm0;        /* 0x6 */
    u8 bgm1;        /* 0x7 */
    int (*check)(); /* 0x8 */
    int (*event)(); /* 0xC */
} EVD;

typedef struct EVSLOT {
    u8 active;      /* 0x0 */
    u8 step;        /* 0x1 */
    s16 timer;      /* 0x2 */
    int (*check)(); /* 0x4 */
    int (*event)(); /* 0x8 */
    EVD *data;      /* 0xC */
} EVSLOT;

typedef struct EVENT_DEMO {
    u8 state;       /* 0x0 */
    u8 idx;         /* 0x1 */
    u8 _pad2[2];
    EVSLOT slot[3]; /* 0x4 */
} EVENT_DEMO;

typedef struct EMS {
    u8 x0;
    u8 x1;
    u8 x2;
    u8 _pad[0xA10 - 3];
} EMS;

/* game_w and player_work by offset (file-local views) */
typedef struct GW {
    u8 _p0[0x11];
    u8 x11;
    u8 _p12[2];
    u8 stage;
    u8 _p15[0x26 - 0x15];
    s16 x26;
    u8 _p28[4];
    u16 quest;
#ifdef VERSION_MHG
    u8 _p2E[0x3A - 0x2E];
    u8 master;                  /* G: 0x3A */
    u8 _p3B[0x1F5 - 0x3B];
    u8 info_stop;               /* G: 0x1F5 */
    u8 _p1F6[0x240 - 0x1F6];
#else
    u8 _p2E[0xD1 - 0x2E];
    u8 master;
    u8 _pD2[0x21F - 0xD2];
    u8 info_stop;
    u8 _p220[4];
#endif
} GW;
extern GW game_w;
extern EVENT_DEMO event_demo;
extern EMS em_work[];
extern u8 player_work[];
extern u8 evdemo_03[], evdemo_04[], evdemo_05[], evdemo_06[], evdemo_08[], evdemo_10[];
extern u8 evdemo_11[], evdemo_14[], evdemo_17[], evdemo_20[], evdemo_23[], evdemo_24[];

int Event_flag_ck();
void Event_flag_set();
void Event_flag_clear();
int evdemo_init_sub();
void evdemo_camera_request();
void DemoCameraRequest();
#ifdef __MWERKS__
int DemoCameraCheck();
#else
signed char DemoCameraCheck();   /* the real return type (tools/check_kr_returns.py) */
#endif
void DemoCameraCancel();
void str_pause();
void adx_se_set();
void demo_bgm_set();
void stage_bgm_set();
void func_63AFA0();

#if defined(VERSION_MHG) && !defined(__MWERKS__)
/* G's event demos for the PC (agent A, 15 Oct 2026; written from G game.bin 0x54A310 / 0x54A5D0 / 0x54AA80, not
 * byte-checked). G picks the demo by G quest number, and before that by the quest's monster kinds (game 0x54A5D0:
 * the Lao-Shan demo, kind 7 / 50, flags 24 / 35, stage 14; kinds 2 / 36 flags 27 / 36). evdemo_g_tbl is G's
 * run of 19 demo entries (0x678280, config/mhg/pc_data.txt); the MH1 names evdemo_03.. point into it. */
extern EVD evdemo_g_tbl[19];
s32 *Em_data_com_adrs_get(s32 *p, int which);
static int evdemo_g_kinds(u8 *k)       /* G sub_main 0x4B5DA0: the quest's monster kinds (mission list 0) */
{
    extern u8 quest_w[];
    s32 *mis = *(s32 **)(quest_w + 0x78);
    s32 *l0 = Em_data_com_adrs_get(mis, 0);
    int n = 0;

    if (l0 == 0 || Em_data_com_adrs_get(mis, 1) == 0)
        return 0;
    for (; *l0 != -1 && n < 32; l0++)
        k[n++] = (u8)*l0;
    return n;
}

static int evdemo_g_kind_demo(EVENT_DEMO *e)     /* G game 0x54A5D0 */
{
    u8 k[32];
    int n, i;

    Event_flag_clear(0x18);
    Event_flag_clear(0x23);
    Event_flag_clear(0x1B);
    Event_flag_clear(0x24);
    n = evdemo_g_kinds(k);
    for (i = 0; i < n; i++) {
        switch (k[i]) {
        case 7:  evdemo_init_sub(&e->slot[0], &evdemo_g_tbl[8]);  return 7;
        case 50: evdemo_init_sub(&e->slot[0], &evdemo_g_tbl[9]);  return 50;
        case 2:  evdemo_init_sub(&e->slot[0], &evdemo_g_tbl[10]); return 2;
        case 36: evdemo_init_sub(&e->slot[0], &evdemo_g_tbl[11]); return 36;
        }
    }
    return 0;
}

void EvDemoInitialize(void)
{
    EVENT_DEMO *e = &event_demo;
    int t = -1, t1 = -1;

    game_w.info_stop = 0;
    event_demo.state = 0;
    *(int *)&e->slot[1] = 0;
    *(int *)&e->slot[0] = 0;
    if ((u16)evdemo_g_kind_demo(e) != 0)
        return;
    switch (game_w.quest) {         /* entry numbers in evdemo_g_tbl (0 = G's evdemo_03 .. 18) */
    case 10006: t = 0; break;
    case 10014: t = 1; break;
    case 10010: t = 2; t1 = 13; break;
    case 10032: t = 3; break;
    case 10005: t = 4; break;
    case 10015: t = 5; break;
    case 10022: t = 6; break;
    case 10041: t = 7; break;
    case 10001: t = 12; break;
    case 10028: t = 14; break;
    case 10035: t = 15; break;
    case 10043: t = 16; break;
    case 10046: t = 17; break;
    case 10008: t = 18; break;
    }
    if (t >= 0)
        evdemo_init_sub(&e->slot[0], &evdemo_g_tbl[t]);
    if (t1 >= 0)
        evdemo_init_sub(&e->slot[1], &evdemo_g_tbl[t1]);
}
#elif !defined(G_PART)
void EvDemoInitialize(void)
{
    EVENT_DEMO *e = &event_demo;

    game_w.info_stop = 0;
    event_demo.state = 0;
    *(int *)&e->slot[1] = 0;
    *(int *)&e->slot[0] = 0;
    switch (game_w.quest) {
    case 0x88:
        evdemo_init_sub(&e->slot[0], evdemo_03);
        break;
    case 0x89:
        evdemo_init_sub(&e->slot[0], evdemo_04);
        break;
    case 0x8A:
        evdemo_init_sub(&e->slot[0], evdemo_05);
        evdemo_init_sub(&e->slot[1], evdemo_24);
        break;
    case 0x8B:
        evdemo_init_sub(&e->slot[0], evdemo_06);
        break;
    case 0x87:
        evdemo_init_sub(&e->slot[0], evdemo_08);
        break;
    case 0x94:
        evdemo_init_sub(&e->slot[0], evdemo_10);
        break;
    case 0x9A:
        evdemo_init_sub(&e->slot[0], evdemo_11);
        break;
    case 0xAB:
        evdemo_init_sub(&e->slot[0], evdemo_14);
        break;
    case 0x65:
    case 0x6B:
    case 0xCE:
        Event_flag_clear(0x18);
        evdemo_init_sub(&e->slot[0], evdemo_17);
        break;
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0xCF:
        Event_flag_clear(0x1B);
        evdemo_init_sub(&e->slot[0], evdemo_20);
        break;
    case 0x83:
        evdemo_init_sub(&e->slot[0], evdemo_23);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
int evdemo_init_sub(s, d)
EVSLOT *s;
EVD *d;
{
    if (Event_flag_ck(d->flag) == 0) {
        s->active = 1;
        s->check = d->check;
        s->event = d->event;
        s->data = d;
        return 0;
    }
    return -1;
}
#endif

#if !defined(G_PART) || G_PART == 3
void EvDemoMove(void)
{
    int i;
    EVSLOT *s;
    EVENT_DEMO *e = &event_demo;

    switch (e->state) {
    case 1:
        s = &e->slot[e->idx];
        if (s->event(s) == 0) {
            return;
        }
        game_w.info_stop = 0;
        e->state = 0;
    case 0:
        s = e->slot;
        i = 0;
        for (;;) {
            if (s->active != 0 && s->check(s) > 0) {
                e->state = 1;
                e->idx = i;
                s->event(s);
                return;
            }
            i++;
            s++;
            if (i >= 3) {
                break;
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 4
int check000(s)
EVSLOT *s;
{
    if (Event_flag_ck(s->data->flag) == 1) {
        *(int *)s = 0;
        return -1;
    }
    return game_w.stage == s->data->x0;
}
#endif

#if !defined(G_PART) || G_PART == 2
int event000(s)
EVSLOT *s;
{
    switch (s->step) {
    case 0:
        game_w.info_stop = 1;
        evdemo_camera_request(s->data->cam, s->data->em);
        switch (s->data->kind) {
        case 3:
            s->timer = 0x10E;
            str_pause(0, 1);
            break;
        case 2:
            s->timer = 0x15E;
        case 1:
            str_pause(0, 1);
            break;
        case 0:
            break;
        }
        if (s->data->se != 0) {
            adx_se_set(player_work + game_w.master * 0xA00, s->data->se);
        }
        s->step++;
        break;
    case 1:
        if (s->timer != 0) {
            s->timer--;
            if (s->timer == 0) {
                switch (s->data->kind) {
                case 2:
                    demo_bgm_set(GV(0x4E, 0x51));
                    break;
                case 3:
                    demo_bgm_set(GV(0x54, 0x57));
                    break;
                }
            }
        }
        if (DemoCameraCheck() <= 0) {
            s->step++;
            game_w.info_stop = 0;
            Event_flag_set(s->data->flag);
            if (s->data->bgm0 != 0) {
                func_63AFA0(s->data->bgm0);
                if (s->data->bgm1 != 0) {
                    func_63AFA0(s->data->bgm1);
                }
            }
            switch (s->data->kind) {
            case 1:
                stage_bgm_set(game_w.stage);
                break;
            case 0:
                break;
            case 2:
            case 3:
                game_w.x11 = 4;
                game_w.x26 = 0x96;
                break;
            }
        }
        break;
    case 2:
        DemoCameraCancel(2);
        *(int *)s = 0;
        return 1;
    }
    return 0;
}
#endif

#if !defined(G_PART)
void evdemo_camera_request(cam, id)
u8 cam;
u8 id;
{
    EMS *e;
    int n;

    if (id == 0) {
        DemoCameraRequest(cam, 0);
        return;
    }
    e = em_work;
    for (n = 20; n != 0; n--) {
#if defined(VERSION_MHG) && !defined(__MWERKS__)
        extern u8 *em_species_tbl[];    /* G 0x54AA80: id is a species (sub_main 0x4C8910) */
        if (e->x0 != 0 && *em_species_tbl[e->x2] == id) {
#else
        if (e->x0 != 0 && e->x2 == id) {
#endif
            DemoCameraRequest(cam, e);
            return;
        }
        e++;
    }
}
#endif
