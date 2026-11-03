/* Monster Hunter 2: cmd_target_kind_set, target_kind_set (game 0x704120, 0x704B90). MH1's C from em_core_g.c with MH2's changes, checked against our own
   disassembly. Kept apart because their jump tables need a data slot, which a part of em_core_g cannot have. em_core_g.c includes this for the PC.
   These fields are u16 in MH2 and not in include/mh2_emw.h yet, so they come through EMT: x828 0x77A, x829 0x77C, x882 0x998, x883 0x99A, x92F 0x986, x95A 0x98C. */
#ifdef EM_CORE_G_IN
#define HEAD(n) void n(EMW *e0, f32 *pos) { EMT *t = (EMT *)e0;
#else
#define cmd_target_kind_set cmd_target_kind_set_
#define target_kind_set target_kind_set_
#include "em_core_g.h"
#undef cmd_target_kind_set
#undef target_kind_set
#define HEAD(n) void n(union EMT *t, f32 *pos) {
#endif

typedef union EMT {
    EMW e;
    struct { char a[0x77A]; u16 x828, x829; } m;
    struct { char a[0x986]; u16 x92F; } n;
    struct { char a[0x998]; u16 x882, x883; } o;
    struct { char a[0x98C]; u16 x95A; } q;
} EMT;
#define em (&t->e)
#define E828(e) (t->m.x828)
#define E829(e) (t->m.x829)
#define E882(e) (t->o.x882)
#define E883(e) (t->o.x883)

extern EM_STG_POS D_008C1100;
extern EMW em_work[];

/* Target position for a commanded monster (x827 = target kind, x828/x829
 * its arguments). */
