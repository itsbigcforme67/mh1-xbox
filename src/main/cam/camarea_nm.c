/* camarea_nm - SLPM_654.95 0x00222E20-0x00223B50 (g_SetAreaData, f_default,
 * f_get_223870), not built for the PS2 (no matching attempted yet; written
 * from the asm for the PC port, logic believed equivalent).
 * Camera areas: the stage camera file (LoadCameraData -> SetCameraData)
 * holds a grid over the stage (origin, cell size, w x h cells); each cell
 * lists the areas (0x300-byte CAMAREA blocks) that may contain a point; an
 * area is a set of 0x40-byte boxes (a plane distance range plus a 4-corner
 * XZ test). With no camera file, default_area_data builds one area of type
 * 0 (follow the player) whose zoom entries come from stage_camera_data_tbl.
 * GetPanTarget / GetRailTarget / GetRailCamPos / GetNearSection /
 * GetNearPoint serve the rail cameras (area types 1-3, cam_sub_stg).
 * Field names are by offset; meanings are guesses from the code. */
#include "types.h"
#include "cam.h"
#include "game.h"

#define EB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define EH(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define EW(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define EF(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define EP(p, o) (*(u8 **)((u8 *)(p) + (o)))

extern u8 *cam_data_area;
extern f32 *stage_camera_data_tbl[4];  /* per zoom level 1-4: 7 floats per stage */
extern PLW player_work[];

