/* hite - SLPM_654.95 0x00113E50-0x00114280: hit_hit_sub_pl (a shell hits a player: sets the shell state, adds the damage scaled by the
 * defence value (pw - pw * def / (80 + def)) and the elemental resistance, records the hit position and plays the hit sound). The rest of
 * the hit file is hit.c / hitb.c / hitc.c / hitd.c; the full near-match C is hit_nm.c. */
#include "hit.h"
#include "pl.h"
#include "em.h"
#include "game.h"

int func_639FC0(u16, u16);                      /* game.bin Guard_dir_ck */
int pl_flag_ck(void *, int);
int shell_flag_ck(HSHL *, int);
int Pl_master_ck(void *);
void Pl_se_req2_com(void *, int, int, f32 *, int, int);
void hit_shell_minus(HSHL *sh);
void hit_data_on_shl_sub(HSHL *sh, HCHR *pl);
u8 dm_skip_ck(HCHR *c);
void dm_value_add(HCHR *c, HSHL *sh);

void hit_hit_sub_pl(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos) {
    f32 rate;
    f32 pw;
    f32 def;
    f32 res;
    f32 k;

    if ((sh->hit_cnt & 0xF) == 0) {
        sh->mode = 3;
        sh->mode2 = 0;
        sh->x1E = 1;
    } else {
        hit_shell_minus(sh);
    }
    sh->hit_chr = c;
    sh->hit_body = 0;
    sh->xCB = 0;
    rate = 1.0f;
    if (sh->x08 != 0xFF) {
        hit_data_on_shl_sub(sh, pl);
        pl->x19 = 1;
        if (shell_flag_ck(sh, 0x20) != 0) {
            pl->x409 = sh->x70;
        }
        pl->x18 = 0;
        pl->x7A0 = c;
        pl->x7A4 = sh;
        rate = pl->atk_rate;
    }
    c->hit_id[c->hit_id_no] = sh->hit_id;
    c->hit_id_no = (c->hit_id_no + 1) & 0xF;
    c->dm_kind[0] |= sh->x69;
    if (dm_skip_ck(c) != 0) {
        return;
    }
    if (Pl_master_ck(c) == 1) {
        switch (sh->atk_type) {
        case 11:
            c->x6A4 = 3;
            break;
        case 12:
            c->x6A8 = 3;
            break;
        case 13:
            c->x7BA = 0;
            break;
        }
    }
    if (sh->own_em != 0 || sh->x08 == 0xFF || sh->atk_type == 8) {
        dm_value_add(c, sh);
        if (sh->atk_type == 8) {
            c->dm_val[0] -= sh->pow;
        } else {
            pw = sh->pow * rate;
            def = c->x7DC;
            k = 80.0f;
            pw = pw - pw * def / (def + k);
            res = 1.0f;
            if (sh->x69 & 4) {
                res = (100.0f - c->resist[0]) / 100.0f;
            } else if (sh->x69 & 0x10) {
                res = (100.0f - c->resist[1]) / 100.0f;
            } else if (sh->x69 & 0x40) {
                res = (100.0f - c->resist[2]) / 100.0f;
            } else if (sh->x69 & 0x20) {
                res = (100.0f - c->resist[3]) / 100.0f;
            }
            c->dm_val[0] += (s16)(pw * res);
            c->x7AE += sh->x74;
        }
    }
    c->x3B0 = pl;
    c->dm_pos[0] = pos[0];
    c->dm_pos[1] = pos[1];
    c->dm_pos[2] = pos[2];
    c->dm_flag = 2;
    if (!(pl_flag_ck(c, 0x8000) != 0 && func_639FC0(c->ang_y, c->dm_ang) != 0 && c->x748 >= 0x4B)) {
        switch (sh->se) {
        case 0xFF:
            break;
        case 0:
        default:
            Pl_se_req2_com(c, 0x21, 0, pos, 1, 0);
            break;
        case 1:
            Pl_se_req2_com(c, 0x20, 0, pos, 1, 0);
            break;
        }
    }
    c->dm_part = 0;
    c->dm_shl = sh;
    c->dm_type = sh->atk_type;
    c->dm_pow = sh->x68;
}
