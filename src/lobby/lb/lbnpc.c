/* lbnpc, run 1: Lb_npc_move_sub .. lb_npc_old_mix (lobby.bin 0x0059DB40-0x005A06EC): the matching functions of lbnpc_nm.c. */
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

void npcMvBEER(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((ran_suu(1) & 0xFFFF) + 0xB4) & 0xFF;
        mv->f0F = 1;
        Lb_Pl_basic_flagset(em, 1, 0, 0);
        if (em->char0 == 0x289) {
            Lb_pl_chr_set(em, 0x261, 0xA, 0);
            return;
        }
        if (em->char0 != 0x261) {
            Lb_pl_chr_set(em, 0x261, 0, 0);
            return;
        }
        break;
    case 1:
        if (game_w.stage == 0x4D) {
            if (--em->work08 <= 0) {
                em->x05++;
                Lb_pl_chr_set(em, 0x263, 6, 0);
                return;
            }
        } else {
            if (--em->work08 <= 0) {
                em->x05++;
                if (em->char0 != 0x289) {
                    Lb_pl_chr_set(em, 0x289, 0, 0);
                    return;
                }
            }
        }
        break;
    case 2:
        if (game_w.stage == 0x4D) {
            if (em->x194 <= 0) {
                em->x05++;
                Lb_pl_chr_set(em, 0x261, 4, 0);
                return;
            }
        } else if (em->x194 <= 0) {
            if (!((u16)ran_suu(1) & 1)) {
                em->work08 = 0;
                em->x05--;
                return;
            }
            em->x05++;
            Lb_pl_chr_set(em, 0x261, 4, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 <= 0) {
            em->x05 = 0;
        }
        break;
    }
}

void npcMvDRUNKDOWN(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x2A6) {
            Lb_pl_chr_set(em, 0x2A6, 0, 0);
        }
        break;
    }
}

void npcMvTOPL2(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->work08 = 8;
        em->x05++;
        break;
    case 1:
        if (--em->work08 <= 0) {
            Lb_act_set(em, 0, 0x64);
        }
        break;
    }
}

void npcMvRANDWAIT(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((ran_suu(1) & 0xFFFF) + 0xFF) & 0xFF;
        Lb_pl_chr_set(em, 1, 0x14, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void npcMvWALL(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        mv->f0F = 1;
        if (em->char0 != 0x27E) {
            if (em->char0 == 0x281) {
                if (em->x194 <= 0) {
                    Lb_pl_chr_set(em, 0x27E, 2, 0);
                    em->x05++;
                }
            } else {
                Lb_pl_chr_set(em, 0x27E, 2, 0);
                em->x05++;
            }
        } else {
            em->x05++;
        }
        break;
    }
}

void npcMvKEGA(em, kind)
EMW *em;
s8 kind;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        mv->f0F = 1;
        em->x05++;
        switch (kind) {
        case 0:
            if (em->char0 != 0x285) {
                Lb_pl_chr_set(em, 0x285, 0, 0);
                return;
            }
            break;
        case 1:
            if (em->char0 != 0x284) {
                Lb_pl_chr_set(em, 0x284, 0, 0);
            }
            break;
        }
        break;
    }
}

