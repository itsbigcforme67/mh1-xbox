/* Lobby effect 25 (character status effects: spark/aura particles that follow a joint)
   (SLPM_654.95 lobby overlay 0x60E330-0x610300). Whole file; runs split into lb_e25NN.c.
   One E25 work slot per effect (pull_eft_work), up to eft25_num[type] E25P particles (0x34 bytes each) hang off it. */
#include "lobby_f.h"

typedef struct E25P {          /* one particle */
    s16 prim;                  /* 0x00 prim handle, -1 = none */
    s16 idx;                   /* 0x02 index inside the effect */
    f32 col[3];                /* 0x04 */
    f32 pos[3];                /* 0x10 */
    f32 alpha[2];              /* 0x1C alpha fade state */
    u16 ang;                   /* 0x24 */
    s16 angspd;                /* 0x26 */
    f32 scale;                 /* 0x28 */
    u8 *pr;                    /* 0x2C prim pointer */
    s16 time;                  /* 0x30 */
    u16 rnd;                   /* 0x32 */
} E25P;

typedef struct E25 {           /* effect work */
    u8 x0;
    u8 on;                     /* 0x01 active */
    u8 id;                     /* 0x02 */
    u8 type;                   /* 0x03 effect type 0..8 */
    u8 state;                  /* 0x04 0 init, 1 move, 2 destroy, 3 end */
    s8 f5;                     /* 0x05 */
    s8 f6;                     /* 0x06 */
    u8 x7;                     /* 0x07 */
    s16 t;                     /* 0x08 frame counter */
    s16 rot;                   /* 0x0A */
    u8 padC[8];
    s32 x14;                   /* 0x14 */
    E25P *part;                /* 0x18 particle array */
    u8 pad1C[4];
    void (*fn)();              /* 0x20 handler */
    f32 pos[3];                /* 0x24 */
    f32 scale;                 /* 0x30 */
    u8 *joint;                 /* 0x34 owner (first byte = alive flag) */
    s32 x38;                   /* 0x38 */
    s16 x3C;                   /* 0x3C prim handle of the root */
} E25;

void *pull_eft_work();
void push_eft_work();
void flvecCopy();
void eft25_move();
void eft25_i();
void eft25_m();
void eft25_d();
void eft25_e();
void release_prim();
extern s16 eft25_num[9];
extern s16 eft25_type0_lag[7];
void eft25_t();
extern s16 eft25_index[9];
extern s16 eft25_param[9];
extern s16 eft25_all_time[9];
extern u16 eft25_time[9];
extern u16 *eft25_time_tbl[9];
extern f32 *eft25_data[];
extern f32 rview_mat[16];
extern char ot0[];
void eft_vec_linear();
void eft_alpha_linear();
void flvecRotY();
f32 flSin();
void flmatCopy();
void *get_joint_wmat();
void flvecApplyMat33_2();

void Eft25_set_pos(f32 scale, int pos, int type, int arg) {
    E25 *e;
    e = pull_eft_work(1);
    if (e != 0) {
        e->id = 0x19;
        e->fn = eft25_move;
        e->x14 = 0;
        e->joint = 0;
        e->type = type;
        e->x7 = arg;
        e->x38 = 0;
        e->scale = scale;
        flvecCopy(e->pos, pos);
    }
}

void eft25_move(E25 *e) {
    u8 st;
    st = e->state;
    switch (st) {
    case 0:
        eft25_i(e);
        return;
    case 1:
        eft25_m(e);
        return;
    case 2:
        eft25_d(e);
        return;
    case 3:
        eft25_e(e);
    }
}

void eft25_d(E25 *e) {
    E25P *p;
    s16 n;
    s16 i;
    s16 *np;
    p = e->part;
    e->state += 1;
    np = &eft25_num[e->type];
    n = *np;
    if (e->x38 != 0) {
        release_prim(e->x3C, np, e->type * 2);
    }
    i = 0;
    if (n > 0) {
        do {
            if (p->pr != 0) {
                release_prim(p->prim);
            }
            i = (s16)(i + 1);
            p += 1;
        } while (i < n);
    }
}

