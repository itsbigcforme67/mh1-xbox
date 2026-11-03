/* Monster Hunter 2: cmd_target_kind_set, target_kind_set (game 0x704120, 0x704B90). MH1's C from em_core_g.c with MH2's changes, checked against our own
   disassembly. Kept apart because their jump tables need a data slot, which a part of em_core_g cannot have. em_core_g.c includes this for the PC.
   MH2's u16 fields x828 0x77A, x829 0x77C, x882 0x998, x883 0x99A, x92F 0x986 and x95A 0x98C come from include/mh2_emw.h (they went through a
   temporary union until STRUCTS fixed them, merge of 2-3 Nov 2026). */
#ifndef EM_CORE_G_IN
#define cmd_target_kind_set cmd_target_kind_set_
#define target_kind_set target_kind_set_
#include "em_core_g.h"
#undef cmd_target_kind_set
#undef target_kind_set
#endif
#define HEAD(n) void n(EMW *em, f32 *pos) {

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
    em->x882 = em->x828;
    em->x883 = em->x829;
    switch (em->x827) {
    case 1:
        switch (em->x882) {
        case 0:
        if (em->x883 == -1) {
            em->x617 = -1;
            em->x3B0 = 0;
        } else {
        if (em->x888 == 1 && em->x844 != -1) {
            em->x829 = em->x844 & 0xF;
            em->x883 = em->x829;
        }
        em->x617 = em->x829;
        em->x3B0 = &player_work[em->x617];
        em_pl_pos_set(em, em->x617, pos);
        }
        }
        break;
    case 2:
        if (em->x828 == 7) {
            p = gp_ptr_ck(em, &D_008C1100);
        } else {
            p = gp_ptr_ck(em, (&em->area->x0)[em->x828]);
        }
        switch (em->x828) {
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
                q = p[em->x829];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            em->x883 = em->x829;
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (em->x829) {
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
                q = p[em->x829];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 3:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x883 = em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 4:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        case 5:
            if (p == 0) {
                em->x828 = 0;
                em->x882 = em->x828;
                p = gp_ptr_ck(em, em->area->x0);
                goto def0;
            }
            q = p[em->x829];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        em->x73A = em->x829;
        NextStage_No_Set();
        em->x828 = em->x92F;
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[em->x883][0];
        pos[1] = p[em->x883][1];
        pos[2] = p[em->x883][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (em->x882) {
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
                if (sm != 0 && sm->x10 == em->x882 && sm->x11 == em->x883) {
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
        pt = r[em->x882].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pos[0] = pt[em->x883].pos[0];
        pos[1] = pt[em->x883].pos[1];
        pos[2] = pt[em->x883].pos[2];
        em->x86D = em->x882;
        em->x86E = em->x883;
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = em->x882;
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
        switch (em->x883) {
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
        switch (em->x882) {
        case 0:
            if (em->x883 == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                if (em->x888 == 1 && em->x844 != -1) {
                    em->x829 = em->x844 % 20;
                    em->x883 = em->x829;
                }
                em->x617 = em->x829;
                em->x3B0 = (void *)&em_work[em->x617];
                em_pl_pos_set(em, em->x617, pos);
            }
        }
        break;
    }
    em->x881 = em->x827;
    em->x882 = em->x828;
    em->x883 = em->x829;
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
    em->x828 = em->x882;
    em->x829 = em->x883;
    switch (em->x881) {
    case 1:
        switch (em->x882) {
        case 0:
            if (em->x883 == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                em->x617 = em->x883;
                em->x3B0 = &player_work[em->x883];
                em_pl_pos_set(em, em->x617, pos);
                em->x844 = em->x883;
            }
        }
        break;
    case 2:
        if (em->x882 == 7) {
            p = gp_ptr_ck(em, &D_008C1100);
        } else {
            p = gp_ptr_ck(em, (&em->area->x0)[em->x882]);
        }
        switch (em->x882) {
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
                q = p[em->x883];
                pos[0] = q[0];
                pos[1] = q[1];
                pos[2] = q[2];
            }
            break;
        case 2:
            if (p == 0) {
            sd = Stage_data_get(em->stg);
            switch (em->x883) {
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
                q = p[em->x883];
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
            q = p[em->x883];
            pos[0] = q[0];
            pos[1] = q[1];
            pos[2] = q[2];
            break;
        }
        break;
    case 3:
        em->x92F = em->x882;
        em->x73A = em->x883;
        break;
    case 5:
        p = em_cmd_pos_tbl[em->kind];
        pos[0] = p[em->x883][0];
        pos[1] = p[em->x883][1];
        pos[2] = p[em->x883][2];
        break;
    case 6: {
        u16 ang;
        f32 sc;
        f32 out[3];
        f32 v[3];
        s32 rot[3];
        FLMAT mat;

        switch (em->x882) {
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
            em->x951 = em->x882;
            em->x952 = em->x883;
            for (i = 0; i < 32; i++) {
                sm = smell_stack[i];
                if (sm != 0 && sm->x10 == em->x882 && sm->x11 == em->x883) {
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
        if (em->x882 >= g->num) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        pt = r[em->x882].pt;
        if (pt == 0) {
            p = gp_ptr_ck(em, em->area->x0);
            goto def0;
        }
        if (em->x883 >= r->n) {
            em->x883 = r->n - 1;
        }
        pos[0] = pt[em->x883].pos[0];
        pos[1] = pt[em->x883].pos[1];
        pos[2] = pt[em->x883].pos[2];
        em->x86D = em->x882;
        em->x86E = em->x883;
        break;
    case 11: {
        f32 *base;
        s32 rot[3];
        f32 v[3];
        f32 out[3];
        FLMAT mat;

        em->x617 = em->x882;
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
        switch (em->x883) {
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
        if (em->x882 == 0) {
            em->x95A = em->x883 << 2;
        }
        break;
    case 13:
        switch (em->x882) {
        case 0:
            if (em->x883 == -1) {
                em->x617 = -1;
                em->x3B0 = 0;
            } else {
                em->x617 = em->x883;
                em->x3B0 = (void *)&em_work[em->x883];
                em_pl_pos_set(em, em->x617, pos);
                em->x844 = em->x883;
            }
        }
        break;
    }
    em->x827 = em->x881;
    em->x828 = em->x882;
    em->x829 = em->x883;
}

#undef HEAD
