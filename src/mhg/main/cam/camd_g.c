/* G only (SLPM_658.69 0x001DC0D0-0x001DC640): the NPC zoom camera. G adds a manual mode (step byte x21 = 1: the
 * pad moves the camera up/down and in/out around the hunter) and a fixed mode (x21 = 2, stage 77 = 0x4D); MH1's version
 * is in src/main/cam/camd.c. Function names are the map's. */
#include "cam.h"
#include "game.h"

void flvecCopy(f32 *, f32 *);
void cpInterVector(f32 *, f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void SubVector(f32 *, f32 *, f32 *);
void func_00111040(f32 *, f32 *, f32);
s32 Cockpit_chat_chk(void);
extern volatile u16 Psw[]; /* Psw[4] = pad 0's PSWE.an (stick directions held), include/ioread.h */

#if !defined(G_PART) || G_PART == 1
void func_001DC0D0(PLW *npc) { /* NPCZoomInCameraRequest (g_names.txt) */
    CAMZOOM *z = &CameraWork.sl[3].d.ex.zoom;

    z->req = 1;
    z->npc = npc;
    z->x2C = 0.5235988f; /* pi/6 */
    if (npc != 0) {
        z->x21 = 0;
        flvecCopy(z->pos, npc->pos);
    } else {
        z->x21 = 1;
        z->x24 = 120.0f;
        z->x28 = 300.0f;
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
void func_001DC160(void) { /* the fixed camera (mode 2) for stage 77 */
    CAMZOOM *z;

    if (game_w.stage == 77) {
        z = &CameraWork.sl[3].d.ex.zoom;
        z->req = 1;
        z->npc = 0;
        z->x21 = 2;
        z->pos[0] = 1461.0f;
        z->pos[1] = 514.0f;
        z->pos[2] = 2744.0f;
        z->tar[0] = 1405.0f;
        z->tar[1] = 51.0f;
        z->tar[2] = 2348.0f;
        z->x2C = 1.0471976f; /* pi/3 */
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
s32 func_001DC1F0(void) { /* zoom state: -1 manual mode past 15 frames, 1 manual/fixed, 0 none */
    CAMZOOM *z = &CameraWork.sl[3].d.ex.zoom;

    switch (z->step) {
    case 1:
        if (z->cnt >= 15) {
            return -1;
        }
        /* fall through */
    case 2:
        return 1;
    default:
        return 0;
    }
}
#endif

#if !defined(G_PART) || G_PART == 3
void cam_plEX_zoom(CAMW *cw, CAMS *cs, CAMZOOM *z) {
    PLW *pl;
    PLW *npc;
    CAMS *src;
    f32 t;
    f32 v[3];

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
    pl = cw->pl;
    t = z->cnt * (1.0f / 15.0f);
    switch (z->x21) {
    case 0:
        npc = z->npc;
        if (npc->kind == 3) {
            cpInterVector(z->tar, pl->pos, npc->pos, 0.5f);
            z->tar[1] += 64.0f;
        } else {
            cpInterVector(z->tar, pl->pos, z->pos, 0.2f);
            z->tar[1] += 150.0f;
        }
        flvecCopy(cs->eye, src->eye);
        break;
    case 1:
        if (Cockpit_chat_chk() != 1) {
            if (Psw[4] & 0x20) {
                z->x24 += 2.0f;
                if (z->x24 > 160.0f) {
                    z->x24 = 160.0f;
                }
            }
            if (Psw[4] & 0x10) {
                z->x24 -= 2.0f;
                if (z->x24 < 40.0f) {
                    z->x24 = 40.0f;
                }
            }
            if (Psw[4] & 0x8) {
                z->x28 -= 8.0f;
                if (z->x28 < 120.0f) {
                    z->x28 = 120.0f;
                }
            }
            if (Psw[4] & 0x4) {
                z->x28 += 8.0f;
                if (z->x28 > 375.0f) {
                    z->x28 = 375.0f;
                }
            }
        }
        flvecCopy(z->tar, pl->pos);
        z->tar[1] += z->x24;
        SubVector(v, src->eye, z->tar);
        func_00111040(v, v, z->x28);
        AddVector(v, z->tar, v);
        cpInterVector(cs->eye, v, src->eye, t);
        break;
    case 2:
        cpInterVector(cs->eye, z->pos, src->eye, t);
        break;
    }
    cpInterVector(cs->tar, z->tar, src->tar, t);
    cs->fov = z->x2C * t + src->fov * (1.0f - t);
    cs->roll = src->roll;
    cs->act = 1;
}
#endif
