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
