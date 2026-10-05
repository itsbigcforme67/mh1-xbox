/* Event demos (cut scenes triggered by quest events), SLPM_654.95 main
 * 0x2862F0-0x286990: EvDemoInitialize picks up to three demo slots by quest
 * number, EvDemoMove runs them (a slot is started when its check function
 * returns > 0, then its event function runs until it returns non-zero),
 * check000/event000 are the generic check/event functions used by the demo
 * tables evdemo_NN (data, not in C), evdemo_camera_request starts the demo
 * camera on the monster with the given id. Field meanings are guesses. */
#include "types.h"

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
    u8 _p2E[0xD1 - 0x2E];
    u8 master;
    u8 _pD2[0x21F - 0xD2];
    u8 info_stop;
    u8 _p220[4];
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
int DemoCameraCheck();
void DemoCameraCancel();
void str_pause();
void adx_se_set();
void demo_bgm_set();
void stage_bgm_set();
void func_63AFA0();

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

int check000(s)
EVSLOT *s;
{
    if (Event_flag_ck(s->data->flag) == 1) {
        *(int *)s = 0;
        return -1;
    }
    return game_w.stage == s->data->x0;
}

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
                    demo_bgm_set(0x4E);
                    break;
                case 3:
                    demo_bgm_set(0x54);
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
        if (e->x0 != 0 && e->x2 == id) {
            DemoCameraRequest(cam, e);
            return;
        }
        e++;
    }
}
