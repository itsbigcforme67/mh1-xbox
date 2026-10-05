/* camp - SLPM_654.95 0x00220F30-0x002213F4 (f_cam): pachinger cannon camera:
 * the camera sits on the cannon matrix and the stick zooms the angle of view
 * (guess from the code). */
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
