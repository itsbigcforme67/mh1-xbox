/* lbnpc, run 4: npcPigATACK .. npcPigJOY (lobby.bin 0x005A1F30-0x005A22D8): the matching functions of lbnpc_nm.c. */
#include "lbnpc_proto.h"

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
