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
