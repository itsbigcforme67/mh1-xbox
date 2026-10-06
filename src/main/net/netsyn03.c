/* Network play sync, pending slots (SLPM_654.95 0x001BD260-0x001BD660): player position slots (2 x 0x10 at +0x89C), enemy action slots (2 x 0x34 at +0x960).
 * net_*_set queue the current state under the net timer; net_receive_* count it down and apply it. */
#include "types.h"
#include "netsyn.h"

extern NGW game_w;
extern u8 player_work[];
void func_535A10(NEMV *, int, int, int);

void net_plpos_set(NPLV *pl, f32 *pos) {
    s8 i;

    if (Online_ck() != 0) {
        for (i = 0; i < 2; i++) {
            if (pl->slot[i].timer == 0) {
                pl->slot[i].timer = game_w.x1B0;
                pl->slot[i].x = pos[0];
                pl->slot[i].y = pos[1];
                pl->slot[i].z = pos[2];
                return;
            }
        }
    }
}

void net_receive_pl_pos_set(void) {
    s8 i;
    NPLV *pl;
    s16 t;

    pl = (NPLV *)(player_work + game_w.master * 0xA00);
    if (Online_ck(game_w.master) != 0) {
        for (i = 0; i < 2; i++) {
            t = pl->slot[i].timer;
            if (t != 0) {
                t--;
                pl->slot[i].timer = t;
                if (t <= 0) {
                    pl->slot[i].timer = 0;
                    pl->x890 = pl->slot[i].x;
                    pl->x894 = pl->slot[i].y;
                    pl->x898 = pl->slot[i].z;
                }
            }
        }
    }
}

void net_emact_set(NEMV *em, s8 kind) {
    s8 i;

    if (Online_ck() != 0) {
        for (i = 0; i < 2; i++) {
            if (em->act[i].timer == 0) {
                em->act[i].timer = game_w.x1B0;
                em->act[i].kind = kind;
                em->act[i].x = em->posx;
                em->act[i].y = em->posy;
                em->act[i].z = em->posz;
                em->act[i].ang[0] = em->xA0;
                em->act[i].ang[1] = em->xA4;
                em->act[i].ang[2] = em->xA8;
                if (em->x8C3 == 0) {
                    em->act[i].b76 = em->x794;
                    em->act[i].b77 = em->x795;
                    em->x9D8 = 1;
                } else {
                    em->act[i].b76 = em->x14;
                    em->act[i].b77 = em->x15;
                    if (em->x14 != 5) {
                        em->act[i].timer = 1;
                    } else {
                        em->act[i].timer = 0;
                    }
                }
                em->act[i].u80 = em->x954;
                em->act[i].b79 = em->x797;
                em->act[i].b7A = em->x7A8;
                em->act[i].b7B = em->x7A9;
                em->act[i].b7E = em->x736;
                em->act[i].s7C = em->x08;
                return;
            }
        }
    }
}

void net_receive_em_act(NEMV *em) {
    s8 i;
    s16 t;
    u8 k1;
    u8 k2;

    if (Online_ck() != 0) {
        for (i = 0; i < 2; i++) {
            t = em->act[i].timer;
            if (t != 0) {
                t--;
                em->act[i].timer = t;
                if (t <= 0) {
                    em->act[i].timer = 0;
                    if (em->x9ED != 0) {
                        em->x9ED = 0;
                    } else if (em->x8C3 != 0) {
                        return;
                    }
                    {
                        if (em->x04 > 0 && (u32)(em->x14 - 4) > 1 && em->act[i].kind == 0) {
                            em->posx = em->act[i].x;
                            em->posy = em->act[i].y;
                            em->posz = em->act[i].z;
                            em->xA0 = (u16)em->act[i].ang[0];
                            em->xA4 = (u16)em->act[i].ang[1];
                            em->xA8 = (u16)em->act[i].ang[2];
                            k1 = em->act[i].b76;
                            k2 = em->act[i].b77;
                            em->x881 = em->act[i].b79;
                            em->x882 = em->act[i].b7A;
                            em->x883 = em->act[i].b7B;
                            em->x736 = em->act[i].b7E;
                            em->x08 = em->act[i].s7C;
                            em->x39A = em->act[i].u80;
                            em->x14 = k1;
                            em->x15 = k2;
                            func_535A10(em, k1, k2, 0);
                        }
                    }
                }
            }
        }
    }
}
