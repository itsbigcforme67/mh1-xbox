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
