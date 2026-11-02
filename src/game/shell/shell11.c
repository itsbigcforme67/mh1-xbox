/* shell11 - game.bin 0x00634040-0x00634458. Generated from the shell18
 * template, then edited by hand (draw callback, owner-alive check). */
#include "shell.h"
#include "prim.h"

extern u8 shell11_tbl[8];
extern s32 shell11_body_tbl[];

u8 Em_stg_ck(EMW *);
void pl_atck_data_set_shl(SHLW *, EMW *, int, u8 *);
f32 flAbs(f32);
void flvecCopy(void *, void *);
void release_prim(s16);

static void shell11_move(SHLW *sh);
static void shell11_i(SHLW *sh);
static void shell11_m(SHLW *sh);
static void shell11_d(SHLW *sh);
static void shell11_e(SHLW *sh);
static void shell11_trans(SHLW *sh);

void Shell11_set(EMW *em, int arg) {
    SHLW *sh;

    if (Em_stg_ck(em) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 11;
            sh->arg = arg;
            sh->move = shell11_move;
            sh->em_no = em->id;
            sh->x7A = em->x10;
            sh->char0 = em->char0;
            sh->owner = em;
            sh->xC8 = em->ang[1];
            sh->ang[0] = em->ang[0];
            sh->ang[1] = em->ang[1];
            sh->ang[2] = em->ang[2];
            MH2_SHELL_END1(sh, em)
            em->x19 = 0;
            MH2_SHELL_END2(em)
        }
    }
}

static void shell11_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell11_i(sh);
        break;
    case 1:
        shell11_m(sh);
        break;
    case 2:
        shell11_d(sh);
        break;
    case 3:
        shell11_m(sh);
        break;
    case 4:
        shell11_d(sh);
        break;
    case 5:
        shell11_d(sh);
        break;
    case 7:
        shell11_d(sh);
        break;
    case 6:
        shell11_e(sh);
        break;
    }
}

/* G: original bytes (build/mhg/raw/shell11_i.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
static asm void shell11_i(SHLW *sh)
{
#include "shell11_i.inc"
}
#else
static void shell11_i(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];
    s32 a;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = shell11_trans;
    shell_flag_set(sh, 0x20);
    sh->xB4 = 0;
#ifdef VERSION_MHG
    /* G (game 0x666BE0): attack entry arg, not arg + 1 (G's shell11_tbl list has no leading entry);
       MH1's +1 gave every G swing (Aptonoth, kind 40) the next kind's attack data, often none */
    pl_atck_data_set_shl(sh, em, sh->arg, shell11_tbl);
#else
    pl_atck_data_set_shl(sh, em, sh->arg + 1, shell11_tbl);
#endif
    sh->x88 = shell11_body_tbl[sh->body];
    sh->x8C = 0;
    sh->stg = em->stg;
    if (sh->x60 != 0) {
        a = flAbs(em->blend0 % 100);
        sh->x60 += (u8)(a - (s32)flAbs(em->act_tm0) - 1);
        if ((s8)sh->x60 < 0) {
            sh->x60 = 0;
        }
    }
    sh->prim = 0;
}
#endif

static void shell11_m(SHLW *sh) {
    EMW *em = &em_work[sh->em_no];

    switch (sh->arg) {
    default:
        break;
    case 0x4:
    case 0xA:
        sh->x61 = 99;
        break;
    }
    if (em->be_flag == 0) {
        sh->xB = 0;
        sh->mode = 2;
        return;
    }
    if (em->char0 != sh->char0 || sh->xB == 0) {
        sh->xB = 0;
        sh->mode = 2;
    }
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell11_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell11_e(SHLW *sh) {
    push_shell_work(sh);
}

static void shell11_trans(SHLW *sh) {
}
