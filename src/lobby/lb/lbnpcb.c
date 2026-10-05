/* lbnpc, run 2: npcCatSLEEP .. npcCatHELLO (lobby.bin 0x005A0A50-0x005A1040): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

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
