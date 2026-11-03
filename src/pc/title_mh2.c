/* title_mh2.c - MH2's boot on the PC as far as the title screen (docs/mh2_port.md). PC only, VERSION_MH2: written from MH2's
 * asm, not byte-checked. MH2 rewrote select.bin's boot: G's Init_task / Demo_task do not fit it. Addresses are MH2's.
 *
 * What is here and what the PS2 code does (select 0x6B5CF0 Init_task, 9 steps; 0x6B5230 Demo_task; sub_main 0x334190, 0x334260):
 *  - init_tex.bin (file 6638) into texture slots 2, 4, 5, 6, 9: sub_main 0x334190, slot list at 0x42F3E0.
 *  - demo_tex.bin (6636) into 2, 7, 8, 10, 11, 4, 5 and title_tex.bin (6639) into slot 3: sub_main 0x334260(0), list at 0x42F3E8.
 *  - the title: select 0x6B5660 draws slot 3 full screen, slot 0x155 (a small mark), the copyright, "PRESS START" (0x6B53A0).
 * Left out, each one a stand-in here: the sound packs (select 0x6B6200), the card check and options load (Init_task steps 2-8), the
 * tamper check at sub_main 0x4901B0 (code in the data section; Init_task step 1), the opening movie (0x6B58D0), and pressing start
 * (0x6B5A90 goes on to gm_sub's task at 0x8FAFD0: the mode menu, not ported). The title just stays.
 */
#include "types.h"

#include <stdint.h>
#include <stdlib.h>

void Tsk_Execute(void *fn, int slot);
void Tsk_Exit(void *t);
void Fade_task(void *t);
void all_reset(void);
void fade_set(int n);
void SetFilterMode(int m);
void SetTrnslMode(int src, int dst);
void SetTextureStage(int id);
void flps0008(void *spr);
void flfntSetSize(int w, int h);
void font_print_ex(int x, int y, int p, const char *fmt, ...);
void font_draw(void);
void font_reset(void);
void font_set(void);
int load_file_mdl(void *dst, int id);
void *flCreateTextureFromApx_mem(void *p, int type);
void rt_log_standin(const char *name);
extern u32 mem_tex[];
extern u8 *data_load_ptr;
extern u8 rt_sel_mem[];

#define SEL(a) (rt_sel_mem + ((a) - 0x6AF200))

typedef struct { s16 x, y, w, h; u32 col; s16 u0, v0, u1, v1; } SPR8;

static void load_list(int file, const u8 *slots, int skip_bits)
{
    u8 *p;
    int i, n;
    if (!data_load_ptr)         /* the host's game init sets it; that does not run on MH2's boot yet */
        data_load_ptr = malloc(0x400000);
    p = data_load_ptr;
    load_file_mdl(p, file);
    n = *(int *)p;
    for (i = 0; i < n && slots[i]; i++)
        if (!(skip_bits >> slots[i] & 1))
            mem_tex[slots[i]] = (u32)(uintptr_t)flCreateTextureFromApx_mem(p + *(int *)(p + i * 8 + 4), 0);
}

static void load_init_tex(void)
{
    static const u8 s[] = { 2, 4, 5, 6, 9, 0 };
    load_list(6638, s, 0);
}

static void load_demo_tex(void)
{
    static const u8 s[] = { 2, 7, 8, 10, 11, 4, 5, 0 }, t[] = { 3, 0 };
    load_list(6636, s, 0);
    load_list(6639, t, 0);
}

static void c_mark(int x, int y, u32 col)       /* c_disp, select 0x6B55D0: the (C) sign, slot 8 */
{
    SPR8 s = { (s16)x, (s16)y, 0x11, 0x11, col, 0, 0xEE, 0x12, 0xFF };
    SetTextureStage(8);
    flps0008(&s);
}

static void title_disp(int alpha)               /* select 0x6B5660 */
{
    u32 col = (u32)(alpha & 0xFF) << 24 | 0xFFFFFF;
    SPR8 s = { 0, 0, 0x280, 0x1C0, col, 0, 0, 0x1FF, 0x1BF };
    SPR8 m = { 0x1A7, 0xC7, 0x14, 0x16, col, 0x60, 0x89, 0x7F, 0xA8 };
    SetFilterMode(1);           /* sub_main 0x369010: state 0x63 through the table at 0x43C978. The map's name there, SetOpeMode, is wrong */
    SetTrnslMode(4, 5);         /* the host's trans() sets this for G's tasks; this title draws outside it, and without it the alpha is off (the box round the (C)) */
    SetTextureStage(3);
    flps0008(&s);
    if (mem_tex[0x155]) {       /* a small mark on the logo; what loads slot 0x155 is not known yet */
        SetTextureStage(0x155);
        flps0008(&m);
    }
    flfntSetSize(0x12, 0x12);
    c_mark(0x7D, 0x17A, 0xFF000000);            /* the shadow, then the sign; system_w+0x1A picks this line */
    c_mark(0x7B, 0x178, 0xFFFFFFFF);
    font_print_ex(0x91, 0x178, 1, "%s", (const char *)SEL(0x6CDEA0));
}

static int blink;

static void press_start(void)                   /* select 0x6B53A0: timer starts at 0x20, so the first half second shows it */
{
    if (++blink & 0x20) {
        flfntSetSize(0x18, 0x18);
        font_print_ex(0x68, 0x15C, 1, "%s", (const char *)SEL(0x6D0980));
    }
}

static void mh2_Demo_task(u8 *t)
{
    switch (t[8]) {
    case 0:
        t[8]++;
        all_reset();
        load_demo_tex();
        rt_log_standin("MH2 boot: opening movie (select 0x6B58D0) skipped");
        fade_set(2);
        blink = 0x20;
        break;
    case 1:
        title_disp(0xFF);
        press_start();
        font_draw();
        font_reset();
        break;
    }
}

void mh2_Init_task(u8 *t)
{
    font_set();
    load_init_tex();
    rt_log_standin("MH2 boot: sound packs, card check, tamper check (select Init_task) skipped");
    Tsk_Execute((void *)Fade_task, 9);
    Tsk_Execute((void *)mh2_Demo_task, 3);
    Tsk_Exit(t);
}