HEAD(cmd_target_kind_set)
    EM_POSP p;
    STAGE_DATA *sd;
    EM_STG_POS *g;
    EM_ROUTE *r;
    EM_ROUTE_PT *pt;
    f32 *q;
    s8 i;
    EM_SMELL *sm;
    u8 found;

    em->x881 = em->x827;
    E882(em) = E828(em);
    E883(em) = E829(em);
    switch (em->x827) {
    case 1:
        switch (E882(em)) {
        case 0:
        if (E883(em) == -1) {
            em->x617 = -1;
            em->x3B0 = 0;
        } else {
        if (em->x888 == 1 && em->x844 != -1) {
            E829(em) = em->x844 & 0xF;
            E883(em) = E829(em);
        }
        em->x617 = E829(em);
        em->x3B0 = &player_work[em->x617];
        em_pl_pos_set(em, em->x617, pos);
        }
        }
        break;
    case 2:
        if (E828(em) == 7) {
            p = gp_ptr_ck(em, &D_008C1100);
        } else {
            p = gp_ptr_ck(em, (&em->area->x0)[E828(em)]);
        }
        switch (E828(em)) {
        case 0:
        def0:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = em->pos[1];
                pos[2] = 5000.0f;
            } else {
                pos[0] = p[0][0];
                pos[1] = p[0][1];
                pos[2] = p[0][2];
            }
            break;
        case 1:
        case 7:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = 0.0f;
                pos[2] = 5000.0f;
            } else {
                q = p[E829(em)];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            E883(em) = E829(em);
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (E829(em)) {
            case 0:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            case 1:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 2:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 3:
            default:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            }
            } else {
                q = p[E829(em)];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 3:
            if (p == 0) {
                E828(em) = 0;
                E882(em) = E828(em);
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[E883(em) = E829(em)];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 4:
            if (p == 0) {
                E828(em) = 0;
                E882(em) = E828(em);
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[E829(em)];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 5:
            if (p == 0) {
                E828(em) = 0;
                E882(em) = E828(em);
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[E829(em)];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        em->x73A = E829(em);
        NextStage_No_Set();
        E828(em) = t->n.x92F;
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[E883(em)][0];
        pos[1] = p[E883(em)][1];
        pos[2] = p[E883(em)][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (E882(em)) {
        case 0:
            sc = 1.0f;
            ang = 0;
            break;
        case 1:
            sc = 1.0f;
            ang = 0xC000;
            break;
        case 2:
            sc = 1.0f;
            ang = 0x4000;
            break;
        case 3:
            sc = 1.0f;
            ang = 0x8000;
            break;
        case 8:
            sc = 2.0f;
            ang = 0;
            break;
        case 9:
            sc = 2.0f;
            ang = 0xC000;
            break;
        case 10:
            sc = 2.0f;
            ang = 0x4000;
            break;
        case 11:
            sc = 2.0f;
            ang = 0x8000;
            break;
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 500.0f * sc;
        rot[0] = 0;
        rot[1] = (u16)(em->ang[1] + ang);
        rot[2] = 0;
        cpRotMatrix(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = em->pos[0] + out[0];
        pos[1] = em->pos[1] + out[1];
        pos[2] = em->pos[2] + out[2];
        break;
    }
    case 7:
        found = 0;
        if (smell_cnt != 0) {
            for (i = 0; i < 32; i++) {
                sm = smell_stack[i];
                if (sm != 0 && sm->x10 == E882(em) && sm->x11 == E883(em)) {
                    found = 1;
                    pos[0] = sm->pos[0];
                    pos[1] = sm->pos[1];
                    pos[2] = sm->pos[2];
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        p = gp_ptr_ck(em, em->area->x0);
        goto def0;
    case 8:
        pos[0] = em->x9C8[0];
        pos[1] = em->x9C8[1];
        pos[2] = em->x9C8[2];
        break;
    case 9:
        g = gp_ck(em, em->area->x18, em->stg);
        if (g == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        r = (EM_ROUTE *)g->pos;
        if (r == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pt = r[E882(em)].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pos[0] = pt[E883(em)].pos[0];
        pos[1] = pt[E883(em)].pos[1];
        pos[2] = pt[E883(em)].pos[2];
        em->x86D = E882(em);
        em->x86E = E883(em);
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = E882(em);
        em->x3B0 = &player_work[em->x617];
        {
            u16 n = em->x3B0->x70E;

            base = gp_ptr_ck(em, em->area->x0)[n];
        }
        {
            u16 a = Em_Calc_angY(base, em->pos);

            rot[0] = 0;
            rot[1] = a;
            rot[2] = 0;
        }
        switch (E883(em)) {
        case 0:
            v[0] = 0.0f;
            v[1] = 500.0f;
            v[2] = 1000.0f;
            break;
        case 1:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 800.0f;
            break;
        case 2:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 1500.0f;
            break;
        case 3:
            v[0] = 0.0f;
            v[1] = 600.0f;
            v[2] = 600.0f;
            break;
        }
        cpRotMatrixYXZ2(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = base[0] + out[0];
        pos[1] = base[1] + out[1];
        pos[2] = base[2] + out[2];
        break;
    }
    case 12:
        break;
    case 13:
        switch (E882(em)) {
        case 0:
            if (E883(em) == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                if (em->x888 == 1 && em->x844 != -1) {
                    E829(em) = em->x844 % 20;
                    E883(em) = E829(em);
                }
                em->x617 = E829(em);
                em->x3B0 = (void *)&em_work[em->x617];
                em_pl_pos_set(em, em->x617, pos);
            }
        }
        break;
    }
    em->x881 = em->x827;
    E882(em) = E828(em);
    E883(em) = E829(em);
}

HEAD(target_kind_set)
    EM_POSP p;
    STAGE_DATA *sd;
    EM_STG_POS *g;
    EM_ROUTE *r;
    EM_ROUTE_PT *pt;
    f32 *q;
    s8 i;
    EM_SMELL *sm;
    u8 found;

    em->x827 = em->x881;
    E828(em) = E882(em);
    E829(em) = E883(em);
    switch (em->x881) {
    case 1:
        switch (E882(em)) {
        case 0:
            if (E883(em) == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                em->x617 = E883(em);
                em->x3B0 = &player_work[E883(em)];
                em_pl_pos_set(em, em->x617, pos);
                em->x844 = E883(em);
            }
        }
        break;
    case 2:
        if (E882(em) == 7) {
            p = gp_ptr_ck(em, &D_008C1100);
        } else {
            p = gp_ptr_ck(em, (&em->area->x0)[E882(em)]);
        }
        switch (E882(em)) {
        case 0:
        def0:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = em->pos[1];
                pos[2] = 5000.0f;
            } else {
                pos[0] = p[0][0];
                pos[1] = p[0][1];
                pos[2] = p[0][2];
            }
            break;
        case 1:
        case 7:
            if (p == 0) {
                pos[0] = 5000.0f;
                pos[1] = 0.0f;
                pos[2] = 5000.0f;
            } else {
                q = p[E883(em)];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (E883(em)) {
            case 0:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            case 1:
                pos[0] = 0.2f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 2:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.8f * sd->d;
                break;
            case 3:
            default:
                pos[0] = 0.8f * sd->w;
                pos[1] = 1000.0f + sd->floor_y;
                pos[2] = 0.2f * sd->d;
                break;
            }
            } else {
                q = p[E883(em)];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 3:
        case 4:
        case 5:
            if (p == 0) {
                goto fb;
            }
            q = p[E883(em)];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        t->n.x92F = E882(em);
        em->x73A = E883(em);
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[E883(em)][0];
        pos[1] = p[E883(em)][1];
        pos[2] = p[E883(em)][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (E882(em)) {
        case 0:
            sc = 1.0f;
            ang = 0;
            break;
        case 1:
            sc = 1.0f;
            ang = 0xC000;
            break;
        case 2:
            sc = 1.0f;
            ang = 0x4000;
            break;
        case 3:
            sc = 1.0f;
            ang = 0x8000;
            break;
        case 8:
            sc = 2.0f;
            ang = 0;
            break;
        case 9:
            sc = 2.0f;
            ang = 0xC000;
            break;
        case 10:
            sc = 2.0f;
            ang = 0x4000;
            break;
        case 11:
            sc = 2.0f;
            ang = 0x8000;
            break;
        }
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 500.0f * sc;
        rot[0] = 0;
        rot[1] = (u16)(em->ang[1] + ang);
        rot[2] = 0;
        cpRotMatrix(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = em->pos[0] + out[0];
        pos[1] = em->pos[1] + out[1];
        pos[2] = em->pos[2] + out[2];
        break;
    }
    case 7:
        found = 0;
        if (smell_cnt != 0) {
            em->x951 = E882(em);
            em->x952 = E883(em);
            for (i = 0; i < 32; i++) {
                sm = smell_stack[i];
                if (sm != 0 && sm->x10 == E882(em) && sm->x11 == E883(em)) {
                    found = 1;
                    pos[0] = sm->pos[0];
                    pos[1] = sm->pos[1];
                    pos[2] = sm->pos[2];
                    break;
                }
            }
            if (found) {
                break;
            }
        }
    fb:
        p = gp_ptr_ck(em, em->area->x0);
        goto def0;
    case 8:
        pos[0] = em->x9C8[0];
        pos[1] = em->x9C8[1];
        pos[2] = em->x9C8[2];
        break;
    case 9:
        g = gp_ck(em, em->area->x18, em->stg);
        if (g == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        r = (EM_ROUTE *)g->pos;
        if (r == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        if (E882(em) >= g->num) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pt = r[E882(em)].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        if (E883(em) >= r->n) {
            E883(em) = r->n - 1;
        }
        pos[0] = pt[E883(em)].pos[0];
        pos[1] = pt[E883(em)].pos[1];
        pos[2] = pt[E883(em)].pos[2];
        em->x86D = E882(em);
        em->x86E = E883(em);
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = E882(em);
        em->x3B0 = &player_work[em->x617];
        {
            u16 n = em->x3B0->x70E;

            base = gp_ptr_ck(em, em->area->x0)[n];
        }
        {
            u16 a = Em_Calc_angY(base, em->pos);

            rot[0] = 0;
            rot[1] = a;
            rot[2] = 0;
        }
        switch (E883(em)) {
        case 0:
            v[0] = 0.0f;
            v[1] = 500.0f;
            v[2] = 1000.0f;
            break;
        case 1:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 800.0f;
            break;
        case 2:
            v[0] = 0.0f;
            v[1] = 400.0f;
            v[2] = 1500.0f;
            break;
        case 3:
            v[0] = 0.0f;
            v[1] = 600.0f;
            v[2] = 600.0f;
            break;
        }
        cpRotMatrixYXZ2(rot, &mat);
        flvecApplyMat33(out, v, &mat);
        pos[0] = base[0] + out[0];
        pos[1] = base[1] + out[1];
        pos[2] = base[2] + out[2];
        break;
    }
    case 12:
        if (E882(em) == 0) {
            t->q.x95A = E883(em) << 2;
        }
        break;
    case 13:
        switch (E882(em)) {
        case 0:
            if (E883(em) == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                em->x617 = E883(em);
                em->x3B0 = (void *)&em_work[E883(em)];
                em_pl_pos_set(em, em->x617, pos);
                em->x844 = E883(em);
            }
        }
        break;
    }
    em->x827 = em->x881;
    E828(em) = E882(em);
    E829(em) = E883(em);
}

#undef em
