/* set01 - SLPM_654.95 0x001554E0-0x00156328 (Info_* included: they call
 * set01's static functions, so they are in the same file).
 * The on-screen info banner: messages are queued in set01_stack (up to 20),
 * and each shown one slides down from the top, holds, then slides away. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"

typedef struct SET01_STACK {
    u8 type;            /* 0x00 0xFF = string given directly */
    u8 msg;             /* 0x01 index into info_str* */
    u8 seq;             /* 0x02 game_w.info_seq when queued */
    u8 _pad03;
    s16 arg;            /* 0x04 item number or player number */
    u8 _pad06[2];
    char *str;          /* 0x08 */
    char buf[30];       /* 0x0C copy of the string (set01_set2_use_mem) */
    u8 x2A;             /* 0x2A */
    u8 x2B;             /* 0x2B */
} SET01_STACK;

extern SET01_STACK set01_stack[20];
extern s8 set01_stack_ptr;
extern s8 set01_stack_suu;
extern s8 set01_stack_timer;
extern char *info_str0[25];
extern char *info_str1[17];
extern char *info_str2[1];
extern char *info_str3[3];
extern char *item_str[327];
extern u8 ot2[];
/* "%s". Referenced, not redefined, until data is split per source file:
 * a literal here would duplicate bytes the data asm already holds. */
extern char lit_355_0035B7C0[];

/* Frame box drawn behind on-screen messages. */
typedef struct FRAME_MSG {
    s16 x;              /* 0x0 */
    s16 y;              /* 0x2 */
    u8 w;               /* 0x4 */
    u8 h;               /* 0x5 */
    s8 len;             /* 0x6 width in characters */
    u8 type;            /* 0x7 */
    s16 a;              /* 0x8 */
    s16 b;              /* 0xA */
} FRAME_MSG;

void flSetRenderState(int, u32);
void DispFrameMessage(FRAME_MSG *, int);
void flfntSetSize(int, int);
void flfntLocate(int, int);
void font_set_stack_no(int);
void font_set_palette(int);
void font_print(const char *, ...);

unsigned int strlen(const char *);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
void se_req(int, int, int);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim2(s16);

static void set01_set_sub(int type, int msg, s16 arg, u8 seq);
static void set01_set2_sub(char *str, u8 seq);
static void set01_move(SETW *sw);
static void set01_i(SETW *sw);
static void set01_m(SETW *sw);
static void set01_d(SETW *sw);
static void set01_e(SETW *sw);
static void set01_trans(PRIM *pr);

void Info_Initialization(void) {
    game_w.info_now = 0xFF;
    set01_stack_suu = 0;
    set01_stack_ptr = 0;
    set01_stack_timer = 0;
    game_w.info_seq = 0;
    memset(set01_stack, 0, sizeof(set01_stack));
}

int Info_stack_ck(u8 seq) {
    s16 i;

    if (set01_stack_suu == 0) {
        return 0;
    }
    for (i = 0; i < set01_stack_suu; i++) {
        if (seq == set01_stack[(s16)((set01_stack_ptr + i) % 20)].seq) {
            return 1;
        }
    }
    return 0;
}

void Info_control(void) {
    SET01_STACK *st;

    if (game_w.info_stop == 0) {
        if (set01_stack_timer > 0) {
            set01_stack_timer--;
            return;
        }
        if (set01_stack_suu > 0) {
            st = &set01_stack[set01_stack_ptr];
            if (st->type == 0xFF) {
                set01_set2_sub(st->str, st->seq);
            } else {
                set01_set_sub(st->type, st->msg, st->arg, st->seq);
            }
            if (++set01_stack_ptr >= 20) {
                set01_stack_ptr = 0;
            }
            set01_stack_timer = 60;
            set01_stack_suu--;
        }
    }
}

static void set01_set_sub(int type, int msg, s16 arg, u8 seq) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 1;
        sw->move = set01_move;
        sw->arg = type;
        sw->se1 = msg;
        sw->timer = arg;
        game_w.info_now = seq;
    }
}

u8 set01_set(int type, int msg, s16 arg) {
    s8 i;

    if (set01_stack_suu >= 19) {
        return 0xFF;
    }
    game_w.info_seq = (game_w.info_seq + 1) & 0x7F;
    i = (set01_stack_ptr + set01_stack_suu) % 20;
    set01_stack[i].type = type;
    set01_stack[i].msg = msg;
    set01_stack[i].seq = game_w.info_seq;
    set01_stack[i].arg = arg;
    set01_stack_suu++;
    return game_w.info_seq;
}

static void set01_set2_sub(char *str, u8 seq) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 1;
        sw->move = set01_move;
        sw->arg = 4;
        *sw->str = str;
        game_w.info_now = seq;
    }
}

u8 set01_set2(char *str) {
    s8 i;

    if (set01_stack_suu >= 19) {
        return 0xFF;
    }
    game_w.info_seq = (game_w.info_seq + 1) & 0x7F;
    i = (set01_stack_ptr + set01_stack_suu) % 20;
    set01_stack[i].seq = game_w.info_seq;
    set01_stack[i].type = 0xFF;
    set01_stack[i].str = str;
    set01_stack_suu++;
    return game_w.info_seq;
}

