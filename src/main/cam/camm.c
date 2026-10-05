/* camm - SLPM_654.95 0x0021F4C0-0x00220420 (f_cam): CameraMove (per-frame:
 * finds the camera area, runs the five slots, cam2view), the standard
 * (behind the player) camera init and update. */
#include "cam.h"
#include "game.h"
#include "plf.h"
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

f32 GetGroundHit(f32 *);

void CameraMove(void) {
    PLW *pl = CameraWork.pl;
    CAMW *cw = &CameraWork;
    CAMS *cs;
    u32 i;
    CAMFN *f;
    CAMS *c2;
    u32 n;

    cam_sw_set_sub(cw);
    cw->grid = Get_cam_grid_XZ(&cw->gx, &cw->gz, pl->pos, cw->x590);
    cw->area_chg = 0;
    if (cw->init.b == 0) {
        cw->init.b++;
        cw->area = NULL;
        cw->area_old = 0xFF;
        cw->area_no = 0xFF;
        if (SetAreaData(cw) >= 0) {
            cw->area_no = cw->area->no;
        }
        cs = cw->sl;
        i = 0;
        f = cam_init_sub_jmp;
        do {
            cs->no = i;
            cs->mode.w = 0;
            (*f)(cw, cs);
            i++;
            f++;
            cs++;
        } while (i < 5);
        cw->cam_no = 0;
        if (cw->area != NULL) {
            if (cw->area->type == 0) {
                cw->cam_no = 0;
            } else {
                cw->cam_no = 1;
            }
        }
        cw->cam_old = cw->cam_no;
    }
    if (cw->area_no == 0) {
        if (SetAreaData(cw) > 0) {
            cw->area_old = cw->area_no;
            cw->area_no = cw->area->no;
            cw->area_chg = 1;
        }
    } else if (CameraAreaCheck(cw->area, pl, 2) != 0) {
        cw->area_old = cw->area_no;
        cw->area_chg = 1;
        if (SetAreaData(cw) >= 0) {
            cw->area_no = cw->area->no;
        }
    }
    cw->cam_old = cw->cam_no;
    if (cw->area != NULL) {
        switch (cw->area->type) {
        case 0:
            cw->cam_no = 0;
            break;
        case 1:
        case 2:
        case 3:
            cw->cam_no = 1;
            break;
        }
    } else {
        cw->cam_no = 0;
    }
    c2 = cw->sl;
    n = 5;
    do {
        cam_sub_jmp[c2->no](cw, c2);
        n--;
        c2++;
    } while (n != 0);
    cam2view(cw);
    cw->reset = 0;
    View_move();
}

void cam_init_sub_std(CAMW *cw, CAMS *cs) {
    CAMD_STD *d;
    PLW *pl;
    f32 in[3];
    f32 o[3];
    s32 a[3];
    FLMAT m;

    pl = cw->pl;
    cs->req = 0xFF;
    d = &cs->d.std;
    if (cw->area->type != 0) {
        d->cnf = &cam_cnf_chs;
    } else {
        d->cnf = &cw->area->u.cnf;
    }
    d->cnfe = &d->cnf->e[cw->zoom];
    d->fov = d->cnf->fov;
    d->roll = d->cnf->roll;
    cs->ang = pl->ang[1] + 0x7FFF + 1;
    a[0] = 0;
    a[1] = cs->ang;
    a[2] = 0;
    cpRotMatrix(a, (f32 *)&m);
    in[0] = 0;
    in[1] = d->cnfe->y;
    in[2] = d->cnfe->z;
    flvecApplyMat33(o, in, (f32 *)&m);
    d->tar[0] = pl->pos[0];
    d->tar[1] = pl->pos[1] + d->cnfe->tar_y;
    d->tar[2] = pl->pos[2];
    d->eye[0] = d->tar[0] + o[0];
    d->eye[1] = pl->pos[1] + o[1];
    d->eye[2] = d->tar[2] + o[2];
    d->ang = cs->ang;
    flvecCopy(d->eye_d, d->eye);
    flvecCopy(d->tar_d, d->tar);
    d->gnd = GetGroundHit(d->tar);
    d->gflag = -1;
    flvecCopy(d->old, d->eye);
    d->fov_d = d->fov;
    d->roll_d = d->roll;
    d->x6E = 0;
}
