/* hitd - SLPM_654.95 0x00114320-0x00114878: hit_calc_shl, one shell against one character: tests every body part of the
   shell (sh->body) against the character's hit capsules / monster body table (D_63FA10) and, on the first hit, runs
   dm_vec_calc, hit_hit_sub_pl / hit_hit_sub_em and hit_mark_set_new. Early returns, not a nested if (that changes
   where the compiler puts the shared exit). See hit_nm.c for the rest of the file. */
#include "hit.h"
#include "pl.h"
#include "em.h"
#include "game.h"

typedef struct EM_MEAT {
    u8 v[8];            /* 1/2 cut/blunt %, 3 shot %, 4-7 ailment % */
} EM_MEAT;

extern EM_MEAT *em_meat_tbl[];
extern f32 s_gauge_tbl[];
extern f32 slash_adj_tbl[];
extern s16 slash_minus_tbl[4];
extern s16 hit_se_tbl[];
extern HBODY *D_63FA10[];     /* game.bin: monster body tables by kind */
extern HSHL *shell_w_top;
extern char lit_951_00358250[];
extern char lit_952_00358270[];
extern char lit_953_00358290[];
extern char lit_954_003582B0[];

void Hit_id_init(void);
u8 Get_hit_id(void);
int pl_flag_ck(void *, int);
void pl_flag_set(void *, int);
void pl_flag_clr(void *, int);
void pl_atck_data_set2(void *, int);
void pl_atck_data_set_shl2(HSHL *, void *, int);
int shell_flag_ck(HSHL *, int);
int softdip_ck(int);
u8 Pl_stg_ck(void *);
int Pl_master_ck(void *);
void Pl_se_req2_com(void *, int, int, f32 *, int, int);
void Pl_slash_calc(void *, int);
void vib_set_pl(void *, int);
void printf(char *, ...);
int fptodp(f32);
void pl_body_make(void *, HCAP *, f32);
void hit_cap_pk(HCAP *, HPK *);
s16 hit_data_expand(void *, HBODY *, HCAP *, HSPH *);
s16 hit_data_expand2(f32 *, HBODY *, HCAP *, HSPH *);
s16 hit_data_expand3(f32 *, f32 *, HBODY *, HCAP *);
u8 hit_cap_cap2_m(HPK *, HPK *, f32 *);
u8 hit_cap_sphr2_m(HPK *, HSPH *, f32 *, f32);
u8 hit_sphr_cap_m(HSPH *, HPK *, f32 *, f32);
u8 hit_sphr_sphr2(HSPH *, HSPH *, f32 *, f32, f32);
u16 calc_vec_ang2(f32 *, f32 *);
void body_ptr_ck(HSHL *sh, HBODY **pb);
void eft02_set(void *, int, f32 *);
void Eft02_set3(void *, int, int, int, f32 *, f32);
void Eft02_set5(void *, s16, int, int, f32 *, f32);
void Eft06_set_hit(void *, s16, int);
void func_54B7E0(f32 *, s16, f32);              /* game.bin eft14_set */
void func_54D480(f32 *, int, int, void *, f32); /* game.bin Eft15_set */
void func_5508F0(void *, int, int, f32 *, f32); /* game.bin Eft16_set */
void func_550DD0(void *, f32 *, int, int, int, f32); /* game.bin Eft16_set_impact */
void func_5547B0(f32 *, s16, int);              /* game.bin Eft18_set2 */
int func_639FC0(u16, u16);                      /* game.bin Guard_dir_ck */

void dm_vec_calc(HSHL *sh, HCHR *c, f32 *a, f32 *b);
void hit_mark_set_new(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos);
void hit_hit_sub_pl(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos);
void hit_hit_sub_em(HSHL *sh, HCHR *pl, HCHR *c, HBODY *sb, HBODY *eb, f32 *pos);
void body_ptr_ck2(HCHR *c, HBODY **pb);

