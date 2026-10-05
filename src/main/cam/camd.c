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
