/* fly - game.bin 0x005EB4A0-0x005EB774: em19's flight curves. While a
 * flying move plays, the monster's height and forward speed follow
 * keyframed curves (fly_adjy2_hosei_tbl, fly_adjz2_hosei_tbl): pairs of
 * (frame, value), ended by a frame of 0. em19_fly_adjy2 advances the curve
 * by the animation speed and returns which curves have run out
 * (2: height, 4: forward). */
#include "em.h"

/* em19's work in EMW.ex (EMW+0x444), the part fly.c uses. */
typedef struct EM19W {
    u8 _pad00[0x20];
    s32 spd[3];         /* 0x20 passed to speed_add_g (angle in [1]) */
    u8 adj_x;           /* 0x2C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x2D */
    u8 adj_z;           /* 0x2E */
    u8 adj_type;        /* 0x2F table row */
    s16 adj_tm;         /* 0x30 time into the table */
} EM19W;

extern f32 (*fly_adjy2_hosei_tbl_00668130[])[2];
extern f32 (*fly_adjz2_hosei_tbl_00668360[])[2];

void speed_add_g(EMW *em, s32 *spd);

static u8 fly_adjy2_subx(EMW *em, EM19W *f) {
    return 0;
}

static u8 fly_adjy2_suby(EMW *em, EM19W *f) {
    f32 (*tbl)[2] = fly_adjy2_hosei_tbl_00668130[f->adj_type];
    u8 ret = 0;
    f32 fr;
    f32 *p;
    s32 i;

    if (tbl == 0) {
        return ret;
    }
    {
        fr = f->adj_tm;
        if (fr == 0.0f || fr == 1.0f) {
            em->adj_y = tbl[0][1];
        } else {
            i = 1;
            do {
                p = tbl[i];
                if (fr > p[0] && p[0] != 0.0f) {
                    i++;
                } else if (p[0] == 0.0f) {
                    ret = 2;
                    break;
                } else {
                    i = 0;
                    em->adj_y = (p[1] - p[-1]) / ((p[0] - p[-2]) / em->chr_spd0);
                }
            } while (i != 0);
        }
    }

    return ret;
}

static u8 fly_adjy2_subz(EMW *em, EM19W *f) {
    f32 (*tbl)[2] = fly_adjz2_hosei_tbl_00668360[f->adj_type];
    u8 ret = 0;
    f32 fr;
    f32 *p;
    s32 i;

    if (tbl == 0) {
        return ret;
    }
    {
        fr = f->adj_tm;
        if (fr == 0.0f || fr == 1.0f) {
            em->adj_z = tbl[0][1];
        } else {
            i = 1;
            do {
                p = tbl[i];
                if (fr > p[0] && p[0] != 0.0f) {
                    i++;
                } else if (p[0] == 0.0f) {
                    ret = 4;
                    break;
                } else {
                    i = 0;
                    em->adj_z = (p[1] - p[-1]) / ((p[0] - p[-2]) / em->chr_spd0);
                }
            } while (i != 0);
        }
    }

    return ret;
}

u8 em19_fly_adjy2(EMW *em) {
    EM19W *f = (EM19W *)em->ex;
    u8 ret = 0;

    if (((EM19W *)em->ex)->adj_x) {
        ret = fly_adjy2_subx(em, f);
    }
    if (f->adj_y) {
        ret |= fly_adjy2_suby(em, f);
    }
    if (f->adj_z) {
        ret |= fly_adjy2_subz(em, f);
    }
    f->adj_tm += (s16)em->chr_spd0;
    if (em->x388 == 2) {
        f->spd[0] = 0;
        f->spd[1] = em->ang[1];
        f->spd[2] = 0;
        speed_add_g(em, f->spd);
    }
    return ret;
}