void flvecCopy(f32 *, f32 *);
void AddVector(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void Spline(f32 *out, f32 *pts, int n);
int GetOrthogonalPoint(f32 *out, f32 *coef, f32 *q, int mode);
void CamRailPoint(f32 *out, f32 *c, f32 t);

/* f32 -> u32 as the PS2 code does it (values >= 2^31 through the sign bit;
 * negative values wrap) */
static inline u32 f2u(f32 f) {
    if (f < 2147483648.0f) {
        return (u32)(s32)f;
    }
    return (u32)(s32)(f - 2147483648.0f) | 0x80000000u;
}

/* 0x00222E20: one follow-the-player area covering everything, built in
 * cam_data_area; zoom entries 1-4 from the stage's rows of
 * stage_camera_data_tbl[0..3] (y, z, tar_y, gnd), entry 0 fixed. */
static void default_area_data(CAMW *cw)
{
  u8 *d = cam_data_area;
  u8 *new_var;
  int k;
  u8 *new_var2;
  new_var2 = (u8 *) (((u8 *) d) + 0x30);
  *((u16 *) (((u8 *) d) + 0x00)) = 0x102;
  *((u16 *) (((u8 *) d) + 0x02)) = 0;
  *((u16 *) (((u8 *) d) + 0x04)) = 1;
  *((u16 *) (((u8 *) d) + 0x06)) = 1;
  *((u16 *) (((u8 *) d) + 0x08)) = 20000;
  *((u16 *) (((u8 *) d) + 0x0A)) = 20000;
  *((u32 *) (((u8 *) d) + 0x0C)) = 0;
  *((u32 *) (((u8 *) d) + 0x10)) = 0;
  *((u32 *) (((u8 *) d) + 0x14)) = 20000;
  *((u32 *) (((u8 *) d) + 0x18)) = 20000;
  *((u32 *) (((u8 *) d) + 0x1C)) = 0;
  *((u32 *) (((u8 *) d) + 0x20)) = 0;
  *((u32 *) (((u8 *) d) + 0x24)) = 0;
  *((u8 **) (((u8 *) d) + 0x28)) = d + 0x30;
  *new_var2 = 0;
  *((u8 *) (((u8 *) d) + 0x31)) = 0;
  *((u8 *) (((u8 *) d) + 0x32)) = 0;
  *((u8 *) (((u8 *) d) + 0x33)) = 2;
  *((u8 *) (((u8 *) d) + 0x34)) = 0;
  new_var = (u8 *) d;
  *((u8 *) (new_var + 0x35)) = 0;
  *((f32 *) (new_var + 0x38)) = 400.0f;
  *((f32 *) (new_var + 0x3C)) = 2400.0f;
  *((f32 *) (new_var + 0x40)) = 1.0f;
  *((f32 *) (new_var + 0x44)) = 0.75f;
  *((u32 *) (new_var + 0x48)) = 0;
  *((u32 *) (new_var + 0x4C)) = 0;
  for (k = 0; k < 4; k++)
  {
    f32 *row = (f32 *) (((u8 *) stage_camera_data_tbl[k]) + (game_w.stage * 28));
    u8 *e = (d + 0x90) + (k * 0x20);
    *((f32 *) (((u8 *) e) + 0x04)) = row[1];
    *((f32 *) (((u8 *) e) + 0x08)) = row[2];
    *((f32 *) (((u8 *) e) + 0x10)) = row[4];
    *((f32 *) (((u8 *) e) + 0x18)) = row[6];
  }

  *((f32 *) (new_var + 0x74)) = 300.0f;
  *((f32 *) (new_var + 0x78)) = 160.0f;
  *((f32 *) (new_var + 0x80)) = 184.0f;
  *((f32 *) (new_var + 0x88)) = 80.0f;
  *((f32 *) (new_var + 0x50)) = 0.87266463f;
  *((f32 *) (new_var + 0x54)) = 0.0f;
  *((f32 *) (new_var + 0x58)) = 0.0f;
  *((u16 *) (new_var + 0x5E)) = 0;
  *((u16 *) (new_var + 0x5C)) = 0;
  cw->data = d;
}

/* 0x00223000 */
void StageCamInit(CAMW *cw, u8 no) {
    u8 *d;

    if (cw->data == 0) {
        default_area_data(cw);
    }
    d = cw->data;
    EH(cw, 0x590) = EH(d, 0x4);
    EH(cw, 0x592) = EH(d, 0x6);
    EH(cw, 0x594) = EH(d, 0x8);
    EH(cw, 0x596) = EH(d, 0xA);
    EW(cw, 0x598) = EW(d, 0xC);
    EW(cw, 0x59C) = EW(d, 0x10);
    EW(cw, 0x5A0) = EW(d, 0x14);
    EW(cw, 0x5A4) = EW(d, 0x18);
}

s32 CameraAreaCheck(CAMAREA *a, PLW *pl, s32 mask);

/* 0x00223070: the area the camera's player is in. -1 no camera data,
 * 0 none found (area = first block), 1 found (area set). */
s32 SetAreaData(CAMW *cw) {
    PLW *pl = &player_work[EB(&game_w, 0xD1)];
    u8 *d = cw->data;
    u8 *cell;
    CAMAREA **l;
    u32 n;

    if (d == 0) {
        cw->area = 0;
        return -1;
    }
    cw->area = (CAMAREA *)EP(d, 0x28);
    if ((u8)cw->grid != 0) {
        return 0;
    }
    if (EP(d, 0x1C) == 0) {
        return 0;
    }
    cell = EP(d, 0x1C) + ((u16)cw->gx + EH(cw, 0x590) * (u16)cw->gz) * 8;
    n = EW(cell, 0);
    if (n == 0) {
        return 0;
    }
    l = (CAMAREA **)EP(cell, 4);
    for (; n != 0; n--, l++) {
        if (CameraAreaCheck(*l, pl, 1) == 0) {
            cw->area = *l;
            return 1;
        }
    }
    return 0;
}

/* 0x00223190: grid cell of pos (hdr = CameraWork+0x590: w, h, cell x, cell
 * z, origin x, origin z). Bit 0: x clamped; 0x10: z clamped. */
s8 Get_cam_grid_XZ(s16 *gx, s16 *gz, f32 *pos, u8 *hdr) {
    s8 r = 0;
    u32 i;

    u32 a = pos[0];
    u32 w = EW(hdr, 0x8);
    i = (a - w) / EH(hdr, 0x4);
    if (!(i < EH(hdr, 0x0))) {
        r |= 1;
        i = EH(hdr, 0x0) - 1;
    }
    *gx = i;
    a = pos[2];
    w = EW(hdr, 0xC);
    i = (a - w) / EH(hdr, 0x6);
    if (!(i < EH(hdr, 0x2))) {
        i = EH(hdr, 0x2) - 1;
        r = 0x10;
    }
    *gz = i;
    return r;
}

/* 0x002233C0: areas with flag 0x80 only count for the player actions
 * 0x27-0x2B of state 0 (cannon? guess) */
s32 CamAreaAttribChk(CAMAREA *a, PLW *pl) {
    if (EB(a, 6) & 0x80) {
        if (EB(pl, 0x14) != 0 || EB(pl, 0x15) < 0x27 || EB(pl, 0x15) > 0x2B) {
            return 0;
        }
    }
    return 1;
}

/* 0x00223410: p inside the quad of box q (corners (q0,q2) (q1,q3) (q4,q6)
 * (q5,q7) as the four edge tests read them)? 0 inside, 2 outside. */
s32 Area_XZ_Check(f32 *q, f32 *p) {
    f32 dx0 = p[0] - q[0];
    f32 dz0 = p[2] - q[2];
    f32 dx1 = p[0] - q[1];
    f32 dz1 = p[2] - q[3];

    if ((q[2] - q[6]) * dx0 - dz0 * (q[0] - q[4]) < 0.0f) return 2;
    if ((q[7] - q[2]) * dx0 - dz0 * (q[5] - q[0]) < 0.0f) return 2;
    if ((q[6] - q[3]) * dx1 - dz1 * (q[4] - q[1]) < 0.0f) return 2;
    if ((q[3] - q[7]) * dx1 - dz1 * (q[1] - q[5]) < 0.0f) return 2;
    return 0;
}

/* 0x002232A0: is the entity inside one of the area's boxes (box flag byte
 * +0x2C & mask skips it)? 0 yes, 2 no. */
s32 CameraAreaCheck(CAMAREA *a, PLW *pl, s32 mask) {
    u8 *b;
    u32 n;
    f32 v[3];
    f32 d;

    if (CamAreaAttribChk(a, pl) == 0) {
        return 2;
    }
    n = EB(a, 5);
    b = EP(a, 0x18);
    for (; n != 0; n--, b += 0x40) {
        if (EB(b, 0x2C) & (mask & 0xFF)) continue;
        v[0] = pl->pos[0] - EF(b, 0x20);
        v[1] = pl->pos[1] - EF(b, 0x24);
        v[2] = pl->pos[2] - EF(b, 0x28);
        d = flvecInnerProduct((f32 *)(b + 0x30), v);
        if (d < 0.0f) continue;
        if (!(d <= EF(b, 0x3C))) continue;
        if (Area_XZ_Check((f32 *)b, pl->pos) == 0) {
            return 0;
        }
    }
    return 2;
}

/* nlCalcPoint (0x00120EC0): out = v * m (3x3) + m translation */
void nlCalcPoint(f32 *out, f32 *v, f32 *m) {
    flvecApplyMat33(out, v, m);
    out[0] += m[12];
    out[1] += m[13];
    out[2] += m[14];
}

/* 0x00223500: target of a pan camera: +4 = 0 fixed point (+0x2C), 1 a
 * point on the player's model matrix (+0x38, PLW+0x60), 2 player + offset
 * (+0x44); +3 = 0 keeps y (+0x30), 1 keeps x/z. */
void GetPanTarget(CAMW *cw, f32 *out, CAMAREA *a) {
    u8 *pl = (u8 *)cw->pl;

    switch (EB(a, 4)) {
    case 0:
        flvecCopy(out, (f32 *)((u8 *)a + 0x2C));
        break;
    case 1:
        nlCalcPoint(out, (f32 *)((u8 *)a + 0x38), (f32 *)(pl + 0x60));
        break;
    case 2:
        AddVector(out, (f32 *)(pl + 0xAC), (f32 *)((u8 *)a + 0x44));
        break;
    }
    switch (EB(a, 3)) {
    case 0:
        out[1] = EF(a, 0x30);
        break;
    case 1:
        out[0] = EF(a, 0x2C);
        out[2] = EF(a, 0x34);
        break;
    }
}

/* 0x002235E0: the same for rail cameras (v = point on the rail) */
void GetRailTarget(CAMW *cw, f32 *out, CAMAREA *a, f32 *v) {
    u8 *pl = (u8 *)cw->pl;

    switch (EB(a, 4)) {
    case 0:
        flvecCopy(out, v);
        break;
    case 1:
        nlCalcPoint(out, (f32 *)((u8 *)a + 0x290), (f32 *)(pl + 0x60));
        break;
    case 2:
        AddVector(out, (f32 *)(pl + 0xAC), (f32 *)((u8 *)a + 0x29C));
        break;
    }
    switch (EB(a, 3)) {
    case 0:
        out[1] = v[1];
        break;
    case 1:
        out[0] = v[0];
        out[2] = v[2];
        break;
    }
}

/* 0x002236D0: camera position on the rail spline: section CameraWork+0x5B4
 * at CameraWork+0x5B0 times the section length (+0x2C + 16 * section). */
void GetRailCamPos(f32 *out, CAMW *cw, CAMAREA *a, f32 *spl) {
    u8 sec;

    Spline(spl, (f32 *)((u8 *)a + 0x20), EB(a, 0x280));
    sec = EB(cw, 0x5B4);
    CamRailPoint(out, spl + sec * 12, EF(cw, 0x5B0) * EF((u8 *)(sec * 16) + (int)a, 0x2C));
}

/* 0x00223760: rail point (+0x100, 12-byte stride, +0x260 points) nearest to
 * p, as a section index 0 .. n-2 (the lower end of the nearer neighbour). */
u8 GetNearSection(u8 *rail, f32 *p) {
    f32 d[16];
    f32 best = 10000000.0f;
    int sec = 0;
    int i;
    int n = EB(rail, 0x260);

    for (i = 0; i < n; i++) {
        d[i] = flvecCalcDistance(p, (f32 *)(rail + 0x100 + i * 12));
        if (!(best <= d[i])) {
            best = d[i];
            sec = i;
        }
        n = EB(rail, 0x260);
    }
    if (sec != 0 && sec < n - 1) {
        if (d[sec - 1] < d[sec + 1]) {
            sec--;
        }
    } else if (!(sec < n - 1)) {
        sec = n - 2;
    }
    return sec;
}

/* 0x00223870: of the n candidate parameters t[] pick the one inside
 * [0, seg[3]] (stored, returns 0), else the nearest end (-1 start,
 * 1 end). */
s32 get_near_point_sub(f32 *out, f32 *coef, f32 *seg, f32 *t, s32 n) {
    s32 r;
    f32 best;
    f32 v;

    v = t[0];
    if (v < 0.0f) {
        *out = 0.0f;
        r = -1;
        best = -v;
    } else if (!(v <= seg[3])) {
        best = v - seg[3];
        r = 1;
        *out = seg[3];
    } else {
        *out = v;
        return 0;
    }
    if (n == 1) {
        return r;
    }
    for (n--; n != 0; n--) {
        t++;
        v = *t;
        if (v < 0.0f) {
            if (!(best <= -v)) {
                best = -v;
                r = -1;
                *out = 0.0f;
            }
        } else if (!(v <= seg[3])) {
            if (!(best <= v - seg[3])) {
                *out = seg[3];
                r = 1;
                best = v - seg[3];
            }
        } else {
            *out = v;
            return 0;
        }
    }
    return r;
}

/* 0x00223980: parameter of the rail point nearest to q in section rp[8]
 * (moves to the neighbouring section when it lies beyond an end).
 * rp: f32 parameter at +0, u8 section at +8. 0 when there is none. */
s32 GetNearPoint(f32 *rp, f32 *coef, u8 *rail, f32 *q) {
    f32 t[8];
    s32 k;
    s32 r;
    u8 *sec = (u8 *)rp + 8;

    Spline(coef, (f32 *)(rail + 0x100), EB(rail, 0x260));
    k = GetOrthogonalPoint(t, coef + *sec * 12, q, EB(rail, 0x261));
    if (k == 0) {
        *(s32 *)rp = 0;
        return 0;
    }
    r = get_near_point_sub(rp, coef + *sec * 12, (f32 *)(rail + *sec * 16 + 0x100), t, k);
    if (r > 0) {
        if (*sec < EB(rail, 0x260) - 2) {
            k = GetOrthogonalPoint(t, coef + (*sec + 1) * 12, q, EB(rail, 0x261));
            if (k != 0) {
                (*sec)++;
                get_near_point_sub(rp, coef + *sec * 12, (f32 *)(rail + *sec * 16 + 0x100), t, k);
            }
        }
    } else if (r < 0) {
        if (*sec > 0) {
            k = GetOrthogonalPoint(t, coef + (*sec - 1) * 12, q, EB(rail, 0x261));
            if (k != 0) {
                (*sec)--;
                get_near_point_sub(rp, coef + *sec * 12, (f32 *)(rail + *sec * 16 + 0x100), t, k);
            }
        }
    }
    return 1;
}
