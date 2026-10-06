/* cam - SLPM_654.95 0x0021F3D0-0x00222E20 (f_cam): game camera.
 * CameraMove runs each frame: reads the pad, finds the camera area the
 * player is in, runs the five camera slots (std = behind the player,
 * stage = fixed/rail cameras from the area data, pachinger cannon, player
 * EX = fishing and NPC zoom, demo = scripted point cameras) and cam2view
 * copies the winning slot into lpView with the screen quake. */
#include "cam.h"
#include "game.h"
#include "fl.h"

#ifndef NULL
#define NULL 0
#endif

typedef struct CAMBLK {
    u8 _pad00[5];
    u8 n;               /* 0x05 count of 0x40-byte records after 0x300 */
    u8 _pad06[0x18 - 0x06];
    u8 *a;              /* 0x18 offsets made into pointers */
    u8 *b;              /* 0x1C if set, 0x20 more bytes follow */
} CAMBLK;

typedef struct CAMGRID {
    s32 n;              /* 0x00 */
    u8 *p;              /* 0x04 */
} CAMGRID;

typedef struct CAMDATA {
    u8 _pad00[2];
    u16 num;            /* 0x02 blocks at 0x30 */
    u16 w;              /* 0x04 */
    u16 h;              /* 0x06 */
    u8 _pad08[0x1C - 0x08];
    u8 *grid;           /* 0x1C CAMGRID[w * h] */
    u8 *list;           /* 0x20 */
    u8 _pad24[0x28 - 0x24];
    CAMBLK *blk;        /* 0x28 */
    u8 _pad2C[0x30 - 0x2C];
} CAMDATA;

typedef void (*CAMFN)(CAMW *, CAMS *);

extern CAMFN cam_init_sub_jmp[];
extern CAMFN cam_sub_jmp[];
extern CAMCNF cam_cnf_chs;
extern CAMCNFE stage_camera_data_ex;
extern s16 quake_time_tbl[];
extern f32 *fishcam_ofs_tbl[];
extern s8 *Demo_cam_tbl[];
extern f32 pch_pos[][6];
extern u16 Psw[];
extern f32 *lpView;

