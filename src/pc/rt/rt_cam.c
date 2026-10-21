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

short act_ck(void *chr, int a, int b);

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
#ifdef VERSION_MHG
    {   /* RT_GLINE=1: every 60 ticks the ground line test (func_0010AE00) from the hunter (height RT_GLINE_H, default 150) out RT_GLINE (default 2500) along 8 directions, and the camera eye */
        static int gl = -1, gn;
        static float gd = 2500.0f, gh = 150.0f, gdy = 0.0f;
        if (gl < 0)
        {
            gl = getenv("RT_GLINE") != NULL;
            if (gl && atof(getenv("RT_GLINE")) > 1.0)
                gd = (float)atof(getenv("RT_GLINE"));
            if (getenv("RT_GLINE_DY"))
                gdy = (float)atof(getenv("RT_GLINE_DY"));
            if (getenv("RT_GLINE_H"))
                gh = (float)atof(getenv("RT_GLINE_H"));
        }
        if (gl && CameraWork.pl && (++gn % 60) == 0) {
            extern int func_0010AE00(f32 *, f32 *, f32 *, int);
            f32 a[3], b[3], o[3];
            int k;
            a[0] = CameraWork.pl->pos[0]; a[1] = CameraWork.pl->pos[1] + gh; a[2] = CameraWork.pl->pos[2];
            { extern int GroundFieldInCheck(f32 *); fprintf(stderr, "gline: in %d", GroundFieldInCheck(a)); }
            for (k = 0; k < 8; k++) {
                b[0] = a[0] + gd * cosf(k * 0.7854f); b[1] = a[1] + gdy; b[2] = a[2] + gd * sinf(k * 0.7854f);
                o[0] = o[1] = o[2] = 0;
                fprintf(stderr, " %d", func_0010AE00(a, b, o, 9));
            }
            fprintf(stderr, "\n");
        }
    }
#endif
    if (tr < 0)
        tr = getenv("RT_CAM_TRACE") != NULL;
    if (tr) {
        CAMS *cs = &CameraWork.sl[0];
        CAMD_STD *d = &cs->d.std;
        fprintf(stderr, "cam: no %d area %p type %d zoom %d sw %04X | std ang %04X want %04X wall %d | eye %.0f %.0f %.0f tar %.0f %.0f %.0f fov %.2f lift %.0f\n",
                CameraWork.cam_no, (void *)CameraWork.area, CameraWork.area ? CameraWork.area->type : -1,
                CameraWork.zoom, CameraWork.sw_on, CS_STD_ANG(cs) & 0xFFFF, d->ang & 0xFFFF, d->wall,
                lpView[0], lpView[1], lpView[2], lpView[3], lpView[4], lpView[5], lpView[0x2C / 4],
#ifdef VERSION_MHG
                (double)d->x2C);
#else
                0.0);
#endif
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

/* RT_CAM_TRACE in the village: every slot that wants the view (agent D, G intro camera) */
void rt_cam_dump(int tick)
{
    int k;
    for (k = 0; k < 5; k++) {
        CAMS *c = &CameraWork.sl[k];
        if (c->act)
            fprintf(stderr, "vcam:   tick %d slot %d act %d mode %d step %d cnt %d/%d eye %.0f %.0f %.0f tar %.0f %.0f %.0f fov %.2f | demo no %d st %d pos %d/%d tar %d/%d move %d\n",
                    tick, k, c->act, c->mode.w, c->step.w, c->cnt, c->cnt_max, c->eye[0], c->eye[1], c->eye[2], c->tar[0], c->tar[1], c->tar[2], c->fov,
                    c->d.demo.no, c->d.demo.state, c->d.demo.pos_mode, c->d.demo.pos_part, c->d.demo.tar_mode, c->d.demo.tar_part, c->d.demo.move);
    }
}
