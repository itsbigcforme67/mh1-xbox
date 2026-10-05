/* trans - SLPM_654.95 0x00163AB0 (f_weapon): per-frame display "transfer":
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

void TransReset(void) {
    s32 i;

    trans_num = 0;
    for (i = 0; i < 0x40; i++) {
        trans_func[i] = 0;
    }
}

void TransSet(TRANSFN fn) {
    if (trans_end != 0) {
        TransReset();
    }
    if (trans_num != 0x40) {
        trans_func[trans_num] = fn;
        trans_num++;
    }
}

void GameTrans(void) {
    TRANSFN *f;
    s32 n;

    n = trans_num;
    f = trans_func;
    if (n != 0) {
        do {
            (*f)();
            n--;
            f++;
        } while (n != 0);
    }
}

void trans(void) {
    SetFilterMode(0);
    SetTrnslMode(4, 5);
    flSetRenderState(0x60, 0);
    flSetRenderState(0x6C, 1);
    flfntSetHalftype(1);
    trans_stage();
    trans_eft_up();
    GameTrans();
    light_set(1);
    trans_shell();
    draw_prim(ot1);
    draw_prim(ot3);
    draw_prim(ot0);
    light_set(1);
    trans_set();
    trans_eft();
    draw_prim(ot4);
    if (game_w.mode == 2 || game_w.mode == 3) {
        stage_spr_disp();
    }
    flSetRenderState(0x6D, 7);
    trans_sprite();
    flSetRenderState(0x6D, 3);
    draw_prim(ot5);
    font_draw_stack_no(0);
    draw_prim(ot6);
    font_draw_stack_no(1);
    draw_prim(ot7);
    font_draw_stack_no(2);
    draw_prim(ot8);
    font_draw_stack_no(4);
    draw_prim(ot2);
    font_draw_stack_no(3);
    font_draw_end();
    InitRenderState(1);
}
