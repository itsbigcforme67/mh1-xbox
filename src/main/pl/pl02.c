/* Player code (SLPM_654.95 0x00134F30-0x00135078): Pl_ofs_set, skill_hp_calc_00134FF0. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void Pl_ofs_set(PLW *pl, f32 *pos, int ang) {
    s32 rot[3];
    f32 ofs[4];
    f32 out[4];
    f32 mat[16];
    rot[1] = ang & 0xFFFF;
    rot[0] = 0;
    rot[2] = 0;
    cpRotMatrix(rot, mat);
    ofs[0] = start_ofs[pl->id * 2];
    ofs[1] = 0;
    ofs[2] = start_ofs[pl->id * 2 + 1];
    flvecApplyMat33(out, ofs, mat);
    pos[0] += out[0];
    pos[2] += out[2];
}

int skill_hp_calc_00134FF0(PLW *pl) {
    if (Pl_Skill_ck(pl, 0x22) == 1) return 10;
    if (Pl_Skill_ck(pl, 0x23) == 1) return 20;
    if (Pl_Skill_ck(pl, 0x24) == 1) return 30;
    return 0;
}
