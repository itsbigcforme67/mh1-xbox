/* set22 - game.bin 0x00627840-0x0062813C. A flying creature seen in the
 * distance: after a random wait it appears at a random point (around the
 * master player, or at a fixed spot on stage 25), turns and scales by
 * kind, fades in and is drawn as a billboard. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern FLMAT rview_mat;
extern u8 ot3[];

u32 ran_suu(int);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flmatRotZ33(FLMAT *, f32);
void Set13_set2(int, f32 *, s16);
void Eft17_set_ex(f32 *, int, int, f32);

static void set22_move(SETW *sw);
static void set22_i(SETW *sw);
static void set22_m(SETW *sw);
static void set22_d(SETW *sw);
static void set22_e(SETW *sw);
static void set22_trans(PRIM *pr);

void Set22_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 22;
        sw->move = set22_move;
    }
}

static void set22_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set22_i(sw);
        break;
    case 1:
        set22_m(sw);
        break;
    case 2:
        set22_d(sw);
        break;
    case 3:
        set22_e(sw);
        break;
    }
}

static void set22_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->mode2 = 0;
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = (u16)ran_suu(1) & 0xF;
    sw->cnt = 0;
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set22_trans;
        return;
    }
    push_set_work(sw);
}

static void set22_m(SETW *sw) {
    PLW *pl = &player_work[game_w.master];
    s16 n;
    f32 base[3];
    f32 ofs[3];

    switch (game_w.stage) {
    default:
    case 7:
        n = 5;
        break;
    case 0x19:
        n = 5;
        break;
    }
    switch (sw->mode2) {
    case 0:
        if (--sw->timer < 0) {
            sw->mode2++;
            sw->se0 = (u16)ran_suu(1) % n;
            sw->timer = 7;
            if (game_w.stage != 0x19 && ((u16)ran_suu(1) & 0xF) == 0) {
                sw->se0 = 5;
                ofs[0] = 0.0f;
                ofs[2] = (u16)ran_suu(1) & 0x3FF;
                flvecCopy(base, pl->pos);
            } else {
                ofs[0] = 0.0f;
                ofs[2] = 21700.0f;
                base[0] = 10000.0f;
                base[2] = 10000.0f;
                base[1] = 0.0f;
            }
            if (game_w.stage != 0x19) {
                Set13_set2(5, sw->pos, sw->timer);
            }
            switch (sw->se0) {
            case 0:
            case 1:
            case 5:
                ofs[1] = 0.0f;
                sw->cnt = ((u16)ran_suu(1) & 0x7FF) - 0x400;
                sw->speed = 1.0f;
                break;
            case 2:
                ofs[1] = 7300.0f + ((u16)ran_suu(1) & 0x3FF);
                sw->cnt = ((u16)ran_suu(1) & 0xFFF) - 0x800;
                sw->speed = 1.0f;
                break;
            case 3:
                ofs[1] = 6000.0f + ((u16)ran_suu(1) & 0x3FF) * 3;
                sw->cnt = ((u16)ran_suu(1) & 0xFFF) - 0x800;
                sw->speed = 1.0f;
                break;
            case 4:
                ofs[1] = 7000.0f + ((u16)ran_suu(1) & 0x7FF);
                sw->cnt = 0;
                sw->speed = 0.7f + 0.0003f * ((u16)ran_suu(1) & 0x3FF);
                break;
            }
            flvecRotY(ofs, DEG2RAD(ANG2DEG(ran_suu(1))));
            sw->pos[0] = base[0] + ofs[0];
            sw->pos[1] = base[1] + ofs[1];
            sw->pos[2] = base[2] + ofs[2];
            if (sw->se0 == 5) {
                Eft17_set_ex(sw->pos, 0, 4, 0.15f);
                Eft17_set_ex(sw->pos, 0, 9, 1.0f);
                Eft17_set_ex(sw->pos, 0, 8, 1.0f);
                Eft17_set_ex(sw->pos, 0, 0xB, 1.0f);
            }
        }
        break;
    case 1:
        if (--sw->timer <= 0) {
            sw->mode2 = 0;
            sw->timer = ((u16)ran_suu(1) & 0x3F) + 1;
        } else {
            sw->prim->pos[0] = sw->pos[0];
            sw->prim->pos[1] = sw->pos[1];
            sw->prim->pos[2] = sw->pos[2];
            if (sw->se0 == 5) {
                add_prim(ot0, sw->prim, 0x40, 0);
            } else {
                add_prim(ot3, sw->prim, 8, 0);
            }
        }
        break;
    }
}

static void set22_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

static void set22_e(SETW *sw) {
    push_set_work(sw);
}

static void set22_trans(PRIM *pr) {
    FLMAT mat;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    s16 m;
    u8 a;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0);
        flmatMakeScale(&mat, sw->speed, sw->speed, sw->speed);
        flmatRotZ33(&mat, DEG2RAD(ANG2DEG(sw->cnt)));
        flmatSetTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&mat, &rview_mat);
        if (sw->timer < 4) {
            a = 255.0f * (sw->timer / 4.0f);
        } else {
            a = 0xFF;
        }
        switch (game_w.stage) {
        case 7:
            m = 2;
            break;
        case 0x19:
            m = 3;
            break;
        }
        flSetRenderState(0x67, (a << 24) | 0xFFFFFF);
        flSetRenderState(0x1A, (u32)&mat);
        cl = &mw->clay[m] + sw->se0 * 2;
        if (sw->timer < 6) {
            cl++;
        }
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
