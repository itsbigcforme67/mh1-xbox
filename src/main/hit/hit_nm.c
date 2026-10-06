/* hit_nm - NOT BUILT. The whole hit file as C. Not yet matching:
 * hit_hit_sub_em (112 instructions off), hit_hit_sub_pl (now linked from hite.c; was 2: one add.s
 * operand order), hit_calc_shl (now linked from hitd.c). The rest matches and is
 * built from hit.c / hitb.c / hitc.c.
 * hit - SLPM_654.95 0x00111B20-0x00114A88. Shell (attack) hit checks: every
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

static void dm_value_add(HCHR *c, HSHL *sh) {
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

static void hit_mark_set_new(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos) {
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

static void hit_calc_shl(HSHL *sh, HCHR *pl, HCHR *c);

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

static u8 dm_skip_ck(HCHR *c) {
    if (softdip_ck(0x2A) != 0) {
        return 0;
    }
    if (c->x10 != 0 && c->x8C3 != 0) {
        return 1;
    }
    return 0;
}

static void hit_data_on_shl_sub(HSHL *sh, HCHR *pl);
static void hit_shell_minus(HSHL *sh);

static void hit_hit_sub_em(HSHL *sh, HCHR *pl, HCHR *c, HBODY *sb, HBODY *eb, f32 *pos) {
    f32 sharp = 1.0f;
    f32 rate = sharp;
    f32 poison = 0.0f;
    f32 sleep = poison;
    f32 stun = poison;
    f32 para = poison;
    EM_MEAT *meat;
    u8 v;
    f32 a;
    f32 b;
    f32 pw;
    int dmg;

    sh->xCB = 0;
    meat = &em_meat_tbl[c->kind][eb->num];
    if ((sh->hit_cnt & 0xF) == 0) {
        sh->mode = 3;
        sh->mode2 = 0;
        sh->x1E = 1;
    } else {
        hit_shell_minus(sh);
    }
    sh->hit_chr = c;
    sh->hit_body = eb;
    if (sh->x08 != 0xFF && pl != 0) {
        hit_data_on_shl_sub(sh, pl);
        pl->x19 = eb->num + 1;
        if (shell_flag_ck(sh, 0x20) != 0) {
            pl->x409 = sh->x70;
            pl->x7A0 = c;
            pl->x7A4 = sh;
        }
        if (pl->x10 != 0) {
            Pl_se_req2_com(c, 0x60, 0, pos, 1, 0);
        } else {
            if (sh->ailment & 0x10) {
                poison = sh->ailment_val;
            }
            if (sh->ailment & 0x20) {
                para = sh->ailment_val;
            }
            if (sh->ailment & 0x40) {
                sleep = sh->ailment_val;
            }
            if (sh->ailment & 0x80) {
                stun = sh->ailment_val;
            }
            if (shell_flag_ck(sh, 0x40) != 0) {
                if (sh->x75 == 0xFF) {
                    v = 0;
                } else if (sh->x75 != 0) {
                    v = 1;
                } else if (sh->x76 != 0) {
                    if (sb->num != 1) {
                        v = 2;
                    } else {
                        v = 3;
                    }
                } else {
                    v = 0;
                }
                rate = pl->atk_rate;
                sharp = s_gauge_tbl[pl->x887 * 4 + v];
                sharp *= slash_adj_tbl[pl->x887];
                a = pl->x7D4 / 100.0f;
                b = pl->x7D5 / 100.0f;
                a = a * (meat->v[1] / 100.0f);
                b = b * (meat->v[2] / 100.0f);
                if (a >= b) {
                    sharp = sharp * a;
                } else {
                    sharp = sharp * b;
                }
                if (sharp >= 1.0f) {
                    pl->x18 = 3;
                    pl->x409 = sh->x70 * 3;
                } else if (sharp >= 0.45f) {
                    pl->x18 = 2;
                    pl->x409 = 0;
                    pl->x610 = sh->x70;
                } else {
                    if (sharp >= 0.25f) {
                        pl->x18 = 0;
                    } else {
                        pl->x18 = 1;
                    }
                    pl->x409 = 0;
                }
                sh->xCB = pl->x18;
                Pl_slash_calc(pl, slash_minus_tbl[pl->x18]);
                switch (sh->se) {
                case 0xFF:
                    break;
                case 0:
                default:
                    Pl_se_req2_com(c, hit_se_tbl[pl->kind * 8 + pl->x18 * 2], 0, pos, 1, 0);
                    if (hit_se_tbl[pl->kind * 8 + pl->x18 * 2 + 1] != 0) {
                        Pl_se_req2_com(c, hit_se_tbl[pl->kind * 8 + pl->x18 * 2 + 1], 0, pos, 1, 0);
                    }
                    break;
                case 1:
                    Pl_se_req2_com(c, 0x20, 0, pos, 1, 0);
                    break;
                }
            } else {
                sharp = 1.0f;
                if (shell_flag_ck(sh, 8) != 0) {
                    sharp = sharp * (meat->v[3] / 100.0f);
                    if (softdip_ck(0xAE) != 0) {
                        printf(lit_951_00358250);
                        printf(lit_952_00358270, fptodp(1.0f));
                        printf(lit_953_00358290, fptodp(sharp));
                    }
                } else if (softdip_ck(0xAE) != 0) {
                    printf(lit_954_003582B0);
                }
                switch (sh->se) {
                case 0xFF:
                    break;
                case 2:
                case 3:
                    Pl_se_req2_com(c, 0x5D, 0, pos, 1, 0);
                    break;
                case 4:
                default:
                    Pl_se_req2_com(c, 0x5C, 0, pos, 1, 0);
                    break;
                case 5:
                    Pl_se_req2_com(c, 0x63, 0, pos, 1, 0);
                    break;
                case 6:
                    Pl_se_req2_com(c, 0x4D, 0, pos, 1, 0);
                    break;
                case 1:
                    Pl_se_req2_com(c, 0x20, 0, pos, 1, 0);
                    break;
                }
            }
            poison *= meat->v[4] / 100.0f;
            para *= meat->v[5] / 100.0f;
            sleep *= meat->v[6] / 100.0f;
            stun *= meat->v[7] / 100.0f;
            poison = poison + para + sleep + stun;
            vib_set_pl(pl, 0);
        }
    }
    if (sh->atk_type == 7 && c->x56A != 0xFF && c->x8C3 == 0) {
        c->x56A = 1;
        c->x572 = 0x4650;
    }
    c->hit_id[c->hit_id_no] = sh->hit_id;
    c->hit_id_no = (c->hit_id_no + 1) & 0xF;
    if (dm_skip_ck(c) != 0) {
        return;
    }
    pw = sh->pow;
    switch (c->x762) {
    case 2:
        pw *= 3.0f;
        break;
    case 3:
        pw *= 2.0f;
        break;
    case 4:
        pw *= 2.0f;
        break;
    case 0:
    case 1:
        break;
    }
    if (sh->atk_type == 8) {
        dmg = (s16)(-1.0f * sh->pow);
    } else {
        dmg = (s16)(rate * (pw * sharp));
    }
    if (sh->own_em != 0 && c->kind != 0x1D) {
        dmg = dmg / 4;
    }
    if (dmg == 0 && sh->pow != 0) {
        dmg = 1;
    }
    c->dm_kind[eb->part & 7] |= sh->x69;
    dmg += (s32)poison;
    c->dm_val[eb->part & 7] += dmg;
    dm_value_add(c, sh);
    c->x7AE += sh->x74;
    if (sh->x08 != 0xFF && pl->x10 == 0) {
        c->dm_pl[pl->id & 3] += dmg;
    }
    c->x409 = sh->x70;
    c->dm_flag = 2;
    c->dm_part = eb->part;
    if (shell_flag_ck(sh, 0x20) == 0) {
        pl_flag_set(c, 0x80000001);
    } else {
        pl_flag_clr(c, 0x80000001);
    }
    c->dm_shl = sh;
    c->dm_type = sh->atk_type;
    c->dm_pow = sh->x68;
}

static void hit_hit_sub_pl(HSHL *sh, HCHR *pl, HCHR *c, f32 *pos) {
    f32 rate;
    f32 pw;
    f32 def;
    f32 res;

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
            pw = pw - pw * def / (80.0f + def);
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

static void dm_vec_calc(HSHL *sh, HCHR *c, f32 *a, f32 *b) {
    if (sh->x65 & 0x10) {
        c->dm_ang = calc_vec_ang2(a, b) + 0x4000;
    } else {
        c->dm_ang = sh->ang + (u16)(s32)(0.5f + 65536.0f * sh->x66 / 360.0f);
    }
}

static void hit_calc_shl(HSHL *sh, HCHR *pl, HCHR *c) {
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

static void hit_data_on_shl_sub(HSHL *sh, HCHR *pl) {
    pl->x41C = &sh->hit_time;
}

static void hit_shell_minus(HSHL *sh) {
    s8 n = (s8)(sh->hit_cnt & 0xF) - 1;

    sh->hit_cnt = (sh->hit_cnt & 0xF0) | n;
    if (sh->hit_cnt >> 4 != 0) {
        sh->hit_time = sh->hit_cnt >> 4;
        sh->hit_mode = 1;
        sh->hit_id = Get_hit_id();
    }
}

static void hit_timer_calc_shl_sub(HSHL *sh) {
    switch (sh->hit_mode) {
    case 0:
        break;
    case 1:
        if (sh->hit_time-- != 0) {
            break;
        }
        sh->hit_time = 0;
        sh->hit_mode++;
    case 2:
        if (sh->hit_wait == 0) {
            if (sh->x73 != 0) {
                pl_atck_data_set_shl2(sh, &player_work[sh->no], sh->x73);
                sh->hit_mode = 1;
                if (sh->hit_time == 0) {
                    sh->hit_mode = 2;
                    sh->hit_wait--;
                } else {
                    sh->hit_mode = 1;
                    break;
                }
            } else {
                sh->hit_mode = 0;
                sh->hit_time = 0;
                sh->hit_wait = 0;
                break;
            }
        } else {
            sh->hit_wait--;
        }
        if (sh->x75 != 0 && sh->x75 != 0xFF) {
            sh->x75--;
        } else if (sh->x76 != 0) {
            sh->x76--;
        }
        break;
    }
}

void hit_timer_calc_shl(void) {
    HSHL *sh = shell_w_top;

    if (sh != 0) {
        do {
            if (sh->be_flag != 0 && sh->x7B == 0) {
                hit_timer_calc_shl_sub(sh);
            }
            sh = sh->next;
        } while (sh != 0);
    }
}
