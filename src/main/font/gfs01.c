/* gfs01 - font stack, resets, pad accessors 0x00162640-0x00162DA8: font_set_stack_no, font_set_palette, font_reset, font_draw_end, LoadAskData, all_reset, all_motion_free, em_motion_free, stage_free, init_game_work, init_select_work, init_demo_work, softreset, softreset_ck, softkey_ck, Get_sw, Get_sw2, Get_sw_on2, kb_input_ck_enter, Get_kb_input, kb_chat_in_chk, decide_se, cursor_se, cancel_se, unmei_se, Online_ck, Now_Game_ck. Whole file in gfs_nm.c. */
#include "types.h"
#include "sysw.h"

typedef struct PSWE { u16 w[17]; } PSWE;
extern PSWE Psw[];
extern int font_reset_flag;
extern u32 motion_set_handle_tbl[0x1004];
extern int ask_load_adrs;
extern u8 *lpSKey;
extern u8 select_w[0xC0];
extern u8 demo_w[0x44];
extern u8 game_w[0x224];

void *memset(void *, int, unsigned);
void flfntSetZ(f32);
void flfntSetPalette(int);
void flfntSetSize(int, int);
void *flAllocMemory(int);
int load_file_mdl(void *, int);
int flReleaseMotionSetHandle(int);
int SoftKeyboard_alive_check(void);
void se_req(int, int, int);
void adx_se_set(int, int);
void all_motion_free(void);
void init_game_work(void);
void init_demo_work(void);
void init_select_work(void);
void all_reset(void);























/* Extra pieces needing other module symbols: all_reset, softreset, stage_free etc. */
void stage_free(void);
void Info_Initialization(void);
void View_initialize(void);
void set_viewproj(int);
void InitRenderState(int);
void all_model_free(void);
void all_sprite_free(void);
void movie_exit(void);
void release_tex_all(void);
void flCompact(void);
void model_work_init(void);
void init_move_work(void);
void init_view_work(void);
void init_light_work(void);
void clr_pl_work(void);
void clr_em_work(void);
void clr_stg_work(void);
void TransReset(void);
void setBGcolor(int);
void mkTexture(int, int, int);
void fade_reset(void);
void ot_init(void);
void prim_init(void);
void prim_init2(void);
void se_stop_all(void);
void str_stop_all(void);
void vib_stop_all(void);
void Init_rev_set(void);
void font_stack_reset(void);
void font_set2(void);
void SoftKeyboard_exit(void);
void release_stage_model(void);
void init_select_work(void);
void Load_overlay(int, int);
void SchedulerInit(void);
void Tsk_Execute(void *, int);
void func_533A00(void);






void font_set_stack_no(int n) {
    if (n < 0) {
        n = 0;
    }
    if (n >= 5) {
        n = 4;
    }
    flfntSetZ(1.0f + (1000.0f * (f32)n) / 5.0f);
}

void font_set_palette(int n) {
    if ((s16)n >= 0x10) {
        n = 0;
    }
    flfntSetPalette(n);
}

void font_reset(void) {
    flfntSetSize(0x14, 0x14);
    font_set_stack_no(0);
    font_set_palette(0);
}

void font_draw_end(void) {
    font_reset_flag = 1;
}

void LoadAskData(void) {
    ask_load_adrs = (int)flAllocMemory(0x97C00);
    load_file_mdl((void *)ask_load_adrs, 0x883);
}