void npc_move_common(em)
EMW *em;
{
    switch (em->x15) {
    case 0x00:
        npcMvFOOTWORK(em, 0);
        return;
    case 0x75:
        npcMvFOOTWORK(em, 1);
        return;
    case 0x8E:
        npcMvFOOTWORK(em, 2);
        return;
    case 0x76:
        npcMvFOOTWORK(em, 3);
        return;
    case 0x02:
        npcMvWALK(em, 0);
        return;
    case 0x77:
        npcMvWALK(em, 1);
        return;
    case 0x8D:
        npcMvWALK(em, 2);
        return;
    case 0x78:
        npcMvWALK(em, 3);
        return;
    case 0x6A:
        npcMvENJOY(em, 0);
        return;
    case 0x6B:
        npcMvENJOY(em, 1);
        return;
    case 0x79:
        npcMvTALK(em, 0);
        return;
    case 0x7A:
        npcMvTALK(em, 1);
        return;
    case 0x7B:
        npcMvTALK(em, 2);
        return;
    case 0x6C:
        npcMvDOWN(em, 0);
        return;
    case 0x6D:
        npcMvDOWN(em, 1);
        return;
    case 0x6E:
        npcMvDOWN(em, 3);
        return;
    case 0x64:
        npcMvTOPL(em);
        return;
    case 0x7C:
        npcMvBOARD(em, 0);
        return;
    case 0x7D:
        npcMvBOARD(em, 1);
        return;
    case 0x6F:
        npcMvSHOP(em, 0);
        return;
    case 0x70:
        npcMvSHOP(em, 1);
        return;
    case 0x73:
        npcMvBEER(em);
        return;
    case 0x7E:
        npcMvDRUNKDOWN(em);
        return;
    case 0x74:
        npcMvTOPL2(em);
        return;
    case 0x7F:
        npcMvWALL(em);
        return;
    case 0x80:
        npcMvKEGA(em, 0);
        return;
    case 0x81:
        npcMvKEGA(em, 1);
        return;
    case 0x90:
        npcMvWARP(em);
        return;
    case 0x91:
        npcMvRANDWAIT(em);
        break;
    }
}

