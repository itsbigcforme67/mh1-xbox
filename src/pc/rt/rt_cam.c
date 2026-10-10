/*
 * rt_cam.c - what the game camera C (src/main/cam/: CameraMove and the
 * std / stage / demo cameras, agent D; camarea_nm.c camera areas) needs
 * from the PS2 side, and the host entry points rt_cam_init / rt_cam_tick.
 *
 * Written from the asm: LoadCameraData (0x11F1E0), RollView /
 * SetAngleOfView (0x169DA0, lpView +0x34 / +0x2C), cpInterVector
 * (0x120E30), SubVector (0x120860), AarcTan2 (0x120E80), Pl_scope_ck
 * (0x154D00), Pl_bari_ck (0x14FC40), flPow (powf), flMemset.
 * View_move (0x169A80) is replaced by the host: it reads lpView (eye,
 * target, roll, fov) and builds its own camera from it (rt_cam_view).
 * Not ported (stubs, see each): Cockpit_chat_chk (online chat menu), hit_data_expand / body_ptr_ck2
 * (monster body capsules: k_HitEmCamera finds no monster parts yet).
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "cam.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------ data */
/* cam_data_area (0x38A204): where LoadCameraData puts the stage camera
 * file (or default_area_data builds its one area). */
#define CAM_AREA_SIZE (64u << 10)    /* largest camera file 3460 bytes (st004cmd.bin); was 1 MB */
u8 *cam_data_area;

/* lpView (0x38A110): the view the camera writes: eye 0x00, target 0x0C,
 * up 0x18, fov 0x2C (last fov 0x30), roll 0x34. */
static f32 view[16];
f32 *lpView = view;

/* em_body_tbl of lobby.bin (D_610370: the NPCs' body volumes the camera
 * keeps out of) comes from lobby.bin (tools/gen_rt_auto.py) */

/* ------------------------------------------------------------ helpers */
void RollView(f32 r) { lpView[0x34 / 4] = r; }
void SetAngleOfView(f32 a) { lpView[0x2C / 4] = a; }

f32 flArcTan2(f32, f32);

f32 flPow(f32 a, f32 b) { return powf(a, b); }
void flMemset(void *p, s32 v, s32 n) { memset(p, v, (size_t)n); }

int act_ck(void *chr, int a, int b);

/* Game_clear_ck (0x162DB0): src/main/font/dsp01.c (matched) */

/* Cockpit_chat_chk (0x275220): the online chat menu is open; never here */
s32 Cockpit_chat_chk(void) { return 0; }

/* hit_data_expand: src/pc/rt/rt_eft.c (joint matrices from the viewer) */
/* body_ptr_ck2: src/main/hit/hit_nm.c (built) */

/* View_move: the host builds its camera from lpView (rt_cam_view). */
void View_move(void) {}

/* ------------------------------------------------------------ loading */
extern s32 camera_data_tbl[];
int load_file_mdl(s32 dst, s32 idx);
void SetCameraData(void *d);

/* LoadCameraData (0x11F1E0): the stage's camera file (camera_data_tbl,
 * 0 = none) into cam_data_area and SetCameraData (NULL when none). */
void LoadCameraData(int stage)
{
    s32 idx = camera_data_tbl[stage];
    void *d = NULL;
    if (idx != 0) {
        load_file_mdl((s32)cam_data_area, idx);
        d = cam_data_area;
    }
    SetCameraData(d);
}

/* ------------------------------------------------------------ host side */
extern CAMW CameraWork;
void CameraWorkInit(void);
void Q_camera_init(void);
void CameraInit(void);
void CameraMove(void);

/* Camera for the current stage, following player_work[game_w.master]:
 * CameraWorkInit, Q_camera_init, LoadCameraData, CameraInit (the order of
 * the game's stage start is a guess). */
void rt_cam_init(int stage)
{
    if (!cam_data_area) {
        if (!(cam_data_area = calloc(1, CAM_AREA_SIZE)))
            return;
        rt_area_register(cam_data_area, CAM_AREA_SIZE, "cam_data_area");
    }
    memset(cam_data_area, 0, CAM_AREA_SIZE);
    CameraWorkInit();
    Q_camera_init();
    LoadCameraData(stage);
    CameraInit();
    lpView[0x2C / 4] = lpView[0x30 / 4] = 0.87266463f;
}

/* One tick of the game camera (CameraMove: pad, area, the five slots,
 * cam2view -> lpView). */
void rt_cam_tick(void)
{
    static int tr = -1;
    CameraMove();
    if (tr < 0)
        tr = getenv("RT_CAM_TRACE") != NULL;
    if (tr) {
        CAMS *cs = &CameraWork.sl[0];
        CAMD_STD *d = &cs->d.std;
        fprintf(stderr, "cam: no %d area %p type %d zoom %d sw %04X | std ang %04X want %04X wall %d | eye %.0f %.0f %.0f tar %.0f %.0f %.0f fov %.2f\n",
                CameraWork.cam_no, (void *)CameraWork.area, CameraWork.area ? CameraWork.area->type : -1,
                CameraWork.zoom, CameraWork.sw_on, cs->ang & 0xFFFF, d->ang & 0xFFFF, d->wall,
                lpView[0], lpView[1], lpView[2], lpView[3], lpView[4], lpView[5], lpView[0x2C / 4]);
        {
            int k;
            for (k = 2; k < 5; k++)
                if (CameraWork.sl[k].act)
                    fprintf(stderr, "cam:   slot %d act %d mode %d step %d cnt %d eye %.0f %.0f %.0f fov %.2f demo pos %d/%d tar %d/%d no %d state %d\n", k,
                            CameraWork.sl[k].act, CameraWork.sl[k].mode.w, CameraWork.sl[k].step.w, CameraWork.sl[k].cnt,
                            CameraWork.sl[k].eye[0], CameraWork.sl[k].eye[1], CameraWork.sl[k].eye[2], CameraWork.sl[k].fov,
                            CameraWork.sl[k].d.demo.pos_mode, CameraWork.sl[k].d.demo.pos_part, CameraWork.sl[k].d.demo.tar_mode, CameraWork.sl[k].d.demo.tar_part,
                            CameraWork.sl[k].d.demo.no, CameraWork.sl[k].d.demo.state);
        }
    }
}

/* The view the game camera produced: eye, target, roll and fov
 * (radians). */
void rt_cam_view(float eye[3], float tar[3], float *roll, float *fov)
{
    int k;
    for (k = 0; k < 3; k++) {
        eye[k] = lpView[k];
        tar[k] = lpView[3 + k];
    }
    *roll = lpView[0x34 / 4];
    *fov = lpView[0x2C / 4];
}