void all_reset(void) {
    system_w.loading = 1;
    Info_Initialization();
    View_initialize();
    set_viewproj(0);
    InitRenderState(0);
    all_motion_free();
    all_model_free();
    all_sprite_free();
    movie_exit();
    release_tex_all();
    flCompact();
    model_work_init();
    init_move_work();
    init_view_work();
    init_light_work();
    clr_pl_work();
    clr_em_work();
    clr_stg_work();
    flCompact();
    TransReset();
    setBGcolor(0);
    mkTexture(5, 0x157, 0);
    mkTexture(6, 0x156, 0);
    fade_reset();
    ot_init();
    prim_init();
    prim_init2();
    se_stop_all();
    str_stop_all();
    vib_stop_all();
    Init_rev_set();
    system_w.x32 = 0;
    system_w.softkey = 1;
    system_w.x33 = 0;
    system_w.x3C = 0;
    font_stack_reset();
    font_set2();
    if (lpSKey != 0) {
        SoftKeyboard_exit();
    }
    system_w.loading = 0;
    system_w.x12 = 1;
}

void all_motion_free(void) {
    int i;
    u32 *p = motion_set_handle_tbl;

    for (i = 0; i < 0x1004; i++) {
        if (*p != 0) {
            if (flReleaseMotionSetHandle(*p) == 1) {
                *p = 0;
            }
        }
        p++;
    }
}

void em_motion_free(s16 em) {
    s16 i;
    u32 *p;
    int end;

    i = em * 0x258 + 0x6A4;
    end = i + 0x258;
    if (i < end) {
        p = &motion_set_handle_tbl[i];
        do {
            if (*p != 0) {
                if (flReleaseMotionSetHandle(*p) == 1) {
                    *p = 0;
                }
            }
            i++;
            p++;
        } while (i < end);
    }
}

void stage_free(void) {
    release_stage_model();
    clr_stg_work();
}

void init_game_work(void) {
    memset(game_w, 0, 0x224);
}

void init_select_work(void) {
    u16 a;
    u8 b;

    a = *(u16 *)(select_w + 0xAC);
    b = select_w[0xB6];
    memset(select_w, 0, 0xC0);
    *(u16 *)(select_w + 0xAC) = a;
    select_w[0xB6] = b;
}

void init_demo_work(void) {
    memset(demo_w, 0, 0x44);
}

void softreset(void) {
    vib_stop_all();
    all_reset();
    init_game_work();
    init_select_work();
    init_demo_work();
    clr_pl_work();
    clr_em_work();
    clr_stg_work();
    ot_init();
    prim_init();
    prim_init2();
    Load_overlay(1, 1);
    SchedulerInit();
    Tsk_Execute(func_533A00, 0);
}

int softreset_ck(void) {
    return 0;
}

int softkey_ck(void) {
    if (system_w.softkey == 0 || (s8)SoftKeyboard_alive_check() != 0) {
        return 0;
    }
    return 1;
}

int Get_sw(int n) {
    return (Psw[n].w[12] | Psw[n].w[2]) & 0xFFFF;
}

u16 Get_sw2(int n) {
    u16 r = 0;

    if (SoftKeyboard_alive_check() == 0) {
        r = (Psw[n].w[12] & 0x3C00) | Psw[n].w[2];
    }
    return r;
}

u16 Get_sw_on2(int n) {
    if (SoftKeyboard_alive_check() != 0) {
        return 0;
    }
    return Psw[n].w[2];
}

int kb_input_ck_enter(void) {
    switch (lpSKey[0x65A]) {
    case 0x28:
        return 1;
    default:
        return 0;
    }
}

u8 Get_kb_input(void) {
    return lpSKey[0x65A];
}

int kb_chat_in_chk(void) {
    u8 c = lpSKey[0x65A];

    if (c == 0) {
        return 0;
    }
    return c != 0x29;
}

void decide_se(void) {
    se_req(1, 0x73, 0);
}

void cursor_se(void) {
    se_req(1, 0x72, 0);
}

void cancel_se(void) {
    se_req(7, 0x14, 0);
}

void unmei_se(int a) {
    adx_se_set(a, 9);
}

int Online_ck(void) {
    return system_w.online != 0;
}

int Now_Game_ck(void) {
    if (game_w[0] == 2 || game_w[0] == 3) {
        return 1;
    }
    return 0;
}