void hit_calc_shl(HSHL *sh, HCHR *pl, HCHR *c) {
    HBODY *sb;
    HBODY *eb;
    f32 pos[3];
    HCAP cap1;
    HCAP cap2;
    HSPH sph1;
    HSPH sph2;
    HPK pk1;
    HPK pk2;
    s16 k1;
    s16 k2;
    s16 r;

    if (c->x40C != 0) return;
    if (sh->body == 0) return;
    {
        sb = sh->body;
        if (c->x10 == 0) {
            if (c->mode == 3) {
                if (sh->atk_type != 0xA) {
                    return;
                }
            } else if (sh->atk_type == 0xA) {
                return;
            }
            pl_body_make(c, &cap2, 40.0f);
            hit_cap_pk(&cap2, &pk2);
        }
        while (sb->type != -1) {
            switch (sb->type) {
            case 0x7F:
                k1 = hit_data_expand2(sh->pos2, sb, &cap1, &sph1);
                break;
            case 0x7E:
                k1 = hit_data_expand3(sh->pos2, sh->pos0, sb, &cap1);
                break;
            case 0x7D:
                body_ptr_ck(sh, &sb);
                continue;
            default:
                k1 = hit_data_expand(pl, sb, &cap1, &sph1);
                break;
            }
            if (k1 != 0) {
                hit_cap_pk(&cap1, &pk1);
            }
            if (c->x10 == 0) {
                if (k1 == 1) {
                    r = hit_cap_cap2_m(&pk1, &pk2, pos);
                } else {
                    r = hit_sphr_cap_m(&sph1, &pk2, pos, sph1.r);
                }
                if (r != 0) {
                    if (k1 == 1) {
                        dm_vec_calc(sh, c, pk2.c, pk1.c);
                    } else {
                        dm_vec_calc(sh, c, pk2.c, sph1.c);
                    }
                    hit_hit_sub_pl(sh, pl, c, pos);
                    hit_mark_set_new(sh, pl, c, pos);
                    return;
                }
            } else {
                eb = D_63FA10[c->kind];
                while (eb->type != -1) {
                    switch (eb->type) {
                    case 0x7F:
                    case 0x7E:
                        k2 = hit_data_expand2(c->pos, eb, &cap2, &sph2);
                        break;
                    case 0x7D:
                        body_ptr_ck2(c, &eb);
                        continue;
                    default:
                        k2 = hit_data_expand(c, eb, &cap2, &sph2);
                        break;
                    }
                    if (sh->atk_type == 6 || sh->atk_type == 9) {
                        if (!(eb->flag & 1) || c->x8C3 != 0) {
                            goto next_eb;
                        }
                    }
                    if (sh->atk_type == 5 && !(eb->flag & 2)) {
                        goto next_eb;
                    }
                    if (k2 != 0) {
                        hit_cap_pk(&cap2, &pk2);
                    }
                    if (k1 == 1) {
                        if (k2 == 1) {
                            r = hit_cap_cap2_m(&pk1, &pk2, pos);
                        } else {
                            r = hit_cap_sphr2_m(&pk1, &sph2, pos, sph2.r);
                        }
                    } else if (k2 == 1) {
                        r = hit_sphr_cap_m(&sph1, &pk2, pos, sph1.r);
                    } else {
                        r = hit_sphr_sphr2(&sph1, &sph2, pos, sph1.r, sph2.r);
                    }
                    if (r != 0) {
                        if (k1 == 1) {
                            if (k2 == 1) {
                                dm_vec_calc(sh, c, pk2.c, pk1.c);
                            } else {
                                dm_vec_calc(sh, c, sph2.c, pk1.c);
                            }
                        } else if (k2 == 1) {
                            dm_vec_calc(sh, c, pk2.c, sph1.c);
                        } else {
                            dm_vec_calc(sh, c, sph2.c, sph1.c);
                        }
                        hit_hit_sub_em(sh, pl, c, sb, eb, pos);
                        hit_mark_set_new(sh, pl, c, pos);
                        return;
                    }
                next_eb:
                    eb++;
                }
            }
            sb++;
        }
    }
}
