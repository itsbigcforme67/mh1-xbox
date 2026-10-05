/* shit8_nm (not built): ground queries of f_sphr that report the stage area
 * height for players and monsters (0x001198B0-0x0011ACF0). Each takes the
 * entity (stage number at +0x736), a position and returns the ground height
 * under it: 1 = on ground, 0 = outside the loaded ground file (stage floor
 * height used), -1 = entity not on the stage. Candidate heights are the
 * polygons under the point (up to 5); the highest one not above y + 50 is
 * taken (y + 100 for monsters on special kinds), else the lowest. The
 * Status variants also report the polygon's attribute word and a water /
 * special surface height. Guesses from the code. */
#include "types.h"
#include "hit3.h"

#define EF(e, T, o) (*(T *)((u8 *)(e) + (o)))

typedef struct GATTR {
    u8 kind;
    u8 b1;
    u16 h2;
} GATTR;

typedef struct STAGE_H {
    u8 _pad00[0x18];
    f32 floor;          /* 0x18 */
    f32 floor2;         /* 0x1C */
} STAGE_H;

s32 *GetGroundTblAdrs(f32 *);
int GroundFieldInCheck(f32 *);
int PointHitCheckF3(f32 *, f32 *);
u8 Pl_stg_ck(void *);
STAGE_H *Stage_data_get(int);
extern HKIND *ground_tbl_add[];
extern u8 *stage_work[];

/* 0x001198B0 */
int GetGroundHitArea(void *ent, f32 *pos, f32 *out) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    f32 *hp = h;
    f32 *fp;
    f32 r;

    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(EF(ent, u8, 0x736))->floor;
        return -1;
    }
    h[0] = pos[1];
    EF(ent, u8, 0x7E9) = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5) {
                    HKIND *k = &ground_tbl_add[EF(ent, u8, 0x736)][pl->kind];
                    if (k->x0E != 0) EF(ent, u8, 0x7E9) = k->x0E;
                    if (k->water == 0) {
                        *hp = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                        n++;
                        hp++;
                    }
                }
                cell++;
            } while (*cell != -1);
        }
        m = n;
        if (m != 1) {
            any = 0;
            c = 0;
            i = 0;
            if (m > 0) {
                hp = h;
                fp = hf;
                do {
                    if (*hp <= 50.0f + pos[1]) {
                        any = 1;
                        *fp = *hp;
                        fp++;
                        c++;
                    }
                    i++;
                    hp++;
                } while (i < m);
            }
            if (any == 1) {
                if (c == 1) {
                    r = hf[0];
                } else {
                    i = 0;
                    if (c > 0) {
                        fp = hf;
                        do {
                            if (i == 0) {
                                h[0] = *fp;
                            } else if (h[0] < *fp) {
                                h[0] = *fp;
                            }
                            i++;
                            fp++;
                        } while (i < c);
                    }
                    r = h[0];
                }
            } else {
                i = 0;
                if (m > 0) {
                    hp = h;
                    do {
                        if (i == 0) {
                            hf[0] = *hp;
                        } else if (!(hf[0] <= *hp)) {
                            hf[0] = *hp;
                        }
                        i++;
                        hp++;
                    } while (i < m);
                }
                r = hf[0];
            }
        } else {
            r = h[0];
        }
        *out = r;
        if (*out == -0.0f) *out = 0.0f;
        return 1;
    }
    *out = ((STAGE_H *)*(void **)(stage_work + 0x12))->floor2;
    return 0;
}

/* 0x00119CA0: like GetGroundHitArea but looks for the ground ABOVE y (the
 * lowest height above it, else the highest). */
int GetGroundHitAreaUpper(void *ent, f32 *pos, f32 *out) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    f32 *hp = h;
    f32 *fp;
    f32 r;

    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(EF(ent, u8, 0x736))->floor;
        return -1;
    }
    h[0] = pos[1];
    EF(ent, u8, 0x7E9) = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5) {
                    HKIND *k = &ground_tbl_add[EF(ent, u8, 0x736)][pl->kind];
                    if (k->x0E != 0) EF(ent, u8, 0x7E9) = k->x0E;
                    if (k->water == 0) {
                        *hp = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                        n++;
                        hp++;
                    }
                }
                cell++;
            } while (*cell != -1);
        }
        m = n;
        if (m != 1) {
            any = 0;
            c = 0;
            i = 0;
            if (m > 0) {
                hp = h;
                fp = hf;
                do {
                    if (!(*hp <= pos[1])) {
                        any = 1;
                        *fp = *hp;
                        fp++;
                        c++;
                    }
                    i++;
                    hp++;
                } while (i < m);
            }
            if (any == 0) {
                i = 0;
                if (m > 0) {
                    hp = h;
                    do {
                        if (i == 0) {
                            hf[0] = *hp;
                        } else if (hf[0] < *hp) {
                            hf[0] = *hp;
                        }
                        i++;
                        hp++;
                    } while (i < m);
                }
                r = hf[0];
            } else if (c == 1) {
                r = hf[0];
            } else {
                i = 0;
                if (c > 0) {
                    fp = hf;
                    do {
                        if (i == 0) {
                            h[0] = *fp;
                        } else if (!(h[0] <= *fp)) {
                            h[0] = *fp;
                        }
                        i++;
                        fp++;
                    } while (i < c);
                }
                r = h[0];
            }
        } else {
            r = h[0];
        }
        *out = r;
        if (*out == -0.0f) *out = 0.0f;
        return 1;
    }
    *out = ((STAGE_H *)*(void **)(stage_work + 0x12))->floor2;
    return 0;
}

