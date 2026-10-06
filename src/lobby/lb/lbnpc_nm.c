/* lbnpc - lobby.bin 0x0059DB40-0x005A2A20: movement scripts of the town NPCs (humans npcMv*, cats npcCat*, pigs npcPig*)
 * and the lb_npc_*_move dispatchers. Near-match file in address order (tools/lbmerge.py ... include/lbnpc_proto.h lbnpc.h). */
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

void lb_npc_old_guild(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x15) {
    case 0:
        if (em->x05 == 0) {
            em->x05++;
            em->work08 = ((ran_suu(1) & 0xFFFF) + 0x12C) & 0xFF;
            if (em->char0 != 0x3FF) {
                Lb_pl_chr_set0(em, 0x3FF, 2, 0, 0);
                return;
            }
        } else {
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0);
                return;
            }
        }
        break;
    case 0x69:
        switch (em->x05) {
        case 0:
            em->x05++;
            Lb_pl_chr_set0(em, 0x400, 2, 0, 0);
            return;
        case 1:
            if (em->x194 <= 0) {
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
            Lb_pl_chr_set0(em, 0x401, 8, 0, 0);
            return;
        case 1:
            mv->x2D = 1;
            if (em->x194 <= 0) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x402, 2, 0, 0);
                return;
            }
            if (lb_sys.x68 == 0) {
                em->x05 += 3;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                return;
            }
            break;
        case 2:
            mv->x2D = 1;
            if (em->x194 < 2) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x403, 2, 0, 0);
                return;
            }
            if (lb_sys.x68 == 0) {
                em->x05 += 2;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                Lb_pl_chr_set0(em, 0x402, 0xA, 0, 0);
                return;
            }
            break;
        case 3:
            mv->x2D = 1;
            if (lb_sys.x68 == 0) {
                em->x05++;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x60;
                Lb_pl_chr_set0(em, 0x402, 0xA, 0, 0);
                return;
            }
            break;
        case 4:
            if (--em->work08 <= 0) {
                em->x05++;
                em->work08 = (ran_suu(1) & 0xFF) + 0x258;
                Lb_pl_chr_set0(em, 0x3FF, 0x3C, 0, 0);
                return;
            }
            break;
        case 5:
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0);
            }
            break;
        }
        break;
    }
}

void npcCatSLEEP(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    mv->x28 = 2;
    switch (em->x05) {
    case 0:
        em->x05++;
        mv->f0F = 1;
        if (em->char0 != 0x431) {
            Lb_pl_chr_set0(em, 0x431, 4, 0, 0);
        }
        break;
    }
}

void npcCatFOOTWORK(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        mv->x28 = 0;
        mv->f0F = 0;
        em->work08 = ((ran_suu(1) & 0xFFFF) + 0xFF) & 0xFF;
        Lb_pl_chr_set0(em, 0x3E9, 0x14, 0, 0);
        return;
    case 1:
        if (mv->kind == 0x53) {
            if (lb_sys.x68 == 0x11 && LBS8(9) == 0 && LBS8(6) == 2 && mv->x26 == 0) {
                Lb_act_set(em, 0, 0x85);
                return;
            }
        } else if (flvecCalcDistance(em->pos, (u8 *)&player_work[game_w.master] + 0xAC) < 130.0f) {
            if (mv->x26 == 0) {
                Lb_act_set(em, 0, 0x84);
                mv->x26 = 1;
                em->work08 = (ran_suu(1) & 0xFF) + 0xFF;
                return;
            }
        } else {
            if (--em->work08 <= 0) {
                Lb_act_set(em, 0, 0x83);
                return;
            }
            mv->x26 = 0;
        }
        break;
    }
}

