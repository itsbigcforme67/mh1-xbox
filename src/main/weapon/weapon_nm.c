/* weapon_nm: near-matches of f_weapon (not built). trans_pl_sub, Lb_trans_pl and
 * Ed_trans_pl are 10 of 23 instructions off: the original keeps an empty
 * then-block (`beq; nop; nop; b end`) that MWCC here removes; tried if/else,
 * switch and early returns. See trans.c: SLPM_654.95 0x00163AB0 (f_weapon): per-frame display "transfer":
 * a table of up to 64 draw callbacks (TransSet), run by GameTrans, and trans(),
 * the frame's draw order (stage, effects, shells, ordering tables 0-8, text),
 * plus the per-player display callbacks. Names guessed from the code. */
#include "types.h"
#include "pl.h"
#include "game.h"
#include "fl.h"
#include "prim.h"

typedef void (*TRANSFN)(void);

extern u8 ot2[];
extern u8 ot3[];
extern u8 ot4[4];
extern u8 ot5[4];
extern u8 ot6[4];
extern u8 ot7[4];
extern u8 ot8[4];
extern TRANSFN trans_func[0x40];
extern s32 trans_num;
extern s32 trans_end;

void SetFilterMode(int);
void SetTrnslMode(int, int);
void flfntSetHalftype(int);
void trans_stage(void);
void trans_eft_up(void);
void light_set(int);
void trans_shell(void);
void draw_prim(void *);
void trans_set(void);
void trans_eft(void);
void stage_spr_disp(void);
void trans_sprite(void);
void font_draw_stack_no(int);
void font_draw_end(void);
void InitRenderState(int);
void player_trans(PLW *, int);
void Lb_player_trans(PLW *, int);
void Ed_player_trans(PLW *, int);

typedef struct TRANSPL {
    u8 _pad00[0x18];
    s32 pl;             /* 0x18 player number */
} TRANSPL;

void trans_pl_sub(TRANSPL *t) {
    PLW *pl = &player_work[t->pl];

    if (pl->be_flag != 0) {
        if (pl->x01 == 0) {
        } else {
            player_trans(pl, 0);
        }
    }
}

void Lb_trans_pl(TRANSPL *t) {
    PLW *pl = &player_work[t->pl];

    if (pl->be_flag != 0) {
        if (pl->x01 == 0) {
        } else {
            Lb_player_trans(pl, 0);
        }
    }
}

void Ed_trans_pl(TRANSPL *t) {
    PLW *pl = &player_work[t->pl];

    if (pl->be_flag != 0) {
        if (pl->x01 == 0) {
        } else {
            Ed_player_trans(pl, 0);
        }
    }
}
