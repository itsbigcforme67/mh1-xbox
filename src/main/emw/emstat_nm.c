/* Monster status reset. SLPM_654.95 0x0010BAD0-0x0010BE30 (f_em): em_status_init clears the
 * per monster state block (0x6E8..0x9F1 etc.) and loads the tables selected by the kind.
 * Offsets exact, meanings mostly unknown. */
#include "types.h"

#define B8(w, o)  (*(u8 *)((u8 *)(w) + (o)))
#define B16(w, o) (*(u16 *)((u8 *)(w) + (o)))
#define B32(w, o) (*(s32 *)((u8 *)(w) + (o)))
#define BF(w, o)  (*(f32 *)((u8 *)(w) + (o)))

typedef struct EMK {
    u8 _pad00[2];
    u8 kind;                    /* 0x002 */
    u8 _pad003[0x898 - 3];
    s32 x898, x89C, x8A0;       /* 0x898 limits, defaulted from the tables */
} EMK;
#define KIND(w) ((w)->kind)

typedef struct QW { u8 _pad00[8]; s16 x08; } QW;
extern QW quest_w;
extern f32 D_63BBB0[];
extern u8 D_63BB80[];
extern s32 *D_641240[];
extern s32 *D_6413B0[];
extern s32 *D_641310[];
extern s32 D_640540[];

void func_55B060(void *);
void func_539C90(void *);
f32 func_53B080(void *);
f32 func_53B310(void *);
void Em_hagi_point_clr(void *);

void em_status_init(EMK *w) {
    s8 i;
    u8 *p;
    s32 *ta;
    s32 *tb;
    s32 *tc;

    B8(w, 0x9ED) = 0;
    i = 0;
    p = (u8 *)w;
    do {
        B16(p, 0x960) = 0;
        B32(p, 0x964) = 0;
        B32(p, 0x968) = 0;
        B32(p, 0x96C) = 0;
        B16(p, 0x970) = 0;
        B16(p, 0x972) = 0;
        B16(p, 0x974) = 0;
        B8(p, 0x976) = 0;
        B8(p, 0x977) = 0;
        B8(p, 0x979) = 0;
        B8(p, 0x97A) = 0;
        B8(p, 0x97B) = 0;
        B8(p, 0x97E) = 0;
        B16(p, 0x97C) = 0;
        B16(p, 0x980) = 0;
        i++;
        p += 0x34;
    } while (i < 2);
    func_55B060(w);
    B32(w, 0x878) = 0;
    B16(w, 0x70E) = 1;
    B8(w, 0x412) = 0;
    B8(w, 1) = 1;
    BF(w, 0x1A0) = 2.0f;
    BF(w, 0x1F0) = 2.0f;
    BF(w, 0x240) = 2.0f;
    BF(w, 0x290) = 2.0f;
    BF(w, 0xB8) = D_63BBB0[KIND(w)];
    BF(w, 0xBC) = D_63BBB0[KIND(w)];
    BF(w, 0xC0) = D_63BBB0[KIND(w)];
    if (quest_w.x08 == 0) {
        B32(w, 0xA0) = 0;
        B32(w, 0xA4) = 0;
        B32(w, 0xA8) = 0;
    }
    BF(w, 0x798) = 1.0f;
    B8(w, 0x56A) = 0;
    B32(w, 0x39C) = 0;
    Em_hagi_point_clr(w);
    B8(w, 0x612) = D_63BB80[KIND(w)];
    B32(w, 0x6E8) = 0;
    B32(w, 0x6EC) = 0;
    B32(w, 0x6F0) = 0;
    B32(w, 0x6F4) = 0;
    BF(w, 0x930) = 1.0f;
    B8(w, 0x87F) = 0;
    B8(w, 0x88E) = 0;
    B8(w, 0x884) = 0;
    B8(w, 0x885) = 0;
    B16(w, 0x886) = 0;
    B8(w, 0x888) = 0;
    B8(w, 0x889) = 0;
    B8(w, 0x88A) = 0;
    B8(w, 0x88C) = 0;
    B8(w, 0x88F) = 0;
    B16(w, 0x890) = 0;
    B16(w, 0x892) = 0;
    B16(w, 0x894) = 0;
    B16(w, 0x896) = 0;
    ta = D_641240[KIND(w)];
    tb = D_6413B0[KIND(w)];
    tc = D_641310[KIND(w)];
    B32(w, 0x8A4) = tc[0];
    B32(w, 0x8A8) = ta[0];
    B32(w, 0x8AC) = tb[0];
    if (w->x898 <= 0) {
        w->x898 = tc[1];
    }
    if (w->x89C <= 0) {
        w->x89C = ta[1];
    }
    if (w->x8A0 <= 0) {
        w->x8A0 = tb[1];
    }
    B8(w, 0x8B6) = 0;
    B16(w, 0x8B4) = 0;
    B16(w, 0x8B2) = 0;
    B8(w, 0x8B7) = 0;
    B8(w, 0x8B8) = 0;
    B8(w, 0x8B9) = 0;
    B8(w, 0x8BB) = 0;
    B8(w, 0x8BC) = 0;
    B8(w, 0x8BD) = 0;
    B8(w, 0x8BE) = 0;
    B8(w, 0x8C0) = 0;
    B8(w, 0x8C1) = 0;
    B8(w, 0x8BF) = 0;
    B8(w, 0x8C2) = 0;
    B32(w, 0x8F4) = 0;
    B32(w, 0x8F8) = 0;
    B32(w, 0x8FC) = 0;
    B32(w, 0x900) = 0;
    B8(w, 0x914) = 0;
    B8(w, 0x915) = 0;
    B8(w, 0x916) = 1;
    B8(w, 0x917) = 0;
    B32(w, 0x918) = 0;
    B32(w, 0x91C) = 0;
    B32(w, 0x920) = 0;
    B32(w, 0x924) = 0;
    B32(w, 0x940) = D_640540[KIND(w)];
    B16(w, 0x94E) = 0;
    B8(w, 0x957) = 0;
    B8(w, 0x958) = 0;
    B8(w, 0x959) = 0;
    B8(w, 0x95A) = 0;
    B8(w, 0x95C) = 0;
    B8(w, 0x9E1) = 0;
    B8(w, 0x9E3) = 0;
    B8(w, 0x9E4) = 0;
    B8(w, 0x9E5) = 0;
    B8(w, 0x9E7) = 0;
    B8(w, 0x9E6) = 0;
    B8(w, 0x9E8) = 0;
    B8(w, 0x9EA) = 0;
    B8(w, 0x9F1) = 0;
    func_539C90(w);
    B16(w, 0x7B2) = 0;
    B16(w, 0x7B6) = 0;
    B16(w, 0x7CC) = 0;
    B16(w, 0x7D0) = 0;
    B16(w, 0x7C4) = 0;
    B16(w, 0x7C8) = 0;
    B16(w, 0x7BA) = 0;
    B16(w, 0x7BE) = 0;
    B16(w, 0x7C0) = 0;
    B8(w, 0x7D2) = 0;
    B8(w, 0x7D3) = 0;
    B8(w, 0x7D6) = 0;
    BF(w, 0x7D8) = func_53B080(w);
    BF(w, 0x7DC) = func_53B310(w);
    B8(w, 0x4D4) = 1;
    B32(w, 0x9D4) = 0;
    B8(w, 0x8BA) = 0;
    B8(w, 0x877) = 0;
}
