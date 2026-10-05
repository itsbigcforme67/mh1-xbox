/* hit - SLPM_654.95 0x00111B20-0x001131C0 (first matching run of the
 * file 0x00111B20-0x00114A88; the rest is in hitb.c / hitc.c, the three
 * near-matches in hit_nm.c). Shell (attack) hit checks: every
 * frame each live shell is tested against monsters and players and against
 * other shells. A hit fills the target's damage fields (dm_val per body
 * part, ailment build-up, knock-back direction), plays the hit sound and
 * spawns the hit-mark effects (eft02/06/14/15/16/18). Sharpness (s_gauge_tbl)
 * and monster meat values (em_meat_tbl) scale blade damage.
 * Field names in include/hit.h are mostly by offset. */
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


/* Functions of this file that live in other pieces (hitb.c, hitc.c) or are
 * still asm (hit_hit_sub_em, hit_hit_sub_pl, hit_calc_shl; see hit_nm.c). */
void hit_calc_shl(HSHL *sh, HCHR *pl, HCHR *c);
void hit_hit_sub_em(HSHL *sh, HCHR *pl, HCHR *c, HBODY *sb, HBODY *eb, f32 *pos);
void hit_hit_sub_pl(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos);
void dm_vec_calc(HSHL *sh, HCHR *c, f32 *a, f32 *b);
void hit_data_on_shl_sub(HSHL *sh, HCHR *pl);
void hit_shell_minus(HSHL *sh);

static void hit_chk_init_sub(HCHR *c) {
    s16 i;

    c->x412 = 0;
    for (i = 0; i < 16; i++) {
        c->hit_id[i] = 0;
    }
    c->hit_id_no = 0;
}

void hit_chk_init(void) {
    HCHR *c;
    int i;

    Hit_id_init();
    for (i = 0, c = (HCHR *)player_work; i < 4; i++, c = (HCHR *)((u8 *)c + 0xA00)) {
        hit_chk_init_sub(c);
    }
    for (i = 0, c = (HCHR *)em_work; i < 20; i++, c = (HCHR *)((u8 *)c + 0xA10)) {
        hit_chk_init_sub(c);
    }
}

static void shell_hit_ck(void);

void hit_check(void) {
    HCHR *c;
    int i;
    HCHR *e;
    int k;

    for (i = 0, c = (HCHR *)player_work; i < 4; i++, c = (HCHR *)((u8 *)c + 0xA00)) {
        c->dm_flag = 0;
        c->x7AE = 0;
        c->dm_val[0] = 0;
        c->dm_kind[0] = 0;
        c->dm_val[1] = 0;
        c->dm_kind[1] = 0;
        c->dm_val[2] = 0;
        c->dm_kind[2] = 0;
        c->dm_val[3] = 0;
        c->dm_kind[3] = 0;
        c->dm_val[4] = 0;
        c->dm_kind[4] = 0;
        c->dm_val[5] = 0;
        c->dm_kind[5] = 0;
        c->dm_val[6] = 0;
        c->dm_kind[6] = 0;
        c->dm_val[7] = 0;
        c->dm_kind[7] = 0;
        c->x7B4 = 0;
        c->x7CE = 0;
        c->x7C6 = 0;
        c->x7BC = 0;
    }
    for (k = 0, e = (HCHR *)em_work; k < 20; k++, e = (HCHR *)((u8 *)e + 0xA10)) {
        e->dm_flag = 0;
        e->x7AE = 0;
        e->dm_val[0] = 0;
        e->dm_kind[0] = 0;
        e->dm_val[1] = 0;
        e->dm_kind[1] = 0;
        e->dm_val[2] = 0;
        e->dm_kind[2] = 0;
        e->dm_val[3] = 0;
        e->dm_kind[3] = 0;
        e->dm_val[4] = 0;
        e->dm_kind[4] = 0;
        e->dm_val[5] = 0;
        e->dm_kind[5] = 0;
        e->dm_val[6] = 0;
        e->dm_kind[6] = 0;
        e->dm_val[7] = 0;
        e->dm_kind[7] = 0;
        e->x7B4 = 0;
        e->x7CE = 0;
        e->x7C6 = 0;
        e->x7BC = 0;
    }
    shell_hit_ck();
}

