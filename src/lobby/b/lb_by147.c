/* lb_by147 - agent B 0x005A1040-0x005A19D0: npcCatWAITER (waiter cat: walks the route to the guest, serves the dish, walks back; the leave-conditions are the then-part of an if so they are laid out first). */
#include "lbnpc_proto.h"

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