void flMemset(void *, s32, s32);
void StageCamInit(CAMW *, u8);
void flvecCopy(f32 *, f32 *);
void cam_sw_set_sub(CAMW *);
s8 Get_cam_grid_XZ(s16 *, s16 *, f32 *, u8 *);
s32 SetAreaData(CAMW *);
s32 CameraAreaCheck(CAMAREA *, PLW *, s32);
void cam2view(CAMW *);
void View_move(void);
s32 Cockpit_menu_chk(void);
s32 Cockpit_chat_chk(void);
s32 fishing_cam_chk(PLW *);
s32 Pl_bari_ck(PLW *);
s32 pl_flag_ck(PLW *, s32);
void flvecRotY(f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void cpInterVector(f32 *, f32 *, f32 *, f32);   /* out = a * t + b * (1 - t) (0x120E30) */
s32 fish_cam_sub(CAMW *, CAMS *, CAMFISH *);
s32 point_cam_sub(CAMW *, CAMS *, CAMD_DEMO *);
s32 point_camera(CAMW *, CAMS *);
void RollView(f32);
void SetAngleOfView(f32);
void quake_sub(CAMQUAKE *);
void set_to_std_cam(s32);
s32 Pl_stg_ck(void);
s32 Em_stg_ck(void);
f32 flSin(f32);
f32 flvecCalcDistance(f32 *, f32 *);
s32 Game_clear_ck(s32);
void cam_plEX_fishing(CAMW *, CAMS *, CAMFISH *);
void cam_plEX_zoom(CAMW *, CAMS *, CAMZOOM *);

void CameraWorkInit(void) {
    flMemset(&CameraWork, 0, sizeof(CAMW));
}

void Q_camera_init(void) {
    CameraWork.demo_req = 0;
    CameraWork.demo_arg = 0;
    CameraWork.zoom = 2;
}

void CameraInit(void) {
    CameraWork.init.w = 0;
    CameraWork.qk[1].on = 0;
    CameraWork.qk[0].on = 0;
    CameraWork.reset = 0;
    CameraWork.pl = &player_work[game_w.master];
    StageCamInit(&CameraWork, game_w.master);
}

void SetCameraData(CAMDATA *d) {
    s32 i;
    CAMBLK *b;
    s32 n;
    s32 j;
    u8 *p;
    CAMGRID *g;
    u8 **l;

    CameraWork.data = d;
    if (d != NULL) {
        b = (CAMBLK *)((u8 *)d + 0x30);
        d->blk = b;
        for (i = d->num; i != 0; i--) {
            if (b->b != 0) {
                p = (u8 *)b + 0x300;
                b->a = (u8 *)b + (u32)b->a;
                b->b = (u8 *)b + (u32)b->b;
                b = (CAMBLK *)(p + (b->n << 6) + 0x20);
            } else {
                p = (u8 *)b + 0x300;
                b->a = (u8 *)b + (u32)b->a;
                b = (CAMBLK *)(p + (b->n << 6));
            }
        }
        g = (CAMGRID *)((u8 *)d + (u32)d->grid);
        d->grid = (u8 *)g;
        l = (u8 **)((u8 *)d + (u32)d->list);
        d->list = (u8 *)l;
        n = 0;
        for (j = d->w * d->h; j != 0; j--) {
            if (g->n != 0) {
                g->p = (u8 *)d + (u32)g->p;
                n += g->n;
            }
            g++;
        }
        while (n != 0) {
            n--;
            *l += (u32)d;
            l++;
        }
    }
}

void WyvernFindPlayer(PLW *pl) {
    CAMW *cw = &CameraWork;

    if (pl->id == game_w.master) {
        cw->reset = (s32)pl;
    }
}

void BBQcamera_set(PLW *pl) {
    CAMW *cw = &CameraWork;

    if (pl->id == game_w.master) {
        cw->sl[0].d.std.wall = 0;
        cw->sl[0].d.std.ang = pl->ang[1] + 0x2000;
        cw->zoom = 2;
    }
}

s32 manual_cam_chk(CAMW *cw, PLW *pl) {
    if (Cockpit_menu_chk() == 1) {
        return 0;
    }
    if (pl->x56E != 0) {
        return 0;
    }
    if (pl->work8C2 != 0) {
        return 0;
    }
    if (pl->x763 != 0) {
        return 0;
    }
    if (fishing_cam_chk(pl) != 0) {
        return 0;
    }
    if ((pl->kind == 1 || pl->kind == 5) && pl->flag12 != 0 && (pl->sw.now & 8)) {
        return 0;
    }
    return 1;
}

void std_cam_sw_set_sub(CAMW *cw, CAMD_STD *d) {
    PLW *pl;

    if (cw->cam_no == 0) {
        pl = cw->pl;
        if (manual_cam_chk(cw, pl) == 1) {
            d->on = cw->sw_on;
            d->trg = cw->sw_trg;
            if (Pl_bari_ck(pl) == 1 && (pl->sw.now & 8)) {
                d->on &= 0xF3FF;
                d->trg &= 0xF3FF;
            }
            return;
        }
        if (Cockpit_chat_chk() == 0) {
            d->on = cw->sw_on & 8;
            d->trg = cw->sw_trg & 8;
            return;
        }
    }
    d->trg = 0;
    d->on = 0;
}

void set_to_std_cam(s32 no) {
    CAMS *cs = &CameraWork.sl[0];

    if (no == 4) {
        no = -1;
    }
    if (no >= 0) {
        cs->req = no;
        return;
    }
    cs->req = -1;
}

void cam_init_sub_stg(CAMW *cw, CAMS *cs) {
    cs->roll = 0.0f;
    cs->fov = 0.9599311f;
}

void cam_init_sub_pchngr(CAMW *cw, CAMS *cs) {
    CAMD_PCH *d = &cs->d.pch;

    cs->fov = 0.7853982f;
    cs->roll = 0.0f;
    d->min = 0.17453294f;
    d->max = 1.0471976f;
    d->range = d->max - d->min;
    d->fov0 = d->fov1 = cs->fov;
}

s32 pch_lock_chk(PLW *pl) {
    switch (pl->char0) {
    case 0x3EA:
    case 0x579:
    case 0x57C:
    case 0x583:
    case 0x57D:
    case 0x580:
        return 1;
    }
    return 0;
}

s32 act_ck(PLW *, s32, s32);

s32 PachiTypeCheck(PLW *pl) {
    if ((s16)act_ck(pl, 0, 0x65) != 0 || (s16)act_ck(pl, 0, 0x66) != 0) {
        return 1;
    }
    if ((s16)act_ck(pl, 0, 0x36) != 0 || (s16)act_ck(pl, 0, 0x48) != 0) {
        return 2;
    }
    if (pl->kind == 1 || pl->kind == 5) {
        return 0;
    }
    return -1;
}

s8 GetPachingerInfo(PLW *pl, u8 *step, u8 *zoom, f32 *rate) {
    CAMS *cs = &CameraWork.sl[2];
    CAMD_PCH *d;

    *step = cs->d.pch.x63;
    *zoom = cs->d.pch.zoom;
    d = &cs->d.pch;
    if (cs->d.pch.zoom != 0) {
        *rate = cs->fov - d->min;
        *rate = *rate / d->range;
    }
    return d->type;
}

void cam_init_sub_playerEX(CAMW *cw, CAMS *cs) {
    cs->fov = 0.9599311f;
    cs->roll = 0.0f;
    *(s32 *)&cs->d.ex.fish.step = 0;
    *(s32 *)&cs->d.ex.zoom.step = 0;
    cs->d.ex.zoom.req = 0;
}

void cam_sub_playerEX(CAMW *cw, CAMS *cs) {
    CAMD_EX *d = &cs->d.ex;

    cam_plEX_fishing(cw, cs, &d->fish);
    if (cs->act == 0) {
        cam_plEX_zoom(cw, cs, &d->zoom);
    }
}

void cam_plEX_fishing(CAMW *cw, CAMS *cs, CAMFISH *f) {
    PLW *pl;

    cs->act = 0;
    pl = cw->pl;
    if ((f->on = fishing_cam_chk(pl)) == 0) {
        f->step = 0;
        return;
    }
    if (f->step == 0) {
        f->step++;
        f->fish = pl->fish878;
        if (game_w.stage != 0x4B) {
            cs->fov = 0.9599311f;
        } else {
            cs->fov = 0.87266463f;
        }
        cs->roll = 0.0f;
    }
    if (fish_cam_sub(cw, cs, f) >= 0) {
        cs->act = 1;
    }
}

s32 fishing_cam_chk(PLW *pl) {
    return pl_flag_ck(pl, 0x80000) != 0;
}

s32 fish_cam_sub(CAMW *cw, CAMS *cs, CAMFISH *f) {
    f32 v[3];
    PLW *pl;
    f32 *ofs;
    f32 a;

    pl = cw->pl;
    if (f->fish == NULL) {
        return -1;
    }
    ofs = fishcam_ofs_tbl[game_w.stage];
    if (ofs == NULL) {
        return -1;
    }
    a = 0.0000958738f * *(u16 *)((u8 *)f->fish + 0x14);
    flvecCopy(v, ofs);
    flvecRotY(v, a);
    AddVector(cs->eye, pl->pos, v);
    flvecCopy(v, ofs + 3);
    flvecRotY(v, a);
    AddVector(cs->tar, pl->pos, v);
    return 0;
}

void NPCZoomInCameraRequest(PLW *npc) {
    CAMZOOM *z = &CameraWork.sl[3].d.ex.zoom;

    z->req = 1;
    z->npc = npc;
    flvecCopy(z->pos, npc->pos);
}

void NPCZoomInCameraCancel(void) {
    CameraWork.sl[3].d.ex.zoom.req = 0;
}

s32 NPCZoomInCameraCheck(void) {
    return CameraWork.sl[3].d.ex.zoom.req != 0;
}

void cam_plEX_zoom(CAMW *cw, CAMS *cs, CAMZOOM *z) {
    PLW *pl;
    PLW *npc;
    CAMS *src;
    f32 t;

    cs->act = 0;
    switch (z->step) {
    case 0:
        if (z->req != 0) {
            z->cnt = 0;
            z->step++;
        }
        return;
    case 1:
        if (z->cnt < 15) {
            z->cnt++;
        }
        if (z->req == 0) {
            z->step++;
        }
        break;
    case 2:
        if (z->cnt > 0) {
            z->cnt--;
        }
        if (z->req != 0) {
            z->step = 1;
        } else if (z->cnt == 0) {
            z->step = 0;
            return;
        }
        break;
    }
    src = &cw->sl[cw->cam_no];
    npc = z->npc;
    pl = cw->pl;
    if (npc->kind == 3) {
        cpInterVector(z->tar, pl->pos, npc->pos, 0.5f);
        z->tar[1] += 64.0f;
    } else {
        cpInterVector(z->tar, pl->pos, z->pos, 0.2f);
        z->tar[1] += 150.0f;
    }
    t = z->cnt * (1.0f / 15.0f);
    cpInterVector(cs->tar, z->tar, src->tar, t);
    cs->fov = src->fov * (1.0f - t) + 0.5235988f * t;
    flvecCopy(cs->eye, src->eye);
    cs->roll = src->roll;
    cs->act = 1;
}

void cam_init_sub_demo(CAMW *cw, CAMS *cs) {
    cs->roll = 0.0f;
    cs->fov = 0.9599311f;
    cs->d.demo.no = 0;
    cs->d.demo.state = 0;
    cs->d.demo.arg = 0;
}

void cam_sub_demo(CAMW *cw, CAMS *cs) {
    CAMD_DEMO *d;
    s32 r;

    cs->act = 0;
    d = &cs->d.demo;
    switch (cs->mode.b) {
    case 0:
        if (cw->demo_req == 0) {
            return;
        }
        d->no = cw->demo_req;
        d->arg = cw->demo_arg;
        cs->mode.b++;
        cs->step.w = 0;
    case 1:
        cw->demo_req = 0;
        r = point_camera(cw, cs);
        if (r <= 0) {
            cs->act = 1;
            if (r == 0) {
                d->state = 1;
                return;
            }
            d->state = -1;
            return;
        }
        cs->mode.b = 0;
        d->no = 0;
        d->state = 0;
        d->arg = 0;
        break;
    }
}

void DemoCameraRequest(s8 no, s32 arg) {
    CameraWork.demo_req = no;
    CameraWork.demo_arg = arg;
}

void DemoCameraCancel(void) {
    CAMS *cs = &CameraWork.sl[4];

    cs->mode.b = 0;
    cs->d.demo.no = 0;
    cs->d.demo.state = 0;
    cs->d.demo.arg = 0;
    CameraWork.demo_req = 0;
}

s8 DemoCameraCheck(void) {
    return CameraWork.sl[4].d.demo.state;
}

s32 point_camera(CAMW *cw, CAMS *cs) {
    CAMD_DEMO *d;
    s32 r;

    d = &cs->d.demo;
    switch (cs->step.b) {
    case 0:
        cs->step.b++;
        d->cmd = Demo_cam_tbl[d->no];
        flvecCopy(cs->eye, cw->eye);
        flvecCopy(d->eye, cw->eye);
        flvecCopy(cs->tar, cw->tar);
        flvecCopy(d->tar, cw->tar);
        d->roll = cs->roll = cw->roll;
        d->fov = cs->fov = cw->fov;
        cs->cnt = cs->cnt_max = -1;
    case 1:
        if ((r = point_cam_sub(cw, cs, d)) <= 0) {
            break;
        }
        cs->step.b++;
    case 2:
        return 1;
    }
    /* r is not set on other steps (as in the original) */
    return r;
}

f32 *get_em_local(CAMD_DEMO *d) {
    u8 *em = *(u8 **)((u8 *)d + 0x50);

    if (em != NULL && *em != 0) {
        return (f32 *)(em + 0x60);
    }
    d->stop = 1;
    return NULL;
}

void cmd_copy(CAMD_DEMO *d, s32 n) {
    switch (n) {
    case 0:
        flvecCopy(d->eye_o, d->eye);
        break;
    case 1:
        flvecCopy(d->tar_o, d->tar);
        break;
    case 2:
        d->roll_o = d->roll;
        break;
    case 3:
        d->fov_o = d->fov;
        break;
    }
}

void get_angle(s16 *a, CAMS *cs) {
    if (cs->cnt_max > 0) {
        a[0] = cs->ax - cs->ax0;
        a[0] = cs->ax0 + a[0] * cs->cnt / cs->cnt_max;
        a[1] = cs->ay - cs->ay0;
        a[1] = cs->ay0 + a[1] * cs->cnt / cs->cnt_max;
    } else {
        a[0] = cs->ax;
        a[1] = cs->ay;
    }
}

s32 point_cam_hit(CAMW *cw, CAMS *cs, CAMD_DEMO *d) {
    return 0;
}

void cam2view(CAMW *cw) {
    CAMQUAKE *q;
    s32 i;
    s32 no;
    CAMS *cs;

    q = cw->qk;
    for (i = 2; i > 0; i--) {
        if (q->on != 0) {
            if (--q->time <= 0) {
                q->on = 0;
            }
        }
        q++;
    }
    cs = &cw->sl[cw->cam_no];
    flvecCopy(cw->eye, cs->eye);
    flvecCopy(cw->tar, cs->tar);
    cw->roll = cs->roll;
    cw->fov = cs->fov;
    no = -1;
    if (cw->sl[4].act != 0) {
        no = 4;
    } else if (cw->sl[2].act != 0) {
        no = 2;
    } else if (cw->sl[3].act != 0) {
        no = 3;
    }
    if (no > 0) {
        cs = &cw->sl[no];
        flvecCopy(lpView, cs->eye);
        flvecCopy(lpView + 3, cs->tar);
        if (no == 2) {
            quake_sub(&cw->qk[1]);
        }
        RollView(cs->roll);
        SetAngleOfView(cs->fov);
    } else {
        flvecCopy(lpView, cw->eye);
        flvecCopy(lpView + 3, cw->tar);
        quake_sub(&cw->qk[0]);
        RollView(cw->roll);
        SetAngleOfView(cw->fov);
    }
    set_to_std_cam(no);
}

s32 PachingerCamChk(PLW *pl) {
    CAMW *cw = &CameraWork;
    s32 r;

    if (cw->sl[4].act != 0) {
        return 0;
    }
    if (pl->x763 != 0) {
        r = 1;
    } else {
        r = cw->sl[2].act != 0;
    }
    return r;
}

void set_quake_sub(s32 type, f32 *pos) {
    CAMQUAKE *q = &CameraWork.qk[0];

    q->on = 1;
    q->type = type;
    q->time = quake_time_tbl[type];
    q->pos[0] = pos[0];
    q->pos[1] = pos[1];
    q->pos[2] = pos[2];
}

void set_quake_sub2(s32 type) {
    CAMQUAKE *q = &CameraWork.qk[0];

    q->on = 1;
    q->type = type | 0x80;
    q->time = quake_time_tbl[type];
}

void Pl_set_quake_sub(PLW *pl, s32 type) {
    CAMQUAKE *q = &CameraWork.qk[0];

    if (Pl_stg_ck() & 0xFF) {
        q->on = 1;
        q->type = type;
        q->time = quake_time_tbl[type];
        q->pos[0] = pl->pos[0];
        q->pos[1] = pl->pos[1];
        q->pos[2] = pl->pos[2];
    }
}

void Em_set_quake_sub(PLW *em, s32 type) {
    CAMQUAKE *q = &CameraWork.qk[0];

    if (Em_stg_ck() & 0xFF) {
        q->on = 1;
        q->type = type;
        q->time = quake_time_tbl[type];
        q->pos[0] = em->pos[0];
        q->pos[1] = em->pos[1];
        q->pos[2] = em->pos[2];
    }
}

void Pachinger_set_quake_sub(PLW *pl, s32 type) {
    CAMQUAKE *q = &CameraWork.qk[1];

    if (Pl_stg_ck() & 0xFF) {
        q->on = 1;
        q->type = type;
        q->time = quake_time_tbl[type];
        q->pos[0] = pl->pos[0];
        q->pos[1] = pl->pos[1];
        q->pos[2] = pl->pos[2];
    }
}

void quake_sub(CAMQUAKE *q) {
    s32 t;
    f32 a;
    f32 d;
    f32 k;

    if (q->on != 0) {
        t = q->type & 0x7F;
        a = 90.0f * ((f32)q->time / (f32)quake_time_tbl[t]);
        if (t < 2) {
            d = 10.0f * flSin(2.0f * (3.1415927f * (a / 360.0f)));
        } else {
            d = 12.0f * flSin(2.0f * (3.1415927f * (a / 360.0f)));
        }
        if (q->type & 0x80) {
            k = 1.0f;
        } else {
            a = flvecCalcDistance(q->pos, lpView);
            if (!(a <= 2000.0f)) {
                k = 0.0f;
            } else {
                k = 1.0f - a / 2000.0f;
            }
        }
        d *= k;
        if ((q->time >> 1) & 1) {
            d *= -1.0f;
        }
        lpView[1] += d;
        lpView[4] += d;
    }
}

void cam_sw_set_sub(CAMW *cw) {
    if (Game_clear_ck(1) == 1 || cw->pl->x8C6 != 0) {
        cw->sw_x5E6 = 0;
        cw->sw_trg = 0;
        cw->sw_on = 0;
        cw->an_pow = 0;
        cw->an_ang = 0;
    } else {
        cw->sw_on = Psw[0];
        cw->sw_trg = Psw[2];
        cw->sw_x5E6 = Psw[4];
        cw->an_ang = Psw[9];
        cw->an_pow = Psw[11];
    }
}

/* point_cam_sub is 28 instructions off (only a2/a3 for the command pointer
 * `w`); cmd_set_pos, cmd_set_tar, cmd_cam_move match in camd.c. */
s32 point_cam_sub(CAMW *cw, CAMS *cs, CAMD_DEMO *d) {
    s8 *cmd;
    s32 *w;
    s8 *c;
    s32 go;
    s32 r;

    go = 1;
    d->stop = 0;
    cmd = d->cmd;
    do {
        c = cmd;
        w = (s32 *)cmd;
        cmd += cmd[1] * 4;
        switch (c[0]) {
        case 0:
            d->move = c[2];
            break;
        case 1:
            d->pos_mode = c[2];
            d->pos_part = c[3];
            break;
        case 2:
            cmd_set_pos(d->eye, d, w);
            break;
        case 3:
            cmd_set_pos(d->eye_o, d, w);
            break;
        case 4:
            d->tar_mode = c[2];
            d->tar_part = c[3];
            break;
        case 5:
            cmd_set_tar(d->tar, d, w);
            break;
        case 6:
            cmd_set_tar(d->tar_o, d, w);
            break;
        case 7:
            d->x4D = c[2];
            break;
        case 8:
            cs->ax = *(s16 *)(c + 2);
            break;
        case 9:
            cs->ax0 = *(s16 *)(c + 2);
            break;
        case 10:
            cs->ay = *(s16 *)(c + 2);
            break;
        case 11:
            cs->ay0 = *(s16 *)(c + 2);
            break;
        case 12:
            d->x54 = 0.0625f * w[1];
            break;
        case 13:
            d->x58 = 0.0625f * w[1];
            break;
        case 14:
            d->roll = 0.000095873799f * w[1];
            break;
        case 15:
            d->roll_o = 0.000095873799f * w[1];
            break;
        case 16:
            d->fov = 0.000095873799f * w[1];
            break;
        case 17:
            d->fov_o = 0.000095873799f * w[1];
            break;
        case 18:
            cs->cnt = cs->cnt_max = w[1];
            break;
        case 19:
            cmd_copy(d, c[2]);
            break;
        case 20:
            d->loop = cmd;
            break;
        case 21:
            cs->cnt--;
            if (cs->cnt_max <= 0 || cs->cnt >= 0) {
                cmd = d->loop;
                go = 0;
                r = 0;
            } else {
                flvecCopy(d->eye, cs->eye);
                flvecCopy(d->tar, cs->tar);
                cs->ax = cs->ax0;
                cs->ay = cs->ay0;
                d->roll = cs->roll;
                d->fov = cs->fov;
                cs->cnt_max = cs->cnt = -1;
            }
            break;
        case 22:
            cmd_cam_move(cw, cs, d);
            break;
        case 25:
            if (point_cam_hit(cw, cs, d) == 0) {
                break;
            }
        default:
        case 23:
            return 1;
        case 24:
            go = 0;
            r = -1;
            break;
        }
        if (d->stop != 0) {
            return 1;
        }
        d->cmd = cmd;
    } while (go != 0);
    return r;
}

/* cam_sub_std: complete, ~65 instructions off (angle smoothing registers,
 * two stray alignment nops: the `k` switch and the blend-rate if). */
void cpRotMatrix(s32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
int act_ck(PLW *, int, int);
f32 GetGroundHit(f32 *);
f32 flArcTan2(f32, f32);
f32 flArcCos(f32);
f32 flSqrt(f32);
extern CAMCNFE stage_camera_data_ex;
void std_cam_sw_set_sub(CAMW *, CAMD_STD *);
void k_HitEmCamera(f32 *, f32 *, s32);
f32 k_HitWallCamera(f32 *, f32 *, f32 *);
u8 GetWallHitLine(f32 *, f32 *, f32 *, s32);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flvecCalcLength(f32 *);

void cam_sub_std(CAMW *cw, CAMS *cs) {
    CAMD_STD *d;
    PLW *pl;
    CAMAREA *area;
    CAMS *o2;
    f32 in[3];
    f32 o[3];
    s32 a[3];
    f32 hit[3];
    f32 v[3];
    FLMAT m;
    f32 gnd;
    f32 lim;
    f32 g;
    f32 g2;
    f32 len;
    f32 t;
    s32 diff;
    s32 ca;
    s32 da;
    s32 k;
    s32 f;
    u8 r;
    u8 *p;
    s32 n;

    if (cw->cam_no != 0) {
        cs->act = 0;
        cs->step.b = 0;
        return;
    }
    cs->act = 1;
    d = &cs->d.std;
    pl = cw->pl;
    std_cam_sw_set_sub(cw, d);
    flvecCopy(d->old, d->eye);
    if (cw->reset != 0) {
        d->wall = 0;
        d->ang = pl->x3A8 + 0x7FFF + 1;
    } else {
        r = cs->req;
        if (r != 0xFF) {
            cs->req = 0xFF;
            o2 = (CAMS *)((u8 *)cw + (r << 8) + 0x80);
            cs->ang = d->ang = 10430.378f * flArcTan2(o2->eye[0] - o2->tar[0], o2->eye[2] - o2->tar[2]);
        } else if (cw->cam_old != 0) {
            cs->ang = d->ang = 10430.378f * flArcTan2(cw->eye[0] - cw->tar[0], cw->eye[2] - cw->tar[2]);
        } else if ((d->trg & 8) || pl->x8C8 != 0 || d->wall != 0) {
            d->wall = 0;
            d->ang = pl->ang[1] + 0x7FFF + 1;
        } else {
            switch (game_w.x0F) {
            default:
                k = 0x3B6;
                break;
            case 2:
                k = -0x3B6;
                break;
            }
            if (d->on & 0x800) {
                d->ang = d->ang + k;
            }
            if (d->on & 0x400) {
                d->ang = d->ang - k;
            }
        }
    }
    a[0] = 0;
    a[1] = cs->ang;
    a[2] = 0;
    cpRotMatrix(a, (f32 *)&m);
    area = cw->area;
    if (area->type != 0) {
        d->cnf = &cam_cnf_chs;
    } else {
        d->cnf = &area->u.cnf;
    }
    if (pl->x714 != 0) {
        d->cnfe = &stage_camera_data_ex;
    } else {
        if (game_w.x0F == 0) {
            if (d->trg & 0x1000) {
                if (cw->zoom > 0) {
                    cw->zoom--;
                }
            }
            if (d->trg & 0x2000) {
                if (cw->zoom < 3) {
                    cw->zoom++;
                }
            }
        } else {
            if (d->trg & 0x2000) {
                if (cw->zoom > 0) {
                    cw->zoom--;
                }
            }
            if (d->trg & 0x1000) {
                if (cw->zoom < 3) {
                    cw->zoom++;
                }
            }
        }
        if (pl->st == 1 && cw->zoom == 2) {
            d->cnfe = &d->cnf->e[4];
        } else {
            d->cnfe = &d->cnf->e[cw->zoom];
        }
    }
    in[0] = 0;
    in[1] = d->cnfe->y;
    in[2] = d->cnfe->z;
    gnd = d->cnfe->gnd;
    if (pl->x10 == 0) {
        d->tar_d[1] = pl->pos[1] + d->cnfe->tar_y;
    } else {
        d->tar_d[1] = 140.0f + pl->pos[1];
    }
    if (pl->flag604 != 3 && pl->flag604 != 0) {
        in[0] = 0;
        in[1] = 400.0f;
        in[2] = 400.0f;
        d->tar_d[1] = 100.0f + pl->pos[1];
    } else if (pl->flag604 != 0 || act_ck(pl, 0, 0x11) != 0 || act_ck(pl, 0, 0x1B) != 0 || act_ck(pl, 0, 0x1E) != 0 || act_ck(pl, 0, 0x3A) != 0) {
        in[0] = 0;
        in[1] = 20.0f;
        in[2] = d->cnfe->z;
        d->tar_d[1] = 30.0f + pl->pos[1];
    }
    flvecApplyMat33(o, in, (f32 *)&m);
    lim = 100.0f;
    d->tar_d[0] = pl->pos[0];
    d->tar_d[2] = pl->pos[2];
    d->tar[0] = pl->pos[0];
    d->tar[2] = pl->pos[2];
    d->eye_d[0] = d->tar_d[0] + o[0];
    d->eye_d[1] = pl->pos[1] + o[1];
    d->eye_d[2] = d->tar_d[2] + o[2];
    g = GetGroundHit(d->tar_d);
    if (!(g - d->gnd <= -100.0f)) {
        d->gflag = 0;
        d->gnd = g;
    } else {
        switch (d->gflag) {
        case 0:
            d->gflag = 1;
        case 1:
            lim = 1000.0f;
            break;
        default:
            d->gflag = 0;
            d->gnd = g;
            break;
        }
    }
    g = GetGroundHit(d->eye_d);
    if (g - d->eye_d[1] < lim) {
        g = g + gnd;
        if (!(g <= d->eye_d[1])) {
            d->eye_d[1] = g;
        } else {
            d->gflag = -1;
        }
    }
    ca = cs->ang;
    da = d->ang;
    diff = (u16)(da - ca);
    if (diff > 0x8000) {
        cs->ang = ca - (s16)((u16)(ca + 0x10000 - da) / 6);
    } else {
        cs->ang = ca + (s16)(diff / 6);
    }
    d->tar[1] = d->tar[1] + 0.25f * (d->tar_d[1] - d->tar[1]);
    d->eye[1] = d->eye[1] + 0.125f * (d->eye_d[1] - d->eye[1]);
    d->eye[0] = d->eye[0] + (d->eye_d[0] - d->eye[0]);
    d->eye[2] = d->eye[2] + (d->eye_d[2] - d->eye[2]);
    k_HitEmCamera(d->eye, d->old, diff);
    k_HitWallCamera(d->eye, d->old, &d->hit_h);
    if (game_w.gate_open != 0) {
        d->wall = GetWallHitLine(d->tar, d->eye, hit, 0xC009);
    } else {
        d->wall = GetWallHitLine(d->tar, d->eye, hit, 9);
    }
    g = GetGroundHit(d->eye);
    g2 = g + gnd;
    if (!(g2 <= d->eye[1])) {
        d->eye[1] = g2;
    }
    if (d->x6E == 0) {
        lim = 300.0f;
    } else {
        lim = 400.0f;
        d->x6E = 0;
    }
    PointToPoint(v, d->tar, d->eye);
    len = flvecCalcLength(v);
    if (len < lim) {
        f32 *vp = &v[1];
        f32 x2, y2, z2;
        x2 = v[0] * v[0];
        y2 = *vp * *vp;
        z2 = v[2] * v[2];
        if (1.1780972f < flArcCos(flSqrt((x2 + z2) / (z2 + (x2 + y2))))) {
            d->x6E = 1;
            len = len * flSin(1.1780972f);
            if (*vp < 0.0f) {
                d->tar[1] = d->eye[1] - len;
            } else {
                d->tar[1] = d->eye[1] + len;
            }
        }
    }
    if (cw->area_chg != 0) {
        f = 0xF;
        if (area != NULL) {
            p = area->blend;
            if (p != NULL) {
                n = 16;
                do {
                    if (p[0] == cw->area_old) {
                        f = p[1];
                        break;
                    }
                    p += 2;
                    n--;
                } while (n != 0);
            }
        }
        if (f != 1) {
            if (f != 0) {
                cs->step.b = 1;
                cs->cnt = f - 1;
                d->rate = 1.0f / (f32)f;
            } else {
                cs->step.b = 0;
            }
        } else {
            cs->step.b = 0;
        }
    }
    if (!(d->hit_h <= 250.0f)) {
        d->hit_h = 250.0f;
    }
    flvecCopy(cs->eye, d->eye);
    flvecCopy(cs->tar, d->tar);
    cs->eye[1] += d->hit_h;
    switch (cs->step.b) {
    case 0:
        cs->roll = d->roll;
        cs->fov = d->fov;
        return;
    case 1:
        flvecCopy(cs->eye_f, cw->eye);
        flvecCopy(cs->tar_f, cw->tar);
        cs->roll_f = cw->roll;
        cs->fov_f = cw->fov;
        cs->step.b++;
    case 2:
        t = (f32)cs->cnt * d->rate;
        cpInterVector(cs->eye, cs->eye_f, cs->eye, t);
        cpInterVector(cs->tar, cs->tar_f, cs->tar, t);
        cs->roll = cs->roll_f * t + d->roll * (1.0f - t);
        cs->fov = cs->fov_f * t + d->fov * (1.0f - t);
        cs->cnt--;
        if (cs->cnt == 0) {
            cs->step.b = 0;
        }
        return;
    }
}

/* cam_sub_stg (0x2206B0-0x220EDC): stage (fixed / rail / pan) camera. Written
 * from the m2c draft and the asm; not yet compared with check.py. */
typedef struct CAMSPL { u8 b[0x30]; } CAMSPL;
extern CAMSPL SplineRvalue[];
void CamRailMove(CAMW *, CAMSPL *, f32 *, s32);
void CamRailPoint(f32 *, f32 *, f32);           /* (out, spline section, t): camr2.c */
void GetPanTarget(CAMW *, f32 *, CAMAREA *);
void GetRailTarget(CAMW *, f32 *, CAMAREA *, f32 *);
void GetRailCamPos(f32 *, CAMW *, CAMAREA *, CAMSPL *);
f32 ZoomBaseAngleRail(f32, void *, u8);
f32 RollAngleRail(f32, void *, u8);
f32 ZoomRateCalc(f32, CAMAREA *);
void SubVector(f32 *, f32 *, f32 *);
s16 AarcTan2(f32, f32);
f32 CalcDistanceXZ(f32 *, f32 *);
void flvecRotX(f32 *, f32);
f32 flvecCalcDistance(f32 *, f32 *);

void cam_sub_stg(CAMW *cw, CAMS *cs) {
    CAMSPL *spl = SplineRvalue;
    CAMAREA *area;
    CAMD_STG *d;
    PLW *pl;
    f32 v[3];
    f32 base;
    f32 sc;
    f32 t;
    s16 tx, ty, ax, ay;
    s16 lim, spd, df;
    s32 f;
    u8 *p;
    s32 n;

    cs->act = 0;
    area = cw->area;
    if (area->type == 0) {
        cs->mode.w = 0;
        cw->cam_no = 0;
        return;
    }
    pl = cw->pl;
    d = &cs->d.stg;
    switch (cs->mode.b) {
    case 0:
        cs->mode.b++;
        cs->act = 1;
        cs->step.b = 0;
        switch (area->type) {
        case 1:
            flvecCopy(d->eye, area->u.fix.pos);
            GetPanTarget(cw, d->tar, area);
            break;
        case 3:
            AddVector(d->eye, pl->pos, area->u.fix.pos);
            GetPanTarget(cw, d->tar, area);
            break;
        case 2:
            CamRailMove(cw, spl, pl->pos, 0);
            CamRailPoint(v, (f32 *)&spl[cw->rail_no], cw->rail_t);
            GetRailTarget(cw, d->tar, area, v);
            GetRailCamPos(d->eye, cw, area, spl);
            break;
        }
        SubVector(cs->vec, d->tar, d->eye);
        cs->ay = AarcTan2(cs->vec[0], cs->vec[2]);
        cs->ax = AarcTan2(-cs->vec[1], CalcDistanceXZ(d->tar, d->eye));
        d->vy = 0;
        d->vx = 0;
        break;
    case 1:
        cs->act = 1;
        switch (area->type) {
        case 1:
            flvecCopy(d->eye, area->u.fix.pos);
            GetPanTarget(cw, d->tar, area);
            base = area->u.fix.fov;
            d->roll = area->u.fix.roll;
            break;
        case 3:
            AddVector(d->eye, pl->pos, area->u.fix.pos);
            GetPanTarget(cw, d->tar, area);
            base = area->u.fix.fov;
            d->roll = area->u.fix.roll;
            break;
        case 2:
            if (cw->area_chg == 0) {
                CamRailMove(cw, spl, pl->pos, 1);
            } else {
                CamRailMove(cw, spl, pl->pos, 0);
            }
            CamRailPoint(v, (f32 *)&spl[cw->rail_no], cw->rail_t);
            base = ZoomBaseAngleRail(cw->rail_u, &area->u, cw->rail_no);
            d->roll = RollAngleRail(cw->rail_u, &area->u, cw->rail_no);
            GetRailTarget(cw, d->tar, area, v);
            GetRailCamPos(d->eye, cw, area, spl);
            break;
        }
        SubVector(cs->vec, d->tar, d->eye);
        if (cw->area_chg != 0) {
            cs->ay = AarcTan2(cs->vec[0], cs->vec[2]);
            cs->ax = AarcTan2(-cs->vec[1], CalcDistanceXZ(d->tar, d->eye));
            d->vy = 0;
            d->vx = 0;
        } else {
            ty = AarcTan2(cs->vec[0], cs->vec[2]);
            sc = CalcDistanceXZ(d->tar, d->eye);
            tx = AarcTan2(-cs->vec[1], sc);
            sc = d->fov;
            ax = cs->ax;
            lim = 521.5189f * sc;
            spd = 52.15189f * sc;
            df = tx - ax;
            if (lim < df) {
                cs->ax = tx - lim;
                d->vx = spd;
            } else if (df < -lim) {
                cs->ax = tx + lim;
                d->vx = -spd / 2;
            } else if (d->vx != 0) {
                cs->ax = ax + d->vx;
                n = (s16)(d->vx >> 3);
                if (n == 0) {
                    d->vx = 0;
                } else {
                    d->vx = d->vx - n;
                }
            }
            ay = cs->ay;
            lim = 1.4285715f * (651.8986f * d->fov);
            df = ty - ay;
            if (lim < df) {
                d->vy = ay;
                cs->ay = ty - lim;
                d->vy = cs->ay - d->vy;
                d->vy = d->vy - (s16)(d->vy >> 2);
            } else if (df < -lim) {
                d->vy = ay;
                cs->ay = ty + lim;
                d->vy = cs->ay - d->vy;
                d->vy = d->vy - (s16)(d->vy >> 2);
            } else if (d->vy != 0) {
                cs->ay = ay + d->vy;
                n = (s16)(d->vy * 20 / 100);
                if (n == 0) {
                    d->vy = 0;
                } else {
                    d->vy = d->vy - n;
                }
            }
        }
        v[0] = 0;
        v[1] = 0;
        v[2] = flvecCalcLength(cs->vec);
        flvecRotX(v, 0.000095873799f * cs->ax);
        flvecRotY(v, 0.000095873799f * cs->ay);
        AddVector(d->tar, d->eye, v);
        d->fov = base * ZoomRateCalc(flvecCalcDistance(d->tar, d->eye), area);
        break;
    }
    if (cw->area_chg != 0) {
        p = area->blend;
        f = 0xF;
        if (p != NULL) {
            n = 16;
            do {
                if (p[0] == cw->area_old) {
                    f = p[1];
                    break;
                }
                p += 2;
                n--;
            } while (n != 0);
        }
        if (f != 1) {
            if (f != 0) {
                cs->step.b = 1;
                d->cnt = f - 1;
                d->rate = 1.0f / (f32)f;
            } else {
                cs->step.b = 0;
            }
        } else {
            cs->step.b = 0;
        }
    }
    switch (cs->step.b) {
    case 0:
        flvecCopy(cs->eye, d->eye);
        flvecCopy(cs->tar, d->tar);
        cs->roll = d->roll;
        cs->fov = d->fov;
        return;
    case 1:
        flvecCopy(d->eye_f, cw->eye);
        flvecCopy(d->tar_f, cw->tar);
        d->roll_f = cw->roll;
        d->fov_f = cw->fov;
        cs->step.b++;
    case 2:
        t = (f32)d->cnt * d->rate;
        cpInterVector(cs->eye, d->eye_f, d->eye, t);
        cpInterVector(cs->tar, d->tar_f, d->tar, t);
        cs->roll = d->roll_f * t + d->roll * (1.0f - t);
        cs->fov = d->fov_f * t + d->fov * (1.0f - t);
        d->cnt--;
        if (d->cnt == 0) {
            cs->step.b = 0;
        }
        return;
    }
}

/* cam_sub_pchngr (0x220F40-0x2213F8): pachinger cannon camera: the camera
 * sits on the cannon matrix and the stick zooms the angle of view. Written
 * from the m2c draft and the asm; not yet compared with check.py. */
extern f32 pch_pos[][6];
s32 Pl_scope_ck(PLW *);
s32 pch_lock_chk(PLW *);
void flmatCopy(f32 *, f32 *);
void flmatGetTrans(f32 *, f32 *);
void flmatInit(FLMAT *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void flvecApplyMat33(f32 *, f32 *, f32 *);

void cam_sub_pchngr(CAMW *cw, CAMS *cs) {
    CAMD_PCH *d;
    PLW *pl;
    f32 *fp;
    f32 v[3];
    f32 t[3];
    f32 w[3];

    pl = cw->pl;
    cs->act = 0;
    if (pl->pch_on == 0) {
        cs->mode.w = 0;
        return;
    }
    d = &cs->d.pch;
    d->type = PachiTypeCheck(pl);
    switch (d->type) {
    case 0:
        d->x63 = 1;
        if (Pl_scope_ck(pl) == 1) {
            d->zoom = 1;
            d->min = 0.17453294f;
            d->max = 1.0471976f;
            d->range = d->max - d->min;
        } else {
            d->zoom = 0;
        }
        break;
    case 1:
        d->x63 = 0;
        d->zoom = 1;
        d->min = 0.17453294f;
        d->max = 1.0471976f;
        d->range = d->max - d->min;
        break;
    case 2:
        d->x63 = 0x11;
        d->zoom = 0;
        break;
    }
    switch (cs->mode.b) {
    case 0:
        switch (d->type) {
        case 0:
            if (pch_lock_chk(pl) == 1) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            cs->fov = 0.7853982f;
            break;
        }
        cs->mode.b++;
    case 1:
        if (d->zoom != 0 && cw->an_pow > 0x60) {
            fp = d->type == 0 ? &d->fov0 : &d->fov1;
            if ((u16)(cw->an_ang - 0x4000) < 0x6001) {
                *fp -= 0.022340214f;
                if (*fp < d->min) {
                    *fp = d->min;
                }
            }
            if ((u16)(cw->an_ang - 0x8000 - 0x6000) < 0x6001) {
                *fp += 0.022340214f;
                if (!(*fp <= d->max)) {
                    *fp = d->max;
                }
            }
        }
        switch (d->type) {
        case 0:
            cs->fov = d->fov0;
            break;
        case 1:
            cs->fov = d->fov1;
            break;
        case 2:
            break;
        }
        switch (d->type) {
        case 0:
            if (pch_lock_chk(pl) == 0) {
                flvecCopy(d->pos, pl->pos);
                flmatCopy((f32 *)d->m, (f32 *)((u8 *)pl->mdl148 + 0x40));
            } else {
                SubVector(v, pl->pos, d->pos);
                flvecCopy(d->pos, pl->pos);
                AddVector(d->m[3], d->m[3], v);
            }
            cs->fov = d->fov0;
            break;
        case 1:
            flmatInit(&d->m);
            flmatRotXYZ33(&d->m, 0.000095873799f * pl->x8EE, 0.000095873799f * (s32)(u16)(pl->ang[1] + 0x7FFF + 1), 0.0f);
            flvecCopy(d->m[3], pl->pos);
            cs->fov = d->fov1;
            break;
        case 2:
            flmatInit(&d->m);
            flmatRotXYZ33(&d->m, 0.0f, 0.000095873799f * (s32)(u16)(pl->ang[1] + 0x7FFF + 1), 0.0f);
            flvecCopy(d->m[3], pl->pos);
            break;
        }
    default:
        flmatGetTrans(w, (f32 *)d->m);
        flvecApplyMat33(t, pch_pos[d->type], (f32 *)d->m);
        AddVector(cs->eye, w, t);
        flvecApplyMat33(t, &pch_pos[d->type][3], (f32 *)d->m);
        AddVector(cs->tar, w, t);
        cs->ang = pl->ang[1] + 0x7FFF + 1;
        d->ang = pl->ang[1] + 0x7FFF + 1;
        cs->act = 1;
        return;
    }
}
