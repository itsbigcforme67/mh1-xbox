/* System work helpers. SLPM_654.95 0x001611F0-0x00161480 (g_system_w_init):
 * system_w clear, small random numbers, gaussian weight table for blurs,
 * sprite slot work, render start/end. Names from the symbol file. */
#include "g_alias.h"
#include "types.h"

extern u8 system_w[0x80];
extern u8 sprite_w[0x100];
extern u16 Rnd_w[2];
extern u8 rtcDate[8];
extern f32 std_rate_tbl[60];
extern f32 std_rate_all;
extern s16 reload_tex_total;
extern s16 reload_tex_id;
extern s16 reload_tex_num;
extern s32 render_flag;

void *memset(void *, int, unsigned);
f32 flPow(f32, f32);
f32 flExp(f32);
f32 flSqrt(f32);
void flBeginRender(void);
void flEndRender(void);

#if !defined(G_PART) || G_PART == 1
void system_w_init(void) {
    memset(system_w, 0, 0x80);
}
#endif

#if !defined(G_PART) || G_PART == 1
void system_error(void) {
}
#endif

#if defined(MHG_PS2) || defined(VERSION_MH2)
#define RAN_ARG u8 seed         /* G: the seed is a parameter */
#define RAN_SEED seed
#else
#define RAN_ARG void
#define RAN_SEED rtcDate[1]
#endif
#if !defined(G_PART) || G_PART == 1
void init_ran_suu(RAN_ARG) {
    Rnd_w[0] = RAN_SEED;
    Rnd_w[1] = RAN_SEED;
}
#endif

/* Tiny linear congruential generator, one state per stream n. */
#if !defined(G_PART) || G_PART == 1
int ran_suu(int n) {
    u32 r;
    u32 v;
    u16 *p = &Rnd_w[n];

    v = *p;
    if (v == 0) {
        v = 1;
    }
    r = (v * 0xB0) % 0xFF53;
    *p = r;
    return r & 0xFFFF;
}
#endif

#if !defined(G_PART) || G_PART == 2
void init_std_rate(void) {
    int i;

    std_rate_all = 0.0f;
    for (i = 0; i < 0x3C; i++) {
        f32 e = flExp(-flPow((f32)(i - 0x1E) / 9.0f, 2.0f) / 2.0f);
        f32 g = 1.0f / (1.0f / flSqrt(6.2831855f));
        std_rate_tbl[i] = g * e;
        std_rate_all += std_rate_tbl[i];
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
void sprite_work_init(void) {
    memset(sprite_w, 0, 0x100);
}
#endif

#if !defined(G_PART) || G_PART == 2
void sprite_work_free(s16 n) {
    s8 *p = (s8 *)&sprite_w[n * 4];

    if (*p != 0) {
        *p = 0;
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
void all_sprite_free(void) {
    s16 i;

#ifdef VERSION_MH2
    for (i = 0; i < 0x20; i++) {
#else
    for (i = 0; i < 0x40; i++) {
#endif
        sprite_work_free(i);
    }
}
#endif

#if !defined(G_PART) || G_PART == 2
void render_start(void) {
    flBeginRender();
    reload_tex_total = 0;
    reload_tex_id = -1;
    reload_tex_num = -1;
    render_flag++;
}
#endif

#if !defined(G_PART) || G_PART == 2
void render_end(void) {
    flEndRender();
    render_flag--;
}
#endif