/* 0x0011A080: player version: also the polygon attribute (GATTR) of the
 * chosen height; *flag is cleared when off the ground. */
int GetGroundHitStatusAreaPl(void *ent, f32 *pos, GATTR *at, f32 *out, f32 *flag) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    GATTR a[8];
    GATTR af[8];
    GATTR ar;
    s32 *cell;
    HPOLY *pl;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    f32 *hp = h;
    GATTR *ap = a;
    f32 *fp;
    GATTR *afp;
    f32 r;

    EF(ent, s8, 0x7E8) = 0;
    if (!(Pl_stg_ck(ent) & 0xFF)) {
        *out = Stage_data_get(EF(ent, u8, 0x736))->floor;
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return -1;
    }
    h[0] = pos[1];
    a[0].kind = EF(ent, u8, 0x70C);
    a[0].b1 = EF(ent, u8, 0x70D);
    a[0].h2 = EF(ent, u16, 0x70E);
    EF(ent, s8, 0x7E9) = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5 &&
                    ground_tbl_add[EF(ent, u8, 0x736)][pl->kind].water == 0) {
                    *hp = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                    n++;
                    hp++;
                    ap->kind = pl->kind;
                    ap->b1 = pl->b1;
                    ap->h2 = pl->h2;
                    ap++;
                }
                cell++;
            } while (*cell != -1);
        }
        m = n;
        if (m != 1) {
            any = 0;
            c = 0;
            i = 0;
            if (m > 0) {
                hp = h;
                ap = a;
                fp = hf;
                afp = af;
                do {
                    if (*hp <= 50.0f + pos[1]) {
                        any = 1;
                        c++;
                        *afp = *ap;
                        afp++;
                        *fp = *hp;
                        fp++;
                    }
                    hp++;
                    i++;
                    ap++;
                } while (i < m);
            }
            if (any == 1) {
                if (c == 1) {
                    r = hf[0];
                    ar = af[0];
                } else {
                    i = 0;
                    if (c > 0) {
                        fp = hf;
                        afp = af;
                        do {
                            if (i == 0) {
                                h[0] = *fp;
                                a[0] = *afp;
                            } else if (h[0] < *fp) {
                                h[0] = *fp;
                                a[0] = *afp;
                            }
                            fp++;
                            i++;
                            afp++;
                        } while (i < c);
                    }
                    r = h[0];
                    ar = a[0];
                }
            } else {
                i = 0;
                if (m > 0) {
                    hp = h;
                    ap = a;
                    do {
                        if (i == 0) {
                            hf[0] = *hp;
                            af[0] = *ap;
                        } else if (!(hf[0] <= *hp)) {
                            hf[0] = *hp;
                            af[0] = *ap;
                        }
                        hp++;
                        i++;
                        ap++;
                    } while (i < m);
                }
                r = hf[0];
                ar = af[0];
            }
        } else {
            r = h[0];
            ar = a[0];
        }
        *out = r;
        at->kind = ar.kind;
        at->b1 = ar.b1;
        at->h2 = ar.h2;
        if (*out == -0.0f) *out = 0.0f;
        if (EF(ent, u8, 0x604) != 0) at->b1 = 2;
        return 1;
    }
    *out = ((STAGE_H *)*(void **)(stage_work + 0x12))->floor2;
    *flag = 0.0f;
    at->kind = 0;
    at->b1 = 0;
    at->h2 = 0;
    return 0;
}

/* 0x0011A600: monster version. A kind with x0E set marks a special surface
 * (ent+0x7E9 = its value, attribute kept in sp), a kind with water set
 * gives the water height in *flag (relative to ent+0x7E0) and attribute. */