void body_ptr_ck2(HCHR *c, HBODY **pb) {
    HBODY *b = *pb;

    if (c->x948 & b->mask) {
        *pb = &b[b->num + 1];
    } else {
        *pb = &b[1];
    }
}

void body_ptr_ck(HSHL *sh, HBODY **pb) {
    HBODY *b;

    if (sh->x08 == 0xFF || sh->own_em == 0) {
        *pb = &(*pb)[1];
    } else {
        b = *pb;
        if (((HCHR *)*(void **)((u8 *)sh + 0x94))->x948 & b->mask) {
            *pb = &b[b->num + 1];
        } else {
            *pb = &b[1];
        }
    }
}

void dm_value_add(HCHR *c, HSHL *sh) {
    if (sh->ailment & 1) {
        c->x7B4 += sh->ailment_val;
    }
    if (sh->ailment & 2) {
        c->x7BC += sh->ailment_val;
    }
    if (sh->ailment & 4) {
        c->x7C6 += sh->ailment_val;
    }
    if (sh->ailment & 8) {
        c->x7CE += sh->ailment_val;
    }
}

void hit_mark_set_new(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos) {
    if (sh->mark == 0 || sh->stg != game_w.stage) {
    } else {
    if (c->x10 == 1 && c->kind == 0x1D) {
        eft02_set(c, 0, pos);
        func_5508F0(c, 6, 1, pos, 1.0f);
        return;
    }
    if (c->x10 == 0 && pl_flag_ck(c, 0x8000) != 0 && func_639FC0(c->ang_y, c->dm_ang) == 1 && c->x748 >= 0x4B) {
        eft02_set(c, 0, pos);
        func_5508F0(c, 6, 1, pos, 1.0f);
        return;
    }
    switch (sh->mark) {
    case 0x63:
        func_54B7E0(pos, 0, 1.0f);
        break;
    case 0x12:
        func_5547B0(pos, 8, 0);
        break;
    case 1:
    case 6:
    case 7:
    case 8:
    case 0x14:
    case 4:
    case 5:
    case 0x11:
        switch (sh->mark) {
        case 6:
            func_5547B0(pos, 7, 1);
            break;
        case 7:
            func_5547B0(pos, 7, 2);
            break;
        case 8:
            func_5547B0(pos, 7, 3);
            break;
        case 0x11:
            func_5547B0(pos, 7, 5);
            break;
        case 0x14:
            func_5547B0(pos, 7, 6);
            break;
        case 4:
        case 5:
            switch (sh->xCB) {
            case 0:
            case 2:
            case 3:
                if (sh->mark == 4) {
                    eft02_set(c, 7, pos);
                } else {
                    eft02_set(c, 5, pos);
                }
                eft02_set(c, 6, pos);
                break;
            }
        case 1:
            if (pl != 0 && pl->x10 == 0) {
                if (sh->ailment & 0x10) {
                    func_54D480(pos, 0, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 0x20) {
                    func_54D480(pos, 2, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 0x40) {
                    func_54D480(pos, 3, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 0x80) {
                    func_54D480(pos, 8, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 2) {
                    func_54D480(pos, 1, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 4) {
                    func_54D480(pos, 7, c->dm_ang, pl, 1.0f);
                } else if (sh->ailment & 1) {
                    func_54D480(pos, 6, c->dm_ang, pl, 1.0f);
                }
            }
            break;
        }
        switch (sh->xCB) {
        case 1:
            if (sh->x08 != 0xFF) {
                eft02_set(c, 0, pos);
                func_5508F0(c, 6, 1, pos, 1.0f);
            }
            break;
        case 0:
        default:
            func_5508F0(c, 0, 0, pos, 0.8f);
            func_550DD0(c, pos, 2, 0, sh->mark, 0.8f);
            break;
        case 2:
            func_5508F0(c, 0, 2, pos, 0.8f);
            func_550DD0(c, pos, 2, 2, sh->mark, 0.8f);
            break;
        case 3:
            func_5508F0(c, 0, 3, pos, 1.0f);
            func_550DD0(c, pos, 5, 3, sh->mark, 1.0f);
            Eft02_set3(c, 0, 4, 3, pos, 1.0f);
            break;
        }
        break;
    case 9:
    case 0xB:
    case 0xD:
    case 0xF:
        func_5547B0(pos, 7, 0);
    case 0xA:
    case 0xC:
    case 0xE:
    case 0x10:
        switch (sh->mark) {
        case 9:
        case 10:
            if (sh->x08 != 0xFF) {
                eft02_set(c, 0, pos);
                func_5508F0(c, 6, 1, pos, 1.0f);
            }
            break;
        case 15:
        case 16:
            func_5508F0(c, 0, 0, pos, 0.8f);
            func_550DD0(c, pos, 2, 0, sh->mark, 0.8f);
            break;
        case 11:
        case 12:
            func_5508F0(c, 0, 2, pos, 0.8f);
            func_550DD0(c, pos, 2, 2, sh->mark, 0.8f);
            break;
        case 13:
        case 14:
            func_5508F0(c, 0, 3, pos, 1.0f);
            func_550DD0(c, pos, 5, 3, sh->mark, 1.0f);
            Eft02_set5(c, sh->mark, 4, 3, pos, 1.0f);
            break;
        }
        break;
    case 2:
        func_5508F0(c, 8, 0, pos, 1.0f);
        func_5508F0(c, 9, 0, pos, 1.0f);
        break;
    case 3:
        Eft06_set_hit(c, 0, 0);
        break;
    case 0x17:
        if (c->x10 == 0) {
            Eft06_set_hit(c, 0, 1);
        }
        break;
    case 0x15:
        if (c->x10 == 0) {
            Eft06_set_hit(c, 0, 2);
        }
        break;
    case 0x16:
        if (c->x10 == 0) {
            Eft06_set_hit(c, 0, 3);
        }
        break;
    case 0x13:
        Eft06_set_hit(c, 9, 0);
        break;
    }
    }
}

void hit_timer_calc(HCHR *c) {
    if (pl_flag_ck(c, 0x800) != 0) {
        c->x412 = 1;
        pl_flag_clr(c, 0x40000);
    }
    if (c->x40A != 0) {
        return;
    }
    if (pl_flag_ck(c, 1) == 0) {
        c->x412 = 0;
        pl_flag_clr(c, 0x40000);
    }
    switch (c->x412) {
    case 0:
        break;
    case 1:
        if (c->x3D0-- != 0) {
            break;
        }
        c->x3D0 = 0;
        c->x412++;
    case 2:
        if (c->x3D1 == 0) {
            if (c->x3E3 != 0) {
                pl_atck_data_set2(c, c->x3E3);
                pl_flag_clr(c, 0x40000);
                if (c->x3D0 == 0) {
                    c->x412 = 2;
                    c->x3D1--;
                } else {
                    c->x412 = 1;
                }
            } else {
                c->x412 = 0;
            }
        } else {
            c->x3D1--;
        }
        break;
    }
}

#define CHR_OF(sh) ((sh)->own_em == 0 ? (HCHR *)&player_work[(sh)->no] : (HCHR *)&em_work[(sh)->no])

static void hit_shl_shl_ck(HSHL *sh) {
    HBODY *sb;
    HBODY *ob;
    f32 pos[3];
    HCAP cap1;
    HCAP cap2;
    HSPH sph1;
    HSPH sph2;
    HPK pk1;
    HPK pk2;
    HSHL *o = shell_w_top;
    HCHR *own;
    HCHR *oc;
    s16 k1;
    s16 k2;
    s16 r;

    switch (sh->atk_type) {
    case 6:
    case 9:
    case 4:
    case 5:
        return;
    }
    if (sh->own_em == 0) {
        own = (HCHR *)&player_work[sh->no];
    } else {
        own = (HCHR *)&em_work[sh->no];
    }
    for (; o != 0; o = o->next) {
        if (sh->stg != o->stg || o->be_flag == 0) {
            continue;
        }
        if (o->own_em == sh->own_em && sh->x08 != 0xFF && o->x08 != 0xFF) {
            continue;
        }
        if (o->body2 == 0 || o->xB4 != 0) {
            continue;
        }
        if (shell_flag_ck(o, 0x100) != 0 && sh->own_em != 0 && shell_flag_ck(sh, 0x200) == 0) {
            continue;
        }
        sb = sh->body;
        if (o->own_em == 0) {
            oc = (HCHR *)&player_work[o->no];
        } else {
            oc = (HCHR *)&em_work[o->no];
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
                k1 = hit_data_expand(own, sb, &cap1, &sph1);
                break;
            }
            if (k1 != 0) {
                hit_cap_pk(&cap1, &pk1);
            }
            ob = o->body2;
            while (ob->type != -1) {
                switch (ob->type) {
                case 0x7F:
                    k2 = hit_data_expand2(o->pos2, ob, &cap2, &sph2);
                    break;
                case 0x7E:
                    k2 = hit_data_expand3(o->pos2, o->pos0, ob, &cap2);
                    break;
                case 0x7D:
                    body_ptr_ck(o, &ob);
                    continue;
                default:
                    k2 = hit_data_expand(oc, ob, &cap2, &sph2);
                    break;
                }
                if (k2 != 0) {
                    hit_cap_pk(&cap1, &pk1);
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
                    o->xB4 = 1;
                    if (sh->x08 == 0xFF) {
                        o->xC4 = 0;
                    } else if (sh->own_em == 0) {
                        o->xC4 = (HCHR *)&player_work[o->no];
                    } else {
                        o->xC4 = (HCHR *)&em_work[o->no];
                    }
                    goto next;
                }
                ob++;
            }
            sb++;
        }
    next:;
    }
}

static void hit_chk_sub_shl(HSHL *sh, HCHR *pl, HCHR *c);

static void shell_hit_ck(void) {
    HSHL *sh = shell_w_top;
    HCHR *own;
    HCHR *c;
    int i;
    int j;

    if (sh == 0) {
        return;
    }
    do {
        if (sh->be_flag != 0) {
            if (sh->hit_mode == 2) {
                hit_shl_shl_ck(sh);
            }
            if (sh->x08 == 0xFF) {
                own = 0;
            } else if (sh->own_em == 0) {
                own = (HCHR *)&player_work[sh->no];
            } else {
                own = (HCHR *)&em_work[sh->no];
            }
            sh->x1E = 0;
            for (i = 0, c = (HCHR *)em_work; i < 20; i++, c = (HCHR *)((u8 *)c + 0xA10)) {
                if (c->be_flag == 0 || c->stg != sh->stg) {
                    continue;
                }
                if (sh->x08 != 0xFF && own == c && shell_flag_ck(sh, 4) == 0) {
                    continue;
                }
                for (j = 0; j < 16; j++) {
                    if (sh->hit_id == c->hit_id[j]) {
                        goto skip_em;
                    }
                }
                if (c->dm_flag != 0) {
                    continue;
                }
                if (sh->atk_type == 6 || sh->atk_type == 9) {
                    if (c->x388 == 2 || c->x302 <= 0) {
                        continue;
                    }
                }
                hit_chk_sub_shl(sh, own, c);
            skip_em:;
            }
            for (c = (HCHR *)player_work, i = 0; i < game_w.pl_num; i++, c = (HCHR *)((u8 *)c + 0xA00)) {
                if (c->be_flag == 0 || c->stg != sh->stg || shell_flag_ck(sh, 0x400) != 0) {
                    continue;
                }
                if (sh->x08 != 0xFF && own == c && shell_flag_ck(sh, 4) == 0) {
                    continue;
                }
                for (j = 0; j < 16; j++) {
                    if (sh->hit_id == c->hit_id[j]) {
                        goto skip_pl;
                    }
                }
                if (c->dm_flag != 0) {
                    continue;
                }
                if (sh->atk_type != 6 && sh->atk_type != 9) {
                    hit_chk_sub_shl(sh, own, c);
                }
            skip_pl:;
            }
        }
        sh = sh->next;
    } while (sh != 0);
}

void hit_calc_shl(HSHL *sh, HCHR *pl, HCHR *c);

static void hit_chk_sub_shl(HSHL *sh, HCHR *pl, HCHR *c) {
    if (c->be_flag == 0) {
        return;
    }
    if (sh->x08 != 0xFF && pl->x40A != 0) {
        return;
    }
    if (sh->x7B == 0 && Pl_stg_ck(c) != 0) {
        switch (sh->hit_mode) {
        case 2:
            hit_calc_shl(sh, pl, c);
            break;
        }
    }
}

u8 dm_skip_ck(HCHR *c) {
    if (softdip_ck(0x2A) != 0) {
        return 0;
    }
    if (c->x10 != 0 && c->x8C3 != 0) {
        return 1;
    }
    return 0;
}

void hit_data_on_shl_sub(HSHL *sh, HCHR *pl);
void hit_shell_minus(HSHL *sh);

