/* emride_nm - SLPM_654.95 0x0010B060-0x0010B6C0: em_ride_sub, the Lao-Shan Lung (kind 7) back as a floor. NEAR-MATCH (not byte-exact: the original keeps em, n, cnt, j and the matrix pointer in stack slots and hoists stack-array addresses into s-registers; mine uses 528 bytes of frame, 448 original).
 * Per hunter on the monster's stage: a hunter already riding (PLW+0x604 = 1 or 3, PLW+0x605 = the monster's id
 * at EMW+0x13) is carried along (his position follows joint PLW+0x606 through the saved old matrix, ride_ofs_calc),
 * then a short segment from above his head to 60 units lower is tested against the quads of em_ride_data[kind]
 * (per quad: joint index, then four corner points scaled by the monster's scale and put through the joint's world
 * matrix; two triangles each, VectorHitCheck). A hit puts him on the surface and starts the ride (jump -> act 0/0x11
 * when falling in the air actions 6 / 0x15, else pl_to_normal). No hit and not mid-act 2: PLW+0x604 is cleared.
 * Written from the asm by agent B (8 Oct 2026); compare with check.py before trusting it. */
#include "types.h"
#include "em.h"
#include "pl.h"

extern PLW player_work[4];
extern f32 *em_ride_data[];

s32 Em_stg_ck();
s32 Pl_stg_ck();
s32 Pl_stg_ck_tw();
s32 VectorHitCheck(f32 *, f32 *, f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void flmatGetTrans(f32 *, f32 *);
void ride_ofs_calc(PLW *, EMW *, s16, f32 *);
void Pl_act_set(PLW *, int, int, int);
void pl_to_normal(PLW *, int, int, int);
void rate_clear(PLW *);

#define PB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PF(p, o) (*(f32 *)((u8 *)(p) + (o)))

void em_ride_sub(EMW *em) {
    PLW *pl = player_work;
    u8 *mats = *(u8 **)((u8 *)em->mdl + 0x24);
    int n;
    int cnt;
    int j;
    f32 *q;
    int i;
    f32 *m;
    f32 old[3];
    f32 ofs[3];
    f32 tmp[3];
    f32 v[3];
    f32 w[3];
    f32 quad[4][3];
    f32 tri[3][3];
    f32 a[3];
    f32 b[3];

    if (em->be_flag == 0 || em->x01 == 0) {
        return;
    }
    if (em->kind != 7 || !(Em_stg_ck(em) & 0xFF)) {
        return;
    }
    for (n = 0; n < 4; n++, pl = (PLW *)((u8 *)pl + 0xA00)) {
        if (!(Pl_stg_ck_tw(pl, em) & 0xFF) || !(Pl_stg_ck(pl) & 0xFF)) {
            continue;
        }
        if ((PB(pl, 0x604) == 1 || PB(pl, 0x604) == 3) && PB(pl, 0x605) == PB(em, 0x13)) {
            flvecCopy(old, &PF(pl, 0xAC));
            j = PB(pl, 0x606);
            ride_ofs_calc(pl, em, (s16)j, ofs);
            m = (f32 *)(mats + j * 0x190);
            flvecApplyMat33(tmp, ofs, m);
            flmatGetTrans(ofs, m);
            PF(pl, 0xAC) = tmp[0] + ofs[0];
            PF(pl, 0xB0) = tmp[1] + ofs[1];
            PF(pl, 0xB4) = tmp[2] + ofs[2];
            PF(pl, 0x618) = PF(pl, 0xAC) - old[0];
            PF(pl, 0x61C) = PF(pl, 0xB0) - old[1];
            PF(pl, 0x620) = PF(pl, 0xB4) - old[2];
        }
        if (PB(pl, 0x14) == 2) {
            PB(pl, 0x604) = 0;
            cnt = 1;
            continue;
        }
        if (!(PF(pl, 0x3B8) <= 0.0f) && PB(pl, 0x14) == 0 && PB(pl, 0x15) == 6 && *(s32 *)((u8 *)pl + 0x39C) < 0x15
            && PB(pl, 6) == 0) {
            continue;
        }
        if (!(PF(pl, 0x3B8) <= 0.0f)) {
            a[1] = 230.0f + PF(pl, 0xB0);
            b[1] = a[1] - 60.0f;
        } else {
            a[1] = 30.0f + PF(pl, 0xB0);
            b[1] = a[1] - 60.0f;
        }
        cnt = 0;
        a[0] = PF(pl, 0xAC);
        a[2] = PF(pl, 0xB4);
        b[0] = a[0];
        b[2] = a[2];
        q = em_ride_data[em->kind];
        if (*q != -1.0f) {
            do {
                j = (int)*q;
                q++;
                m = (f32 *)(mats + j * 0x190);
                for (i = 0; i < 4; i++) {
                    v[0] = q[0] * em->scale[0];
                    v[1] = q[1] * em->scale[1];
                    v[2] = q[2] * em->scale[2];
                    q += 3;
                    flvecApplyMat33(w, v, m);
                    quad[i][0] = w[0] + m[12];
                    quad[i][1] = w[1] + m[13];
                    quad[i][2] = w[2] + m[14];
                }
                flvecCopy(tri[0], quad[0]);
                flvecCopy(tri[1], quad[1]);
                flvecCopy(tri[2], quad[2]);
                if (VectorHitCheck(tri[0], a, b, v) == 0) {
                    flvecCopy(tri[0], quad[1]);
                    flvecCopy(tri[1], quad[2]);
                    flvecCopy(tri[2], quad[3]);
                    if (VectorHitCheck(tri[0], a, b, v) == 0) {
                        continue;
                    }
                }
                cnt++;
                if ((PB(pl, 0x604) == 1 || PB(pl, 0x604) == 3) && PB(pl, 0x606) == (j & 0xFF)) {
                    PF(pl, 0xAC) = v[0];
                    PF(pl, 0xB0) = v[1];
                    PF(pl, 0xB4) = v[2];
                } else {
                    PF(pl, 0xAC) = v[0];
                    PF(pl, 0xB0) = v[1];
                    PF(pl, 0xB4) = v[2];
                    PB(pl, 0x605) = PB(em, 0x13);
                    PB(pl, 0x606) = j;
                    if (PB(pl, 0x604) != 1 && PB(pl, 0x604) != 3) {
                        PB(pl, 0x388) = 0;
                        if (!(PF(pl, 0x3B8) <= 0.0f) && PB(pl, 0x14) == 0 && (PB(pl, 0x15) == 6 || PB(pl, 0x15) == 0x15)) {
                            PB(pl, 0x604) = 3;
                            Pl_act_set(pl, 0, 0x11, 0);
                        } else {
                            PB(pl, 0x604) = 1;
                            pl_to_normal(pl, 0, 0, 0);
                        }
                        PB(pl, 0x12) = 0;
                        rate_clear(pl);
                    }
                }
                break;
            } while (*q != -1.0f);
        }
        if (cnt == 0 && PB(pl, 0x604) != 2) {
            PB(pl, 0x604) = 0;
        }
    }
}
