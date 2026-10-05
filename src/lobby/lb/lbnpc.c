/* lbnpc, run 1: Lb_npc_move_sub .. npcMvSHOP (lobby.bin 0x0059DB40-0x0059EEB4): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

void Lb_npc_move_sub(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    s16 i;
    LB_ROUTE *r;
    s32 wait;
    s32 act;

    em->ex[0x2D] = 0;
    if (em->kind == 0) {
        npc_move_func_190[em->type]();
    } else {
        npc_move_func2_191[em->type]();
    }
    if (mv->route != 0) {
        switch (em->x15) {
        case 2:
        case 1:
        case 0x77:
        case 0x64:
        case 0x74:
        case 0x90:
        case 0x8D:
        case 0x78:
        case 0x7E:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x86:
        case 0x87:
        case 0x8B:
        case 0x8C:
        case 0x91:
        case 0x68:
            break;
        default:
        i = mv->idx;
        r = &mv->route[i];
        wait = r->wait;
        if (wait == 0xFF) {
            act = r->act;
            if (act != em->x15) {
                Lb_act_set(em, 0, act & 0xFFFF, i);
            }
        } else {
            if (++mv->cnt >= wait) {
                mv->idx++;
                r = &mv->route[mv->idx];
                if (r->act == -1) {
                    mv->idx = 0;
                    r = &mv->route[mv->idx];
                }
                act = r->act;
                if (act != em->x15) {
                    Lb_act_set(em, 0, act & 0xFFFF, i);
                }
                mv->cnt = 0;
            }
        }
        }
    }
}

void npcMvFOOTWORK(em, kind)
EMW *em;
s8 kind;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_Pl_basic_flagset(em, 0, 0, 0);
        if (em->kind == 1) {
            Lb_pl_chr_set(em, 0x3E9, 0x10, 0);
            return;
        }
        switch (kind) {
        case 0:
            Lb_pl_chr_set(em, 1, 0x10, 0);
            return;
        case 1:
            Lb_pl_chr_set(em, 0x2A3, 0x10, 0);
            return;
        case 3:
            Lb_pl_chr_set(em, 0x3F, 0x10, 0);
            return;
        }
        break;
    case 1:
    case 2:
        if (kind == 3 && em->char0 == 1 && em->x194 < 2) {
            Lb_pl_chr_set(em, 0x3F, 2, 0);
        }
        break;
    }
}

void npcMvWALK(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    LB_ROUTE *r;
    f32 d;

    switch (em->x05) {
    case 0:
        Lb_Pl_basic_flagset(em, 0, 0, 0);
        em->x05++;
        switch (kind) {
        case 0:
            if (em->char0 != 2) {
                Lb_pl_chr_set(em, 2, 4, 0);
                return;
            }
            break;
        case 1:
            if (em->char0 != 0x2A4) {
                Lb_pl_chr_set(em, 0x2A4, 4, 0);
                return;
            }
            break;
        case 2:
            if (em->char0 != 0x3F5) {
                Lb_pl_chr_set0(em, 0x3F5, 4, 0, 0);
                return;
            }
            break;
        case 3:
            if (em->char0 != 0x40) {
                Lb_pl_chr_set(em, 0x40, 4, 0);
                return;
            }
            break;
        }
        break;
    case 1:
        r = &mv->route[mv->idx];
        em->x0E = (s16)(u16)Lb_get_angle(em, r) / 10;
        em->ang[1] += em->x0E;
        d = flvecCalcDistance(em->pos, r);
        if (d < 0.0f) {
            d *= -1.0f;
        }
        if (d <= 50.0f) {
            mv->idx++;
            if (mv->route[mv->idx].wait == -1) {
                mv->idx = 0;
            }
            r = &mv->route[mv->idx];
            if (em->x15 != r->act) {
                em->x05++;
                Lb_act_set(em, 0, (u16)r->act);
                mv->cnt = 0;
                return;
            }
            em->x05 = 0;
        }
        break;
    }
}

void npcMvENJOY(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        mv->f0F = 1;
        em->x05++;
        Lb_Pl_basic_flagset(em, 1, 0, 0);
        if (kind == 1) {
            if (em->char0 != 0x2A0) {
                Lb_pl_chr_set(em, 0x2A0, 0x3C, 0);
                return;
            }
        } else {
            if (em->char0 != 0x29E) {
                Lb_pl_chr_set(em, 0x29E, 0x3C, 0);
            }
        }
    case 1:
        break;
    }
}

void npcMvTALK(em, kind)
EMW *em;
s8 kind;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_Pl_basic_flagset(em, 0, 0, 0);
        switch (kind) {
        case 0:
            Lb_pl_chr_set(em, 0x291, 0xA, 0);
            return;
        case 1:
            Lb_pl_chr_set(em, 0x290, 0xA, 0);
            return;
        case 2:
            Lb_pl_chr_set(em, 0x292, 0xC, 0);
            return;
        }
        break;
    case 1:
        if (lb_sys.x68 == 6) {
            Lb_act_set(em, 0, 0x74);
        }
        break;
    }
}

void npcMvWARP(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    s16 i;
    LB_ROUTE *r;

    if (mv->route == 0) {
        Lb_act_set(em, 0, 0);
    }
    i = mv->idx;
    r = &mv->route[i];
    *(VEC3 *)em->pos = *(VEC3 *)r->pos;
    Lb_act_set(em, 0, 0, i);
}

void npcMvDOWN(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        mv->f0F = 1;
        Lb_Pl_basic_flagset(em, 1, 0, 0);
        if (kind == 1) {
            if (em->char0 != 0x29C) {
                Lb_pl_chr_set(em, 0x29C, 0x3C, 0);
                return;
            }
        } else if (kind == 0) {
            if (em->char0 != 0x29A) {
                Lb_pl_chr_set(em, 0x29A, 0x3C, 0);
                return;
            }
        } else {
            if (em->char0 != 0x268) {
                Lb_pl_chr_set(em, 0x268, 0, 0x7C);
            }
        }
    case 1:
        break;
    }
}

void npcMvTOPL(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    LB_ROUTE *rt;

    switch (em->char0) {
    case 0x2A6:
    case 0x285:
    case 0x268:
        Lb_act_set(em, 0, 0);
        return;
    case 0x431:
        Lb_act_set(em, 0, 0x82);
        return;
    default:
        switch (em->x05) {
        case 0:
            mv->x2D = 1;
            if (mv->f0F != 0) {
                em->work08 = 0;
            } else {
                em->work08 = 0xA;
                em->x0E = Lb_get_angle(em, (u8 *)&player_work[game_w.master] + 0xAC);
            }
            em->x05++;
            if (em->kind == 2) {
                Lb_pl_chr_set0(em, 0x42E, 4, 0, 0);
                return;
            }
            switch (em->char0) {
            case 0x29F:
            case 0x2B8:
            case 0x2A1:
            case 0x29B:
            case 0x29D:
            case 0x297:
            case 0x299:
            case 0x295:
            case 0x3EF:
            case 0x3F0:
            case 0x292:
            case 0x281:
            case 0x286:
            case 0x289:
                break;
            case 0x29E:
                Lb_pl_chr_set(em, 0x29F, 0xA, 0);
                return;
            case 0x2A3:
            case 0x2A4:
                Lb_pl_chr_set(em, 0x2B8, 0x10, 0);
                return;
            case 0x2A0:
                Lb_pl_chr_set(em, 0x2A1, 0xA, 0);
                return;
            case 0x29A:
                Lb_pl_chr_set(em, 0x29B, 0xA, 0);
                return;
            case 0x29C:
                Lb_pl_chr_set(em, 0x29D, 0xA, 0);
                return;
            case 0x296:
                Lb_pl_chr_set(em, 0x297, 0xA, 0);
                return;
            case 0x298:
                Lb_pl_chr_set(em, 0x299, 0xA, 0);
                return;
            case 0x294:
            case 0x293:
                Lb_pl_chr_set(em, 0x295, 0xA, 0);
                return;
            case 0x27E:
                Lb_pl_chr_set(em, 0x27F, 4, 0);
                return;
            case 0x284:
                Lb_pl_chr_set(em, 0x286, 0x10, 0);
                return;
            case 0x263:
                Lb_pl_chr_set(em, 0x261, 0x10, 0);
                return;
            case 0x261:
                Lb_pl_chr_set(em, 0x289, 0, 0);
                return;
            case 0x3F5:
            case 0x3E9:
                Lb_pl_chr_set(em, 0x3EF, 0x10, 0);
                return;
            default:
                if (em->x388 == 0) {
                    Lb_pl_chr_set(em, 0x292, 0xC, 0);
                    return;
                }
            }
            break;
        case 1:
            mv->x2D = 1;
            em->ang[1] += em->x0E / 10;
            if (--em->work08 <= 0) {
                em->x05++;
                return;
            }
            break;
        case 2:
            mv->x2D = 1;
            if (em->char0 == 0x3EF && em->x194 <= 0) {
                Lb_pl_chr_set(em, 0x3F0, 8, 0);
            } else if (em->char0 == 0x261 && frame_check2(em, 5.0f, 0) != 0) {
                Lb_pl_chr_set(em, 0x289, 0, 0);
            }
            if (lb_sys.x68 == 0) {
                em->work08 = 0x10;
                em->x05++;
                if (mv->kind == 0x19 || mv->kind == 0x11) {
                    em->work08 = 0xA;
                }
                if (em->kind == 2 && em->char0 == 0x42E) {
                    em->work08 = 0x20;
                    Lb_pl_chr_set(em, 0x3E9, 0x10, 0);
                    return;
                }
            }
            break;
        case 3:
            switch (mv->kind) {
            case 0x11:
            case 0x19:
            case 0x1E:
            case 0x2A:
            case 0x30:
            case 0x33:
                mv->x2D = 1;
                break;
            }
            if (em->char0 == 0x286) {
                if (frame_check2(em, 260.0f, 0) != 0) {
                    Lb_pl_chr_set(em, 0x284, 0x12, 0);
                    return;
                }
            } else {
                if (--em->work08 <= 0) {
                    if (mv->f0F != 0) {
                        em->work08 = 0;
                        switch (em->char0) {
                        case 0x27F:
                            Lb_pl_chr_set(em, 0x281, 0xA, 0);
                            break;
                        }
                    } else {
                        em->work08 = 0xA;
                        if (em->kind == 2) {
                            switch (em->char0) {
                            case 0x42E:
                                Lb_pl_chr_set(em, 0x3E9, 0xA, 0);
                                break;
                            case 0x3ED:
                                em->work08 = 0;
                                Lb_pl_chr_set(em, 0x426, 0xA, 0);
                                break;
                            }
                        } else {
                            switch (em->char0) {
                            case 0x2B8:
                                Lb_pl_chr_set(em, 0x2A3, 0xA, 0);
                                break;
                            case 0x3EF:
                            case 0x3F0:
                                Lb_pl_chr_set(em, 0x3E9, 0xA, 0);
                                break;
                            default:
                                Lb_pl_chr_set(em, 1, 0xA, 0);
                                break;
                            }
                        }
                    }
                    em->x05++;
                    return;
                }
            }
            break;
        case 4:
            switch (mv->kind) {
            case 0x11:
            case 0x19:
            case 0x1E:
            case 0x2A:
            case 0x30:
            case 0x33:
            case 0x2F:
            case 0x47:
            case 0x2D:
                mv->x2D = 1;
                break;
            }
            em->ang[1] -= em->x0E / 10;
            if (--em->work08 <= 0) {
                rt = mv->route;
                if (rt != 0) {
                    Lb_act_set(em, 0, *(u16 *)((u8 *)rt + mv->idx * 0x14 + 0xC));
                    return;
                }
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}

void npcMvBOARD(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        mv->f0F = 1;
        switch (kind) {
        case 0:
            if (em->char0 != 0x293) {
                Lb_pl_chr_set(em, 0x293, 0x14, 0);
                return;
            }
            break;
        case 1:
            if (em->char0 != 0x294) {
                Lb_pl_chr_set(em, 0x294, 0x14, 0);
            }
            break;
        }
        break;
    }
}

void npcMvSHOP(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        mv->f0F = 1;
        switch (kind) {
        case 0:
            if (em->char0 != 0x296) {
                Lb_pl_chr_set(em, 0x296, 0xA, 0);
            }
            break;
        case 1:
            if (em->char0 != 0x298) {
                Lb_pl_chr_set(em, 0x298, 0xA, 0);
            }
            break;
        }
        break;
    case 1:
        break;
    }
}
