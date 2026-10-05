/* cam - SLPM_654.95 0x0021F3D0-0x00222E20 (f_cam): game camera.
 * CameraMove runs each frame: reads the pad, finds the camera area the
 * player is in, runs the five camera slots (std = behind the player,
 * stage = fixed/rail cameras from the area data, pachinger cannon, player
 * EX = fishing and NPC zoom, demo = scripted point cameras) and cam2view
 * copies the winning slot into lpView with the screen quake. */
#include "cam.h"
#include "game.h"

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
void flvecRotY(f32, f32 *);
void AddVector(f32 *, f32 *, f32 *);
void cpInterVector(f32, f32 *, f32 *, f32 *);
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
    u8 *p;
    CAMGRID *g;
    u8 **l;
    s32 n;
    s32 j;
    CAMBLK *b;
    s32 i;

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
    cs->fov = 0.7853982f;
    cs->roll = 0.0f;
    cs->d.pch.min = 0.17453294f;
    cs->d.pch.max = 1.0471976f;
    cs->d.pch.range = cs->d.pch.max - cs->d.pch.min;
    cs->d.pch.fov1 = cs->d.pch.fov0 = cs->fov;
}

s32 pch_lock_chk(PLW *pl) {
    switch (pl->char0) {
    case 0x580:
    case 0x57D:
    case 0x583:
    case 0x57C:
    case 0x579:
    case 0x3EA:
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
    flvecRotY(a, v);
    AddVector(cs->eye, pl->pos, v);
    flvecCopy(v, ofs + 3);
    flvecRotY(a, v);
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
        cpInterVector(0.5f, z->tar, pl->pos, npc->pos);
        z->tar[1] += 64.0f;
    } else {
        cpInterVector(0.2f, z->tar, pl->pos, z->pos);
        z->tar[1] += 150.0f;
    }
    t = z->cnt * (1.0f / 15.0f);
    cpInterVector(t, cs->tar, z->tar, src->tar);
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