int GetGroundHitStatusAreaEm(void *ent, f32 *pos, GATTR *at, f32 *out, f32 *flag) {
    f32 pt[4];
    f32 h[8];
    f32 tri[12];
    f32 hf[8];
    GATTR a[8];
    GATTR af[8];
    GATTR ar;
    GATTR sp;           /* special surface attribute (x0E) */
    GATTR wt;           /* water attribute */
    s32 *cell;
    HPOLY *pl;
    HKIND *k;
    s8 n = 0;
    s8 m;
    s8 any;
    s8 c;
    s8 i;
    s8 w = 0;
    s8 inw;
    f32 *hp = h;
    GATTR *ap = a;
    f32 *fp;
    GATTR *afp;
    f32 r;
    f32 t;
    STAGE_H *sd;

    EF(ent, s8, 0x7E8) = 0;
    if (!(Pl_stg_ck(ent) & 0xFF)) {
        sd = Stage_data_get(EF(ent, u8, 0x736));
        if (EF(ent, u8, 0x10) != 0 && EF(ent, u8, 2) == 0x15) {
            *out = sd->floor2;
        } else {
            *out = sd->floor;
        }
        *flag = 0.0f;
        at->kind = 0;
        at->b1 = 0;
        at->h2 = 0;
        return -1;
    }
    h[0] = pos[1];
    a[0].kind = EF(ent, u8, 0x70C);
    a[0].b1 = EF(ent, u8, 0x70D);
    a[0].h2 = EF(ent, u16, 0x70E);
    EF(ent, u8, 0x7E9) = 0;
    if (GroundFieldInCheck(pos) == 1) {
        cell = GetGroundTblAdrs(pos);
        if (*cell != -1) {
            do {
                pl = (HPOLY *)*cell;
                pt[0] = pos[0];
                pt[1] = pos[2];
                tri[0] = pl->v[0][0];
                tri[1] = pl->v[0][2];
                tri[3] = pl->v[1][0];
                tri[4] = pl->v[1][2];
                tri[6] = pl->v[2][0];
                tri[7] = pl->v[2][2];
                if (!(pl->n[1] <= 0.0f) && PointHitCheckF3(tri, pt) == 1 && n < 5) {
                    k = &ground_tbl_add[EF(ent, u8, 0x736)][pl->kind];
                    t = -(pl->d + (pl->n[0] * pos[0] + pl->n[2] * pos[2])) / pl->n[1];
                    if (k->x0E != 0) {
                        EF(ent, u8, 0x7E9) = k->x0E;
                        *flag = t;
                        sp.kind = pl->kind;
                        sp.b1 = pl->b1;
                        sp.h2 = pl->h2;
                    }
                    if (k->water != 0) {
                        w = 1;
                        *flag = t - EF(ent, f32, 0x7E0);
                        wt.kind = pl->kind;
                        wt.b1 = pl->b1;
                        wt.h2 = pl->h2;
                    } else {
                        *hp = t;
                        hp++;
                        n++;
                        ap->kind = pl->kind;
                        ap->b1 = pl->b1;
                        ap->h2 = pl->h2;
                        ap++;
                    }
                }
                cell++;
            } while (*cell != -1);
        }
        m = n;
        if (m != 1) {
            any = 0;
            c = 0;
            i = 0;
            if (m > 0) {
                hp = h;
                ap = a;
                do {
                    if (EF(ent, u8, 0x7E9) != 0) {
                        if (*hp <= 100.0f + pos[1]) {
                            any = 1;
                            af[c] = *ap;
                            hf[c] = *hp;
                            c++;
                        }
                    } else if (*hp <= 50.0f + pos[1]) {
                        any = 1;
                        af[c] = *ap;
                        hf[c] = *hp;
                        c++;
                    }
                    hp++;
                    i++;
                    ap++;
                } while (i < m);
            }
            if (any == 1) {
                if (c == 1) {
                    r = hf[0];
                    ar = af[0];
                } else {
                    i = 0;
                    if (c > 0) {
                        fp = hf;
                        afp = af;
                        do {
                            if (i == 0) {
                                h[0] = *fp;
                                a[0] = *afp;
                            } else if (h[0] < *fp) {
                                h[0] = *fp;
                                a[0] = *afp;
                            }
                            fp++;
                            i++;
                            afp++;
                        } while (i < c);
                    }
                    r = h[0];
                    ar = a[0];
                }
            } else {
                i = 0;
                if (m > 0) {
                    hp = h;
                    ap = a;
                    do {
                        if (i == 0) {
                            hf[0] = *hp;
                            af[0] = *ap;
                        } else if (!(hf[0] <= *hp)) {
                            hf[0] = *hp;
                            af[0] = *ap;
                        }
                        hp++;
                        i++;
                        ap++;
                    } while (i < m);
                }
                r = hf[0];
                ar = af[0];
            }
        } else {
            r = h[0];
            ar = a[0];
        }
        *out = r;
        at->kind = ar.kind;
        at->b1 = ar.b1;
        at->h2 = ar.h2;
        inw = 0;
        if (w != 0 && *out < *flag) {
            inw = 1;
            *at = wt;
        }
        if (EF(ent, u8, 0x7E9) != 0) *at = sp;
        EF(ent, s8, 0x7E8) = inw;
        if (*out == -0.0f) *out = 0.0f;
        return 1;
    }
    *out = ((STAGE_H *)*(void **)(stage_work + 0x12))->floor2;
    *flag = 0.0f;
    at->kind = 0;
    at->b1 = 0;
    at->h2 = 0;
    return 0;
}
