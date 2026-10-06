/* lb_vs02 - lobby.bin 0x005C31A0-0x005C34F8: lb_npc_item_trans, the village NPC
 * "carried item" draw: for the NPC motions 0x2AB-0x2AD (kind 0, work byte
 * 0xE == 3) it draws the item model (clay set 0x21E8 into the set-model work
 * at D_3C8DC0) at two joints (0x12, 0xE) with fixed offsets/rotations, then
 * calls the player version. Which NPC/item this is: not identified (guess). */
#include "lobby.h"
#include "em.h"
#include "fl.h"
#include "clay.h"

typedef struct LBSETMDL {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    u8 *mat;            /* 0x10 material table (0x4C each) */
    u8 _pad14[0x1C];
    u8 *clay;           /* 0x30 clay array base */
} LBSETMDL;
int em_frame_check2(EMW *, f32, int);
void *get_joint_wmat_em();
void flmatCopy();
void flmatGetTrans();
void flvecApplyMat33_2();
void clay_attr_set();
void clay_attr_reset();
void flExecuteClay();
void lb_pl_item_trans();

void lb_npc_item_trans(EMW *em) {
    CLAY *c;
    u8 *mat;
    s16 n;
    LBSETMDL *mw = *(LBSETMDL **)0x3C8DC0;
    s16 i;
    FLMAT m;
    FLMAT jm;
    f32 p[3];
    f32 off[2][3];
    f32 rot[2][3];
    s16 jn[2];
    int k;
    u8 *ex = (u8 *)em + 0x444;

    if (mw != 0 && mw->flag != 0) {
        mat = mw->mat;
        n = 0;
        switch (em->kind) {
        case 0:
            switch (ex[0xE]) {
            case 3:
                switch (em->char0) {
                case 0x2AD:
                    if (em_frame_check2(em, 80.0f, 0) != 0) {
                        return;
                    }
                case 0x2AB:
                case 0x2AC:
                    c = (CLAY *)(mw->clay + 0x21E8);
                    jn[0] = 0x12;
                    rot[0][0] = 2.5132742f;
                    rot[0][1] = -0.48869222f;
                    rot[0][2] = 0.017453294f;
                    off[0][0] = -9.4f;
                    off[0][1] = -7.4f;
                    off[0][2] = -3.0f;
                    jn[1] = 0xE;
                    rot[1][0] = 1.186824f;
                    rot[1][1] = 0.5585054f;
                    rot[1][2] = -2.0245821f;
                    off[1][0] = 11.0f;
                    off[1][1] = -6.4f;
                    off[1][2] = -1.6f;
                    n += 2;
                    break;
                }
            }
        }
        for (i = 0; i < n; i++) {
            if (c != 0 && c->handle != -1) {
                flmatCopy(jm, get_joint_wmat_em(em, jn[i]));
                flmatGetTrans(p, &jm);
                flvecApplyMat33_2(off[i], &jm);
                p[0] += off[i][0];
                p[1] += off[i][1];
                p[2] += off[i][2];
                flmatInit(&m);
                flmatRotXYZ33(&m, rot[i][0], rot[i][1], rot[i][2]);
                flmatMul33_2(&m, &jm);
                flmatSetTrans(&m, p[0], p[1], p[2]);
                flSetRenderState(0x1A, (u32)m);
                flSetRenderState(0x67, -1);
                for (k = 0; k < c->mat_num; k++) {
                    u8 *mm = mat + c->mat_no[k] * 0x4C;
                    *(f32 *)(mm + 0x10) = em->x798;
                    flSetRenderState((k + 0x3A) & 0xFF, (u32)mm);
                }
                clay_attr_set(c->attr);
                flExecuteClay(c->handle, 0);
            }
        }
        clay_attr_reset();
        lb_pl_item_trans(em);
    }
}