void eft25_e(void) {
    push_eft_work();
}

/* place the effect root on the joint with a local offset */
#define E25_ROOT(e, joint_no, ox, oy, oz) \
    flmatCopy(m, get_joint_wmat((e)->joint, joint_no, 1)); \
    v[0] = (ox); \
    v[1] = (oy); \
    v[2] = (oz); \
    flvecApplyMat33_2(v, m); \
    (e)->pos[0] = m[12] + v[0]; \
    (e)->pos[1] = m[13] + v[1]; \
    (e)->pos[2] = m[14] + v[2]

void eft25_i(E25 *e) {
    f32 v[3];
    f32 m[16];
    E25P *p;
    s16 *lag;
    s16 i;
    s16 n;
    long nn;
    u8 ty;
    p = e->part;
    e->state += 1;
    e->f5 = 0;
    e->f6 = 0;
    e->on = 1;
    e->x14 = 0;
    e->t = 0;
    ty = e->type;
    n = eft25_num[ty];
    switch (ty) {
    case 0:
        e->rot = (u16)ran_suu(1);
    case 1:
        flmatCopy(m, get_joint_wmat(e->joint, 0xA));
        v[0] = 0.0f;
        v[1] = 50.0f;
        v[2] = 25.0f;
        flvecApplyMat33_2(v, m);
        e->pos[0] = m[12] + v[0];
        e->pos[1] = m[13] + v[1];
        e->pos[2] = m[14] + v[2];
        break;
    case 2:
        if (e->joint != 0) {
            E25_ROOT(e, 0xB, -26.4f, 2.8f, 7.7f);
        }
        break;
    case 5:
        E25_ROOT(e, 0x14, 0.0f, 40.0f, 0.0f);
        e->f5 = (u16)ran_suu(1) & 3;
        e->f6 = (u16)ran_suu(1) & 3;
        break;
    case 6:
        E25_ROOT(e, 0x12, -8.0f, 0.0f, 70.0f);
        break;
    case 8:
        E25_ROOT(e, 0x14, 0.0f, 40.0f, 0.0f);
        break;
    }
    nn = n;
    i = 0;
    if (nn > 0) {
        lag = eft25_type0_lag;
        do {
            p->prim = get_prim();
            if (p->prim != -1) {
                p->time = 0;
                p->scale = e->scale;
                flvecCopy(p->pos, e->pos);
                p->idx = i;
                switch (e->type) {
                case 0:
                    p->time = *lag - 10;
                    switch (p->idx) {
                    case 0:
                        p->ang = 0;
                        p->angspd = 0;
                        break;
                    case 1:
                        p->ang = 0;
                        p->angspd = 0;
                        p->scale = p->scale * 3.0f;
                        break;
                    case 2:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                        break;
                    case 3:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0xFF) - 0x80;
                        p->scale = p->scale * 3.0f;
                        break;
                    case 4:
                    case 5:
                    case 6:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                        p->scale = p->scale * (0.5f + 0.0003f * (f32)((u16)ran_suu(1) & 0x3FF));
                        break;
                    }
                    break;
                case 1:
                    p->ang = 0;
                    p->angspd = ((u16)ran_suu(1) & 0x7FF) - 0x400;
                    p->scale = p->scale * (0.8f + 0.00040000002f * (f32)((u16)ran_suu(1) & 0x3FF));
                    p->rnd = (u16)ran_suu(1);
                    break;
                case 2:
                    p->ang = (u16)ran_suu(1);
                    p->angspd = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                    p->rnd = (u16)ran_suu(1);
                    break;
                case 3:
                    switch (p->idx) {
                    case 0:
                        p->ang = 0x2000;
                        break;
                    default:
                    case 1:
                        p->ang = 0;
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 4:
                    switch (p->idx) {
                    case 1:
                    case 0:
                        p->ang = 0;
                        break;
                    default:
                    case 3:
                    case 2:
                        p->ang = (u16)ran_suu(1);
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 5:
                    p->ang = 0;
                    p->angspd = 0;
                    break;
                case 6:
                    switch (p->idx) {
                    case 3:
                        p->ang = 0x4000;
                        break;
                    default:
                    case 1:
                        p->ang = 0;
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 7:
                    p->time = p->idx * -5 - 10;
                    p->ang = 0x2000;
                    p->angspd = 0;
                    break;
                case 8:
                    p->time = 0;
                    p->ang = 0;
                    p->angspd = 0;
                    p->col[0] = 0.3f;
                    p->col[1] = 0.3f;
                    p->col[2] = 0.3f;
                    break;
                }
                p->pr = get_prim_ptr(p->prim);
                *(E25 **)(p->pr + 0x18) = e;
                *(s32 *)(p->pr + 0x1C) = p->idx;
                *(void **)(p->pr + 0x14) = eft25_t;
            } else {
                p->pr = 0;
            }
            p += 1;
            i = (s16)(i + 1);
            lag += 1;
        } while (i < nn);
    }
}

#define E25_RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))

void eft25_m(E25 *e) {
    f32 v[3];
    f32 w[3];
    f32 m[16];
    u16 *tbl;
    u16 *tp;
    E25P *p;
    s16 n;
    s16 idx;
    s16 par;
    s16 t;
    u16 tm;
    long nn;
    long k;
    int rot;
    int s;
    int o;
    u8 ty;
    ty = e->type;
    p = e->part;
    o = ty * 2;
    t = e->t + 1;
    n = eft25_num[ty];
    idx = eft25_index[ty];
    par = eft25_param[ty];
    tm = eft25_time[ty];
    tbl = eft25_time_tbl[ty];
    e->t = t;
    if (t >= (u16)eft25_all_time[ty]) {
        e->state += 1;
        e->on = 0;
        return;
    }
    switch (e->type) {
    case 6:
        if (*e->joint == 0) {
            e->state += 1;
            e->on = 0;
            return;
        }
        flmatCopy(m, get_joint_wmat(e->joint, 0x12));
        v[0] = -8.0f;
        v[1] = 0.0f;
        v[2] = 70.0f;
        flvecApplyMat33_2(v, m);
        e->pos[0] = m[12] + v[0];
        e->pos[1] = m[13] + v[1];
        e->pos[2] = m[14] + v[2];
        break;
    case 7:
        if (*e->joint == 0) {
            e->state += 1;
            e->on = 0;
            return;
        }
        break;
    case 8:
        if (*e->joint == 0) {
            e->state += 1;
            e->on = 0;
            return;
        }
        flmatCopy(m, get_joint_wmat(e->joint, 0x14));
        v[0] = 0.0f;
        v[1] = 40.0f;
        v[2] = 0.0f;
        flvecApplyMat33_2(v, m);
        e->pos[0] = m[12] + v[0];
        e->pos[1] = m[13] + v[1];
        e->pos[2] = m[14] + v[2];
        break;
    }
    nn = n;
    k = 0;
    if (nn > 0) {
        tp = tbl;
        rot = 0;
        do {
            if (tbl != 0) {
                tm = *tp;
            } else {
                idx = eft25_index[e->type];
            }
            p->time += 1;
            if (p->time <= 0) {
                idx = idx + par;
            } else {
                if (p->time == 1) {
                    if (e->type == 7) {
                        flmatCopy(m, get_joint_wmat(e->joint, 0x12));
                        w[0] = -8.0f + (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                        w[1] = (f32)(((u16)ran_suu(1) & 0xF) - 8);
                        w[2] = 70.0f + (f32)(((u16)ran_suu(1) & 0x3F) - 0x20);
                        flvecApplyMat33_2(w, m);
                        p->pos[0] = m[12] + w[0];
                        p->pos[1] = m[13] + w[1];
                        p->pos[2] = m[14] + w[2];
                    }
                    goto step;
                }
                if (tm < p->time) {
                    idx = idx + par;
                } else {
step:
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 0.0f;
                    switch (e->type) {
                    case 0:
                        s = idx;
                        eft_vec_linear((f32)p->time, eft25_data[s], p->col);
                        eft_vec_linear((f32)p->time, eft25_data[(s16)(s + 1)], w);
                        if (p->idx != 6 && p->idx != 5 && p->idx != 4) {
                            flvecRotY(E25_RAD(e->rot), w);
                        } else {
                            flvecRotY(E25_RAD(e->rot + rot), w);
                        }
                        idx = (s16)(idx + 3);
                        v[0] += w[0];
                        v[1] += w[1];
                        v[2] += w[2];
                        eft_alpha_linear((f32)p->time, eft25_data[(s16)(s + 2)], p->alpha);
                        break;
                    case 1:
                        s = idx;
                        eft_vec_linear((f32)p->time, eft25_data[s], p->col);
                        idx = (s16)(idx + 2);
                        eft_vec_linear((f32)p->time, eft25_data[(s16)(s + 1)], w);
                        flvecRotY(E25_RAD(e->rot), w);
                        v[0] = (f32)p->time * (0.030000001f * (f32)((p->rnd & 0x3FF) - 0x200)) / (f32)tm;
                        v[1] = 0.0f;
                        v[2] = 0.0f;
                        flvecApplyMat33_2(v, rview_mat);
                        v[0] += w[0];
                        v[1] += w[1];
                        v[2] += w[2];
                        break;
                    case 2:
                        s = idx;
                        eft_vec_linear((f32)p->time, eft25_data[s], p->col);
                        eft_vec_linear((f32)p->time, eft25_data[(s16)(s + 1)], w);
                        flvecRotY(E25_RAD(e->rot), w);
                        idx = (s16)(idx + 3);
                        eft_alpha_linear((f32)p->time, eft25_data[(s16)(s + 2)], p->alpha);
                        v[0] = (f32)p->time * (0.010000001f * (f32)((p->rnd & 0x3FF) - 0x200)) / (f32)tm;
                        v[1] = 0.0f;
                        v[2] = 0.0f;
                        flvecApplyMat33_2(v, rview_mat);
                        v[0] += w[0];
                        v[1] += w[1];
                        v[2] += w[2];
                        v[0] *= e->scale;
                        v[1] *= e->scale;
                        v[2] *= e->scale;
                        break;
                    case 3:
                    case 7:
                        eft_vec_linear((f32)p->time, eft25_data[idx], p->col);
                        idx = (s16)(idx + 1);
                        v[2] = 5.0f * (f32)k;
                        flvecApplyMat33_2(v, rview_mat);
                        break;
                    case 6:
                        flvecCopy(p->pos, e->pos);
                    case 4:
                        s = idx;
                        eft_vec_linear((f32)p->time, eft25_data[s], p->col);
                        idx = (s16)(idx + 2);
                        eft_alpha_linear((f32)p->time, eft25_data[(s16)(s + 1)], p->alpha);
                        v[2] = 5.0f * (f32)k;
                        flvecApplyMat33_2(v, rview_mat);
                        break;
                    case 5:
                        eft_vec_linear((f32)p->time, eft25_data[idx], p->col);
                        idx = (s16)(idx + 1);
                        v[0] = (f32)((p->rnd & 0xF) + 0xF) * (flSin(E25_RAD(e->t << 11)) - 0.5f);
                        v[1] = 0.0f;
                        v[2] = 0.0f;
                        p->pos[1] = p->pos[1] + 1.25f;
                        flvecApplyMat33_2(v, rview_mat);
                        break;
                    case 8:
                        flvecCopy(p->pos, e->pos);
                        break;
                    }
                    p->ang = p->ang + p->angspd;
                    if (p->pr != 0) {
                        *(f32 *)(p->pr + 8) = p->pos[0] + v[0];
                        *(f32 *)(p->pr + 0xC) = p->pos[1] + v[1];
                        *(f32 *)(p->pr + 0x10) = p->pos[2] + v[2];
                        add_prim(ot0, p->pr, 0x40, 0);
                    }
                }
            }
            tp += 2;
            rot += 0x5555;
            k = (s16)(k + 1);
            p += 1;
        } while (k < nn);
    }
}