void lb_npc_bar_move(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x15) {
    case 0:
        if (em->x05 == 0) {
            em->x05++;
            em->work08 = ((ran_suu(1) & 0xFFFF) + 0xA) & 0xFF;
            if (em->char0 != 0x28A) {
                Lb_pl_chr_set(em, 0x28A, 0, 0);
                return;
            }
        } else {
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0x67);
                return;
            }
        }
        break;
    case 0x65:
    case 0x64:
        mv->x2D = 1;
        if (em->x05 == 0) {
            em->x05++;
            Lb_pl_chr_set(em, 0x28B, 8, 0);
            return;
        }
        if (em->x194 == 0 && em->char0 == 0x28B) {
            Lb_pl_chr_set(em, 0x28C, 2, 0);
        }
        if (lb_sys.x68 == 0) {
            Lb_act_set(em, 0, 0x66);
            return;
        }
        break;
    case 0x66:
        switch (em->x05) {
        case 0:
            em->x05++;
            Lb_pl_chr_set(em, 0x28D, 2, 0);
            return;
        case 1:
            if (em->x194 == 0) {
                Lb_pl_chr_set(em, 0x28A, 0xE, 0);
                em->work08 = (ran_suu(1) & 0xFF) + 0xFF;
                em->x05++;
                return;
            }
            break;
        case 2:
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0);
                return;
            }
            break;
        }
        break;
    case 0x67:
        if (em->x05 == 0) {
            em->x05++;
            Lb_pl_chr_set(em, 0x28E, 0xA, 0);
            return;
        }
        if (em->x194 == 0) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void lb_npc_mother_move(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x15) {
    case 0:
        switch (em->x05) {
        case 0:
            em->x05++;
            em->work08 = ((ran_suu(1) & 0xFFFF) + 0x64) & 0xFF;
            Lb_pl_chr_set(em, 0x2B2, 0, 0);
            return;
        case 1:
            if (--em->work08 <= 0) {
                if (!((u16)ran_suu(1) & 1)) {
                    em->x05++;
                    em->work08 = ((u16)ran_suu(1) & 3) + 3;
                    Lb_pl_chr_set(em, 0x2B3, 6, 0);
                    return;
                }
                em->x05 = 4;
                Lb_pl_chr_set(em, 0x2B6, 8, 0);
                return;
            }
            break;
        case 2:
            if (em->x194 <= 0) {
                if (--em->work08 <= 0) {
                    em->x05++;
                    Lb_pl_chr_set(em, 0x2B4, 0xA, 0);
                    return;
                }
            }
            break;
        case 3:
            if (em->x194 <= 0) {
                em->x05 = 1;
                em->work08 = 0x78;
                Lb_pl_chr_set(em, 0x2B2, 4, 0);
                return;
            }
            break;
        case 4:
            if (em->x194 <= 0) {
                em->x05 = 1;
                em->work08 = ((ran_suu(1) & 0xFFFF) + 0x64) & 0xFF;
                Lb_pl_chr_set(em, 0x2B2, 0xA, 0);
                return;
            }
            break;
        }
        break;
    case 0x64:
        switch (em->x05) {
        case 0:
            mv->x2D = 1;
            em->x05++;
            Lb_pl_chr_set(em, 0x2B5, 0xA, 0);
            return;
        case 1:
            mv->x2D = 1;
            if (lb_sys.x68 != 0xC && lb_sys.x68 != 0x2B) {
                em->x05++;
                Lb_pl_chr_set(em, 0x2B2, 0xA, 0);
                return;
            }
            break;
        case 2:
            if (EM_S32(em, 0x1E4) <= 0) {
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}

void lb_npc_father_move(em)
EMW *em;
{
    PLW *pl = &player_work[game_w.master];
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    f32 dist = flvecCalcDistance(em->pos, pl->pos);

    switch (em->x15) {
    case 0:
        if (em->x05 == 0) {
            em->x05++;
            em->work08 = ((ran_suu(1) & 0xFFFF) + 0xA) & 0xFF;
            if (game_w.stage == 0x50) {
                Lb_pl_chr_set(em, 0x2A8, 4, 0);
            } else {
                Lb_pl_chr_set(em, 0x2AB, 0xE, 0);
            }
        }
        if (dist < 400.0f && game_w.stage != 0x50) {
            Lb_act_set(em, 0, 0x71);
            return;
        }
        break;
    case 0x71:
        switch (em->x05) {
        case 0:
            em->x05++;
            if (((EMW *)pl)->x15 == 2 || ((EMW *)pl)->x15 == 0) {
                Lb_pl_chr_set(em, 0x2AC, 0xA, 0);
                return;
            }
            Lb_pl_chr_set(em, 0x2AD, 8, 0);
            return;
        case 1:
            if (dist < 300.0f) {
                em->x05++;
                if (em->char0 != 0x2AD) {
                    Lb_pl_chr_set(em, 0x2AD, 8, 0);
                    return;
                }
            } else if (!(dist <= 400.0f)) {
                em->x05 = 3;
                em->work08 = 0x78;
                return;
            }
            break;
        case 2:
            if (em->x194 <= 0) {
                Lb_act_set(em, 0, 0x72);
                return;
            }
            break;
        case 3:
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0);
                return;
            }
            break;
        }
        break;
    case 0x72:
        switch (em->x05) {
        case 0:
            em->x05++;
            if (!((u16)ran_suu(1) & 1)) {
                Lb_pl_chr_set(em, 0x2A9, 4, 0);
                return;
            }
            Lb_pl_chr_set(em, 0x2A8, 8, 0);
            return;
        case 1:
            if (!(dist <= 300.0f)) {
                em->x05++;
                em->work08 = 0x78;
                return;
            }
            break;
        case 2:
            if (--em->work08 <= 0) {
                if (dist < 300.0f) {
                    em->x05--;
                    return;
                }
                Lb_act_set(em, 0, 0);
                return;
            }
            break;
        }
        break;
    case 0x64:
        switch (em->x05) {
        case 0:
            mv->x2D = 1;
            em->x05++;
            Lb_pl_chr_set(em, 0x2AA, 0xA, 0);
            return;
        case 1:
            mv->x2D = 1;
            if (lb_sys.x68 != 9) {
                Lb_pl_chr_set(em, 0x2A8, 0x10, 0);
                em->x05++;
                return;
            }
            break;
        case 2:
            if (!(dist <= 300.0f) && em->x194 <= 0) {
                em->x05++;
                em->work08 = 0x78;
                return;
            }
            break;
        case 3:
            if (--em->work08 <= 0) {
                if (dist < 300.0f) {
                    em->x05--;
                    return;
                }
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}

void lb_npc_old_material(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x15) {
    case 0:
        if (em->x05 == 0) {
            em->x05++;
            Lb_pl_chr_set0(em, 0x3F7, 2, 0, 0);
            return;
        }
        break;
    case 0x64:
        mv->x2D = 1;
        switch (em->x05) {
        case 0:
            em->x05++;
            if (em->char0 != 1) {
                Lb_pl_chr_set0(em, 0x3F8, 4, 0, 0);
                return;
            }
            break;
        case 1:
            if (em->x194 <= 0) {
                Lb_act_set(em, 0, 0x65);
                return;
            }
            break;
        }
        break;
    case 0x65:
        switch (em->x05) {
        case 0:
            mv->x2D = 1;
            em->x05++;
            Lb_pl_chr_set0(em, 0x3F9, 2, 0, 0);
            return;
        case 1:
            mv->x2D = 1;
            if (em->x194 < 2 && em->char0 != 0x3FA) {
                Lb_pl_chr_set0(em, 0x3FA, 2, 0, 0);
            }
            if (lb_sys.x68 == 0) {
                em->work08 = (ran_suu(1) & 0xFF) + 0x258;
                em->x05++;
                Lb_pl_chr_set0(em, 0x3F9, 0x1A, 0, 0);
                return;
            }
            break;
        case 2:
            if (--em->work08 == 0) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x3F7, 0x1E, 0, 0);
                return;
            }
            if (em->x194 == 0) {
                Lb_pl_chr_set0(em, 0x3F9, 2, 0, 0);
                return;
            }
            break;
        case 3:
            if (em->x194 <= 0) {
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}

void lb_npc_old_mix(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    int idx = game_w.stage != 0x4E;
    VEC3 *tbl;
    VEC3 v;

    switch (em->x15) {
    case 0:
        mv->f0F = 1;
        if (em->x05 == 0) {
            em->x05++;
            Lb_pl_chr_set0(em, 0x3FB, 2, 0, 0);
            return;
        }
        if (em->x194 == 0) {
            Lb_pl_chr_set0(em, 0x3FB, 2, 0x58, 0);
            return;
        }
        break;
    case 0x64:
        mv->x2D = 1;
        switch (em->x05) {
        case 0:
            if (em->char0 == 0x3FB) {
                if (frame_check2(em, 89.0f, 0) != 0) {
                    em->x05++;
                    return;
                }
            } else if (em->char0 == 0x3FC || em->char0 == 0x3F0) {
                em->x05 = 2;
                return;
            } else {
                em->x05++;
                return;
            }
            break;
        case 1:
            em->x05++;
            Lb_pl_chr_set0(em, 0x3FC, 4, 0, 0);
            return;
        case 2:
            if (em->x194 <= 0) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x3F0, 4, 0, 0);
                return;
            }
            break;
        case 3:
            if (lb_sys.x68 == 0) {
                Lb_pl_chr_set0(em, 0x3FB, 0xA, 0, 0);
                em->x05++;
                return;
            }
            break;
        case 4:
            Lb_act_set(em, 0, 0);
            return;
        }
        break;
    case 0x68:
        switch (em->x05) {
        case 0:
            Lb_pl_chr_set0(em, 0x3FD, 0, 0, 0);
            em->x05++;
            return;
        case 1:
            tbl = &old_dir_tbl[(u16)idx];
            em->ang[1] += (u16)((s16)(u16)Lb_get_angle(em, tbl) / 2);
            if (flvecCalcDistance(em->pos, tbl) <= 40.0f) {
                Lb_pl_chr_set0(em, 0x3E9, 0, 0, 0);
                em->work08 = 0x3C;
                em->x05++;
                cnWrap_SoundRequest(0xD);
                return;
            }
            break;
        case 2:
            if (em->work08 % 5 == 0) {
                v.x = 2100.0f;
                v.z = 1800.0f;
                v.y = 0.0f;
                Eft02_set_pos(&v, 7, 0);
            }
            if (--em->work08 <= 0) {
                u8 s = em->x05;
                tbl = &old_pos_tbl[(u16)idx];
                em->x05 = s + 1;
                em->ang[1] += (u16)(s16)(u16)Lb_get_angle(em, tbl, s);
                *(VEC3 *)em->pos = *tbl;
                Lb_pl_chr_set0(em, 0x3FE, 2, 0, 0);
                return;
            }
            break;
        case 3:
            if (em->x194 == 0) {
                Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
                Lb_act_set(em, 0, 0x64);
                em->x05 = 2;
            }
            break;
        }
        break;
    }
}