u8 set01_set2_use_mem(char *str) {
    s8 i;
    int j;

    if (set01_stack_suu >= 19) {
        return 0xFF;
    }
    game_w.info_seq = (game_w.info_seq + 1) & 0x7F;
    i = (set01_stack_ptr + set01_stack_suu) % 20;
    set01_stack[i].seq = game_w.info_seq;
    set01_stack[i].type = 0xFF;
    set01_stack[i].str = set01_stack[i].buf;
    if (strlen(str) < 30) {
        strcpy(set01_stack[i].buf, str);
        set01_stack[i].x2A = 0;
        set01_stack[i].x2B = 0;
    } else {
        for (j = 0; j < 30; j++) {
            set01_stack[i].buf[j] = str[j];
        }
        set01_stack[i].x2A = 0;
        set01_stack[i].x2B = 0;
    }
    set01_stack_suu++;
    return game_w.info_seq;
}

static void set01_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set01_i(sw);
        break;
    case 1:
        set01_m(sw);
        break;
    case 2:
        set01_d(sw);
        break;
    case 3:
        set01_e(sw);
        break;
    }
}

static void set01_i(SETW *sw) {
    char **str = sw->str;
    s16 len;
    f32 w;

    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->flag3E = 1;
    se_req(7, 0x10, 0);
    switch (sw->arg) {
    case 0:
        len = strlen(info_str0[sw->se1]);
        break;
    case 1:
        len = strlen(info_str1[sw->se1]);
        len += (s16)strlen(item_str[sw->timer]);
        break;
    case 2:
        len = strlen(info_str2[sw->se1]);
        len += (s16)strlen(item_str[sw->timer]);
        break;
    case 3:
        len = strlen(player_work[sw->timer].name);
        if (len & 1) {
            len++;
        }
        len += (s16)strlen(info_str3[sw->se1]);
        break;
    case 4:
        len = ((s16)strlen(*str) + 1) & ~1;
        break;
    }
    w = len;
    sw->pos[0] = 320.0f - 10.0f * (w / 2.0f);
    sw->pos[1] = -20.0f;
    sw->pos[2] = w;
    sw->speed = 40.0f;
    sw->cnt = 60;
    sw->prim_no = get_prim2();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr2(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set01_trans;
    } else {
        push_set_work(sw);
    }
    set01_m(sw);
}

static void set01_m(SETW *sw) {
    switch (sw->mode2) {
    case 0:
        sw->pos[1] += sw->speed;
        if (!(sw->pos[1] < 224.0f)) {
            sw->mode2++;
            sw->pos[1] = 224.0f;
        }
        break;
    case 1:
        sw->cnt--;
        if (sw->cnt <= 0 || (sw->cnt < 31 && set01_stack_suu > 0)) {
            sw->speed *= -1.0f;
            game_w.info_now = 0xFF;
            sw->mode2++;
        }
        break;
    case 2:
        sw->pos[1] += sw->speed;
        if (sw->pos[1] <= -20.0f) {
            sw->mode++;
        }
        break;
    }
    add_prim(ot2, sw->prim, 0x10, 1);
}

static void set01_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim2(sw->prim_no);
}

static void set01_e(SETW *sw) {
    push_set_work(sw);
}

static void set01_trans(PRIM *pr) {
    SETW *sw = pr->owner;
    char **str = sw->str;
    FRAME_MSG fm;
    s16 len;

    flSetRenderState(0x60, 0);
    fm.x = sw->pos[0];
    fm.y = sw->pos[1];
    fm.h = 20;
    fm.w = 20;
    fm.len = (s16)sw->pos[2] / 2;
    fm.type = 1;
    fm.a = 0;
    fm.b = 0;
    DispFrameMessage(&fm, 0);
    flfntSetSize(20, 20);
    flfntLocate(sw->pos[0], sw->pos[1]);
    font_set_stack_no(3);
    switch (sw->arg) {
    case 0:
        font_set_palette(0);
        font_print(lit_355_0035B7C0, info_str0[sw->se1]);
        break;
    case 1:
        font_set_palette(2);
        font_print(lit_355_0035B7C0, item_str[sw->timer]);
        len = strlen(item_str[sw->timer]);
        font_set_palette(0);
        flfntLocate(sw->pos[0] + 10.0f * len, sw->pos[1]);
        font_print(lit_355_0035B7C0, info_str1[sw->se1]);
        break;
    case 2:
        font_print(lit_355_0035B7C0, info_str2[sw->se1]);
        len = strlen(info_str2[sw->se1]);
        font_set_palette(2);
        flfntLocate(sw->pos[0] + 10.0f * len, sw->pos[1]);
        font_print(lit_355_0035B7C0, item_str[sw->timer]);
        break;
    case 3:
        font_set_palette(2);
        font_print(lit_355_0035B7C0, player_work[sw->timer].name);
        len = strlen(player_work[sw->timer].name);
        if (len & 1) {
            len++;
        }
        font_set_palette(0);
        flfntLocate(sw->pos[0] + 10.0f * len, sw->pos[1]);
        font_print(lit_355_0035B7C0, info_str3[sw->se1]);
        break;
    case 4:
        font_set_palette(0);
        font_print(lit_355_0035B7C0, *str);
        break;
    }
    flSetRenderState(0x60, 0x80);
}