void npcCatRUN(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    LB_ROUTE *r;
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 != 0x3F3) {
            Lb_pl_chr_set0(em, 0x3F3, 4, 0, 0);
            return;
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
        if (d <= 130.0f) {
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

void npcCatKYORO(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_pl_chr_set0(em, 0x3EC, 4, 0, 0);
        return;
    case 1:
        if (em->x194 <= 0) {
            em->x05++;
            Lb_pl_chr_set0(em, 0x3E9, 8, 0, 0);
            return;
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void npcCatHELLO(em)
EMW *em;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x0E = (s16)(u16)Lb_get_angle(em, (u8 *)&player_work[game_w.master] + 0xAC) / 10;
        em->work08 = 0xA;
        Lb_pl_chr_set0(em, 0x42F, 8, 0, 0);
        return;
    case 1:
        if (--em->work08 <= 0) {
            em->ang[1] += em->x0E;
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            Lb_pl_chr_set0(em, 0x3E9, 0xA, 0, 0);
            em->work08 = 0xA;
            em->x05++;
            return;
        }
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->ang[1] -= em->x0E;
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void npcCatWAITER(em)
EMW *em;
{
    PLW *pl;
    LB_WAITEX *ex;
    f32 *route;
    s32 *flags;
    u8 step;
    s32 t;
    u8 m;
    u8 stage;
    f32 *r;

    m = game_w.master;
    pl = &player_work[m];
    flags = ((s32 **)&((u8 *)waiter_tbl_03)[0x3C])[game_w.stage];
    ex = (LB_WAITEX *)em->ex;
    if (pl->fish878 == 0) {
        Lb_act_set(em, 0, 0);
        return;
    }
    step = em->x05;
    route = ex->route;
    switch (step) {
    case 0:
        em->x05 = step + 1;
        stage = game_w.stage;
        if ((u32)(stage - 0x51) <= 1 || stage == 0x53) {
            em->work08 = 0x3C;
            return;
        }
        em->work08 = 1;
        return;
    case 1:
        t = em->work08 - 1;
        em->work08 = t;
        if (t <= 0) {
            em->x05++;
            ex->x26 = 1;
            *(s16 *)((u8 *)&lb_sys + 0x76) = 1;
            stage = game_w.stage;
            if (stage == 0x56) {
                r = waiter_tbl[1];
            } else {
                r = waiter_tbl[stage - 0x51 + (*(u16 *)pl->fish878 - 0xE)];
            }
            if (r != 0) {
                ex->route = r;
                ex->idx = 0;
                em->x0E = Lb_get_angle(em, r + (ex->idx + 1) * 3);
                em->work08 = 3;
                stage = game_w.stage;
                if (stage == 0x54 || stage == 0x55) {
                    Lb_pl_chr_set0(em, 0x3F7, 4, 0, 0);
                    return;
                }
                Lb_pl_chr_set0(em, 0x3F4, 4, 0, 0);
                return;
            }
        }
        break;
    case 2:
        t = em->work08 - 1;
        em->work08 = t;
        if (t >= 0) {
            em->ang[1] += em->x0E / 3;
        }
        if (flvecCalcDistance(em->pos, route + (ex->idx + 1) * 3) < 50.0f) {
            ex->idx++;
            if (flags[ex->idx] == 1) {
                em->x0E = Lb_get_angle(em, route + (ex->idx + 1) * 3);
                em->work08 = 3;
                return;
            }
            em->x05++;
            em->x0E = Lb_get_angle(em, (u8 *)&player_work[game_w.master] + 0xAC);
            em->work08 = 10;
            Lb_pl_chr_set0(em, 0x430, 8, 0, 0);
            return;
        }
        break;
    case 3:
        t = em->work08 - 1;
        em->work08 = t;
        if (t > 0) {
            em->ang[1] += em->x0E / 10;
            return;
        }
        if (frame_check2(em, 130.0f, 0) != 0) {
            em->x05++;
            Lb_eat_to_rcpt();
            return;
        }
        break;
    case 4:
        if (LBS8(6) != 3) {
            em->x05 = step + 1;
            em->x0E = Lb_get_angle(em, route + ex->idx * 3);
            em->work08 = 3;
            Lb_pl_chr_set0(em, 0x3F4, 8, 0, 0);
            return;
        }
        break;
    case 5:
        t = em->work08 - 1;
        em->work08 = t;
        if (t >= 0) {
            em->ang[1] += em->x0E / 3;
        }
        if (flvecCalcDistance(em->pos, route + ex->idx * 3) < 50.0f) {
            t = ex->idx - 1;
            ex->idx = t;
            if (t >= 0) {
                em->x0E = Lb_get_angle(em, route + ex->idx * 3);
                em->work08 = 3;
                return;
            }
            if (*(s32 *)((u8 *)&lb_sys + 0x68) != 0x11 || LBS8(6) == 6 || LBS8(6) == 7) {
                ex->x26 = 0;
                *(s16 *)((u8 *)&lb_sys + 0x76) = 0;
                Lb_act_set(em, 0, 0);
                return;
            }
            em->x05++;
            ex->idx = 3;
            em->x0E = Lb_get_angle(em, route + (ex->idx + 1) * 3);
            em->work08 = 3;
            Set21_set(em, (s16)(*(u16 *)pl->fish878 - 0xE));
            Lb_pl_chr_set0(em, 0x3F8, 4, 0, 0);
            return;
        }
        break;
    case 6:
        if (*(s32 *)((u8 *)&lb_sys + 0x68) != 0x11 || LBS8(6) == 6 || LBS8(6) == 7) {
            ex->x26 = 0;
            *(s16 *)((u8 *)&lb_sys + 0x76) = 0;
            ex->idx = 0;
            em->x05 += 2;
            return;
        }
        t = em->work08 - 1;
        em->work08 = t;
        if (t >= 0) {
            em->ang[1] += em->x0E / 3;
        }
        if (flvecCalcDistance(em->pos, route + (ex->idx + 1) * 3) < 70.0f) {
            ex->idx++;
            if (flags[ex->idx] == 1) {
                em->x0E = Lb_get_angle(em, route + (ex->idx + 1) * 3);
                em->work08 = 3;
                return;
            }
            em->x05++;
            em->work08 = 100;
            em->x0E = Lb_get_angle(em, (u8 *)&player_work[game_w.master] + 0xAC);
            Lb_pl_chr_set0(em, 0x432, 8, 0, 0);
            return;
        }
        break;
    case 7:
        if (*(s32 *)((u8 *)&lb_sys + 0x68) != 0x11 || LBS8(6) == 6 || LBS8(6) == 7) {
            ex->x26 = 0;
            *(s16 *)((u8 *)&lb_sys + 0x76) = 0;
            ex->idx = 0;
            em->x05++;
            return;
        }
        t = em->work08 - 1;
        em->work08 = t;
        if (t >= 0x5A) {
            em->ang[1] += em->x0E / 10;
        } else if (em->work08 == 0x42) {
            Lb_eat_to_eat();
        }
        if (em->x194 <= 0) {
            em->x05++;
            em->work08 = 100;
            Lb_pl_chr_set0(em, 0x430, 6, 12, 0);
            return;
        }
        break;
    case 8:
        if (em->x194 > 0) {
            t = em->work08 - 1;
            em->work08 = t;
            if (t > 0) {
                break;
            }
        }
        em->x05++;
        Lb_pl_chr_set0(em, 0x3F4, 4, 0, 0);
        em->x0E = Lb_get_angle(em, route + ex->idx * 3);
        em->ang[1] += em->x0E / 4;
        return;
    case 9:
        em->x0E = Lb_get_angle(em, route + ex->idx * 3);
        em->ang[1] += em->x0E / 4;
        if (flvecCalcDistance(em->pos, route + ex->idx * 3) < 50.0f) {
            t = ex->idx - 1;
            ex->idx = t;
            if (t >= 3) {
                em->x0E = Lb_get_angle(em, route + ex->idx * 3);
                em->work08 = 3;
                return;
            }
            ex->x26 = 0;
            *(s16 *)((u8 *)&lb_sys + 0x76) = 0;
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void lb_npc_cat_move(em)
EMW *em;
{
    switch (em->x15) {
    case 0:
        npcCatFOOTWORK(em);
        return;
    case 1:
        npcCatRUN(em);
        return;
    case 0x82:
        npcCatSLEEP(em);
        return;
    case 0x64:
        npcMvTOPL(em);
        return;
    case 0x83:
        npcCatKYORO(em);
        return;
    case 0x84:
        npcCatHELLO(em);
        return;
    case 0x85:
        npcCatWAITER(em);
        return;
    case 0x90:
        npcMvWARP(em);
        break;
    }
}

void npcPigFOOTWORK(em, kind)
EMW *em;
s8 kind;
{
    switch (em->x05) {
    case 0:
        em->x05++;
        if (kind == 0) {
            Lb_pl_chr_set0(em, 0x3E9, 2, 0, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x3E9, 8, 0, 0);
        break;
    }
}

void npcPigSLEEP(em, kind)
EMW *em;
s8 kind;
{
    PLW *pl = &player_work[game_w.master];

    switch (em->x05) {
    case 0:
        em->x05++;
        pl_flag_set((PLW *)em, 0x20000);
        if (kind == 0) {
            Lb_pl_chr_set0(em, 0x432, 0, 0x58, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x432, 0xE, 0, 0);
        return;
    case 1:
        if (((EMW *)pl)->x15 != 0x33 && ((EMW *)pl)->x15 != 0x34) {
            pl_flag_clr((PLW *)em, 0x20000);
            Lb_pl_chr_set0(em, 0x431, 4, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            Lb_pl_chr_set0(em, 0x3E9, 0xA, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 3:
        if (em->x194 <= 0) {
            Lb_act_set(em, 0, 0x8C);
        }
        break;
    }
}

void npcPigTOPL(em)
EMW *em;
{
    PLW *pl = &player_work[game_w.master];
    VEC3 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_act_set(pl, 0, 0x58);
        em->work08 = 0xA;
        em->x0E = Lb_get_angle(em, *(s32 *)((u8 *)em + 0x7A0) + 0xAC);
        Lb_pl_chr_set0(em, 0x3EA, 2, 0, 0);
        return;
    case 1:
        if (--em->work08 >= 0) {
            em->ang[1] += em->x0E / 10;
            return;
        }
        em->x05++;
        return;
    case 2:
        if (flvecCalcDistance(em->pos, pl->pos) < 78.0f) {
            d.x = em->pos[0] - pl->pos[0];
            d.y = em->pos[1] - pl->pos[1];
            d.z = em->pos[2] - pl->pos[2];
            flvecNormalize(&d);
            d.x = d.x * 78.0f;
            d.y *= 78.0f;
            d.z *= 78.0f;
            EM_F32(em, 0x934) = pl->pos[0] + d.x;
            EM_F32(em, 0x938) = pl->pos[1] + d.y;
            EM_F32(em, 0x93C) = pl->pos[2] + d.z;
            Lb_Em_adj_calc(em, 0x14);
            Lb_pl_chr_set0(em, 0x3F2, 4, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 3:
        em->ang[1] += (s16)(u16)Lb_get_angle(em, pl->pos) / 5;
        break;
    }
}

void npcPigATACK(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    s16 idx;

    if (em->x05 < 3) {
        u16 t = mv->x26 - 1;
        mv->x26 = t;
        if ((s16)t <= 0) {
            Eft25_set(em, 1);
            mv->x26 = ((ran_suu(1) & 0xFFFF) % 5) + 3;
        }
    }
    switch (em->x05) {
    case 0:
        idx = game_w.stage - 0x51;
        if (idx < 0 || idx > 0x55) {
            idx = 0;
        }
        lb_sys.x88[idx] = 1;
        em->x05++;
        EM_F32(em, 0x930) = 3.0f;
        Lb_pl_chr_set0(em, 0x3F6, 4, 0x26, 0);
        em->work08 = 0x14;
        return;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em->work08 = 5;
            EM_F32(em, 0x930) = 1.0f;
            Lb_pl_chr_set0(em, 0x3FA, 4, 0, 0);
            Lb_act_set(&player_work[game_w.master], 0, 0x57);
            return;
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
            em->x05++;
            return;
        }
        break;
    case 3:
        if (D_3E4C05[game_w.master * 0xA00] == 0) {
            Lb_act_set(em, 0, 0x8A);
        }
        break;
    }
}

void npcPigJOY(em)
EMW *em;
{
    s16 idx;

    switch (em->x05) {
    case 0:
        idx = game_w.stage - 0x51;
        if (idx < 0 || idx > 0x55) {
            idx = 0;
        }
        lb_sys.x88[idx] = 2;
        em->x05++;
        Lb_pl_chr_set0(em, 0x3F3, 4, 0, 0);
        Eft25_set(em, 0);
        return;
    case 1:
        if (em->x194 <= 0) {
            Lb_pl_chr_set0(em, 0x3EF, 6, 0, 0);
            em->x05++;
            em->work08 = 0x64;
            return;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            Lb_pl_chr_set0(em, 0x3E9, 0x18, 0, 0);
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x68 = 0;
            em->x05++;
            return;
        }
        break;
    case 3:
        if (em->x194 < 2) {
            Lb_act_set(em, 0, 0x8C);
        }
        break;
    }
}

void npcPigEXIT(em)
EMW *em;
{
    VEC3 v;
    f32 *p = St_unique_tbl[game_w.stage];

    v.x = p[1];
    v.y = p[2];
    v.z = p[3];
    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = 0xA;
        em->x0E = Lb_get_angle(em, &v);
        Lb_pl_chr_set0(em, 0x3F7, 2, 0, 0);
        return;
    case 1:
        if (--em->work08 >= 0) {
            em->ang[1] += em->x0E / 10;
            return;
        }
        em->x05++;
        return;
    case 2:
        if (em->work08 >= 0x1F4) {
            Lb_act_set(em, 0, 0);
        }
        break;
    }
}

void npcPigWALK(em)
EMW *em;
{
    LB_NPCMV *mv = (LB_NPCMV *)em->ex;
    LB_ROUTE *r;
    f32 d;
    s16 i;
    LB_ROUTE *rt;

    switch (em->x05) {
    case 0:
        em->x05++;
        if (em->char0 == 0x3EF) {
            Lb_pl_chr_set0(em, 0x3EB, 8, 0, 0);
            return;
        }
        Lb_pl_chr_set0(em, 0x3EB, 4, 0, 0);
        return;
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
            i = mv->idx;
            rt = mv->route;
            if (rt[i].wait == -1) {
                mv->idx = 0;
                return;
            }
            if ((u16)ran_suu(1) & 1) {
                em->x05++;
                em->work08 = ((u16)ran_suu(1) & 0x1F) + 0x136;
                Lb_pl_chr_set0(em, 0x3EF, 0xA, 0, 0);
                return;
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
    case 2:
        if (--em->work08 <= 0) {
            i = mv->idx;
            r = &mv->route[i];
            if (em->x15 != r->act) {
                em->x05++;
                Lb_act_set(em, 0, (u16)r->act, i);
                mv->cnt = 0;
                return;
            }
            em->x05 = 0;
        }
        break;
    }
}

void npcPigWALK2(em)
EMW *em;
{
    PLW *pl = &player_work[game_w.master];
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        Lb_pl_chr_set0(em, 0x3EB, 4, 0, 0);
        return;
    case 1:
        em->x0E = Lb_get_angle(em, pl->pos);
        em->ang[1] += em->x0E / 10;
        d = flvecCalcDistance(em->pos, pl->pos);
        if (((EMW *)pl)->x15 == 0x33 || ((EMW *)pl)->x15 == 0x34) {
            if (d < 65.0f) {
                em->x05++;
                Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
                return;
            }
            if (em->char0 != 0x3EB) {
                Lb_pl_chr_set0(em, 0x3EB, 2, 0, 0);
                return;
            }
        } else {
            if (d < 130.0f) {
                if (em->char0 != 0x3E9) {
                    Lb_pl_chr_set0(em, 0x3E9, 4, 0, 0);
                    return;
                }
            } else if (em->char0 != 0x3EB) {
                Lb_pl_chr_set0(em, 0x3EB, 2, 0, 0);
                return;
            }
        }
        break;
    case 2:
        if (em->x194 <= 0) {
            em->x05++;
            Lb_pl_chr_set0(em, 0x3EF, 0xA, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 <= 0) {
            Lb_act_set(em, 0, 0x87);
        }
        break;
    }
}

void lb_npc_pig_move(em)
EMW *em;
{
    switch (em->x15) {
    case 0:
        npcPigFOOTWORK(em, 0);
        return;
    case 0x8F:
        npcPigFOOTWORK(em, 1);
        return;
    case 0x86:
        npcPigSLEEP(em, 0);
        return;
    case 0x87:
        npcPigSLEEP(em, 1);
        return;
    case 0x64:
        npcPigTOPL(em);
        return;
    case 0x88:
        npcPigATACK(em);
        return;
    case 0x89:
        npcPigJOY(em);
        return;
    case 0x8A:
        npcPigEXIT(em);
        return;
    case 0x8B:
        npcPigWALK(em);
        return;
    case 0x8C:
        npcPigWALK2(em);
        break;
    }
}
