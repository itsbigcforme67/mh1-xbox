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

/* weapon_joint_calc (0x164410, 2376 bytes): complete, ~440 of 594 instructions
 * differ by layout only: the jump table for PLW.kind matches; the original
 * fills some branch delay slots from the fall-through path (li of the next
 * compare constant) which this build does not. `if (f) {} else {r = 1}` is
 * written as a switch with an empty default because that gave the original
 * block layout (beq; nop; b end; nop; b end/li). */
s32 frame_check2(f32, PLW *, int);

#define W1C4(pl) (*(s32 *)((u8 *)(pl) + 0x1C4))

s16 weapon_joint_calc(PLW *pl) {
    s16 r = 2;

    switch (pl->kind) {
    case 2:
        switch (pl->char0) {
        case 0x3EA:
            if (frame_check2(6.0f, pl, 0) != 0) {
                r = 1;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3EB:
            switch (frame_check2(48.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F1:
            switch (frame_check2(50.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F2:
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 1;
                break;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    case 3:
        switch (pl->char0) {
        case 0x3FA:
        case 0x3EA:
            r = 2;
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 1;
                break;
            }
            break;
        case 0x3EB:
            switch (frame_check2(60.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F1:
            switch (frame_check2(66.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F2:
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 1;
                break;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    case 4:
        switch (pl->char0) {
        case 0x3F2:
        case 0x3FA:
        case 0x3EA:
            r = 2;
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 1;
                break;
            }
            break;
        case 0x3EB:
            switch (frame_check2(32.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F1:
            switch (frame_check2(30.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            break;
        case 0x3F8:
            if (frame_check2(14.0f, pl, 0) != 0) {
                r = 0;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3F9:
            switch (frame_check2(44.0f, pl, 0)) {
            case 0:
                r = 0;
                break;
            default:
                break;
            }
            break;
        case 0x582:
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 0;
                break;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    case 0:
    default:
        switch (pl->char0) {
        case 0x3EA:
            if (frame_check2(16.0f, pl, 0) != 0) {
                r = 0;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3EB:
            r = 2;
            switch (frame_check2(38.0f, pl, 0)) {
            case 0:
                r = 0;
                break;
            default:
                break;
            }
            break;
        case 0x3F8:
            if (frame_check2(14.0f, pl, 0) != 0) {
                r = 0;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3F9:
            r = 2;
            switch (frame_check2(44.0f, pl, 0)) {
            case 0:
                r = 0;
                break;
            default:
                break;
            }
            break;
        case 0x587:
        case 0x586:
        case 0x585:
        case 0x584:
            r = 0;
            break;
        case 0x582:
            r = 2;
            switch (W1C4(pl)) {
            case 1:
                break;
            default:
                r = 0;
                break;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    case 1:
        switch (pl->char0) {
        case 0x3EA:
            if (frame_check2(2.0f, pl, 0) != 0) {
                r = 1;
            }
            break;
        case 0x3EB:
            switch (frame_check2(110.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            if (W1C4(pl) == 1) {
                r = 1;
            }
            break;
        case 0x3F1:
            switch (frame_check2(92.0f, pl, 0)) {
            case 0:
                r = 1;
                break;
            default:
                break;
            }
            if (W1C4(pl) == 1) {
                r = 1;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    case 5:
        switch (pl->char0) {
        case 0x3EA:
            if (frame_check2(28.0f, pl, 0) != 0) {
                r = 0;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3F2:
            if (frame_check2(22.0f, pl, 0) != 0) {
                r = 0;
            }
            if (W1C4(pl) == 1) {
                r = 2;
            }
            break;
        case 0x3F1:
        case 0x3EB:
            if (W1C4(pl) == 1) {
                r = 1;
            } else {
                r = 2;
            }
            break;
        default:
            if (pl->flag12 != 0) {
                r = 1;
            }
            break;
        }
        break;
    }
    switch (pl->char0) {
    case 0xCC:
    case 0xCA:
        if (pl->flag12 != 0) {
            r = 1;
        }
        break;
    case 0xD8:
    case 0xCF:
        if (pl->flag12 != 0) {
            r = 2;
            if (pl->flag14 == 6) {
            } else {
                r = 1;
            }
        }
        break;
    case 0x279:
    case 0x278:
    case 0x277:
    case 0x276:
    case 0x275:
    case 0x274:
    case 0x273:
    case 0x272:
    case 0x271:
    case 0x270:
    case 0x26F:
    case 0x26E:
    case 0x26D:
    case 0x26C:
    case 0x280:
    case 0x3D:
    case 0x3C:
        r = 2;
        break;
    case 0xDC:
    case 0xDB:
    case 0xD7:
    case 0xD6:
    case 0xD5:
    case 0xD4:
    case 0xD3:
    case 0xD1:
    case 0xD0:
    case 0xCE:
    case 0xCD:
        r = 2;
        break;
    case 0xD2:
        if (pl->flag14 == 6) {
            r = 2;
        }
        break;
    case 0x19D:
        r = 2;
        if (frame_check2(328.0f, pl, 0) != 0) {
        } else {
            r = 1;
            switch (frame_check2(22.0f, pl, 0)) {
            case 0:
                r = 2;
                break;
            default:
                break;
            }
        }
        if (W1C4(pl) == 1) {
            r = 2;
        }
        break;
    }
    return r;
}
