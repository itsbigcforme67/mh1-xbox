/* dispframe_nm.c - DispFrameMessageA (SLPM_654.95 main 0x00276170-0x00276E64):
 * the game's message window (NPC talk, item lists, the info banner, the
 * quit-quest menu, reward boxes ...): a frame of 2TF sprite pieces around
 * cols x rows character cells, then the text, one line per '\n'.
 * Written by hand from the asm for the PC port (m2c's draft lost the float
 * offsets); NOT built for the PS2 and not compared with check.py.
 *
 * Frame record (fr): +0 s16 x, +2 s16 y, +4 u8 cell width, +5 u8 cell
 * height, +6 u8 columns, +7 u8 rows, +8 s16 font palette, +0xA u16 flags
 * (bits 0-1 frame style: 0/2 normal, 1 and 3 other skins; bit 0 also
 * selects the narrow border; 0x8000: border pieces + a filled background
 * of colour +0xC, else a tiled background only), +0xC u32 colour.
 * Sprite record of flps0008: s16 x, y, w, h; u32 colour; s16 u0, v0, u1, v1.
 * Screen x is scaled by 0.8 (the 640-wide layout on the 512-wide frame). */
#include "types.h"

void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void SetTextureStage(int);
void flps0004(void *);
void flps0008(void *);
void flfntLocate(int, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print_sp(void *, ...);
extern u8 lit_2244[];

#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define FU16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FU32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define SX(f) ((s16)(s32)(0.8f * (f)))

void DispFrameMessageA(u8 *fr, char *text, int alpha) {
    s16 q[10];          /* sprite: x y w h col(2) u0 v0 u1 v1 */
    s16 r[6];           /* rect: x0 y0 x1 y1 col(2) */
    f32 x, cw, xl, f, f2;
    s32 u0, u1b, xs, n, i, j, rows, cols, last;
    s16 y0, lh, yb, ty;
    u16 fl;
    char buf[0x80];
    char *d;

    SetFilterMode(1);
    SetTrnslMode(4, 5);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    x = (f32)FS16(fr, 0);
    cw = (f32)fr[4];
    fl = FU16(fr, 0xA);
    switch (fl & 3) {
    case 3:
        u0 = 156;
        u1b = 176;
        y0 = FS16(fr, 2) - 2;
        lh = fr[5] + 4;
        break;
    case 1:
        u0 = 228;
        u1b = 248;
        y0 = FS16(fr, 2) - 2;
        lh = fr[5] + 4;
        break;
    default:
        u0 = 192;
        u1b = 212;
        y0 = FS16(fr, 2) - 1;
        lh = fr[5] + 2;
        break;
    }
    cols = fr[6];
    rows = fr[7];
    if (fl & 0x8000) {
        /* the background: two thin bands and the body */
        *(u32 *)&r[4] = FU32(fr, 0xC);
        if (!(fl & 1)) {
            r[0] = SX(x);
            r[2] = SX(x + cw * (f32)cols);
            r[1] = y0 - 2;
            r[3] = y0;
            flps0004(r);
            r[1] = y0 + lh * rows;
            r[3] = r[1] + 2;
            flps0004(r);
            r[0] = SX(x - 2.5f);
            r[2] = SX(2.5f + (x + cw * (f32)cols));
        } else {
            r[0] = SX(x - 1.4f);
            r[2] = SX(1.4f + (x + cw * (f32)cols));
        }
        r[1] = y0;
        r[3] = y0 + lh * rows;
        flps0004(r);
        /* the border */
        u0 &= 0xFF;
        *(u32 *)&q[4] = ((u32)(alpha & 0xFF) << 24) | 0xFFFFFF;
        xl = 0.8f * (x - 8.0f);
        xs = SX(x);
        yb = y0 + lh * rows;
        /* top left */
        q[0] = (s16)(s32)xl;
        q[2] = xs - q[0];
        q[1] = y0 - 8;
        q[3] = 8;
        q[6] = u0 - 8;
        q[8] = u0;
        q[7] = 180;
        q[9] = 188;
        flps0008(q);
        /* top edge */
        q[6] = u0;
        q[8] = u1b;
        f = x;
        if (!(fl & 1)) {
            q[3] = 6;
            q[9] = 186;
        }
        for (n = cols; n > 0; n--) {
            q[0] = SX(f);
            f += cw;
            q[2] = SX(f) - q[0];
            flps0008(q);
        }
        /* top right */
        q[0] = SX(f);
        q[2] = 6;
        q[3] = 8;
        q[6] = u0 + 20;
        q[8] = u0 + 28;
        flps0008(q);
        /* bottom left */
        q[0] = (s16)(s32)xl;
        q[2] = xs - q[0];
        q[1] = yb;
        q[3] = 8;
        q[6] = u0 - 8;
        q[8] = u0;
        q[7] = 208;
        q[9] = 216;
        flps0008(q);
        /* bottom edge */
        q[6] = u0;
        q[8] = u1b;
        f = x;
        if (!(fl & 1)) {
            q[1] += 2;
            q[3] = 6;
            q[7] += 2;
        }
        for (n = cols; n > 0; n--) {
            q[0] = SX(f);
            f += cw;
            q[2] = SX(f) - q[0];
            flps0008(q);
        }
        /* bottom right */
        q[0] = SX(f);
        q[2] = 6;
        q[1] = yb;
        q[3] = 8;
        q[6] = u0 + 20;
        q[8] = u0 + 28;
        flps0008(q);
        /* left edge */
        q[0] = (s16)(s32)xl;
        q[1] = y0;
        q[3] = lh;
        q[6] = u0 - 8;
        q[7] = 188;
        q[9] = 208;
        if (!(fl & 1)) {
            q[2] = 4;
            q[8] = u0 - 2;
        } else {
            q[2] = 5;
            q[8] = u0 - 1;
        }
        for (n = rows; n > 0; n--) {
            flps0008(q);
            q[1] += lh;
        }
        /* right edge */
        q[1] = y0;
        q[3] = lh;
        q[8] = u0 + 28;
        q[7] = 188;
        q[9] = 208;
        if (!(fl & 1)) {
            q[0] = SX(2.5f + (x + cw * (f32)cols));
            q[2] = 4;
            q[6] = u0 + 22;
        } else {
            q[0] = SX(1.4f + (x + cw * (f32)cols));
            q[2] = 5;
            q[6] = u0 + 21;
        }
        for (n = rows; n > 0; n--) {
            flps0008(q);
            q[1] += lh;
        }
    } else {
        /* tiled background, the outer cells 8 larger */
        *(u32 *)&q[4] = ((u32)(alpha & 0xFF) << 24) | 0xFFFFFF;
        q[1] = y0;
        q[3] = 0;
        last = rows - 1;
        for (i = 0; i < rows; i++) {
            q[1] += q[3];
            q[3] = lh;
            q[7] = 188;
            q[9] = 208;
            if (i == 0) {
                q[1] -= 8;
                q[3] += 8;
                q[7] -= 8;
            }
            if (!(i < last)) {
                q[3] += 8;
                q[9] += 8;
            }
            f = x;
            for (j = 0; j < cols; j++) {
                f2 = f;
                f += cw;
                q[6] = (s16)(u0 & 0xFF);
                q[8] = (s16)u1b;
                if (j == 0) {
                    f2 -= 8.0f;
                    q[6] -= 8;
                } else if (!(j < cols - 1)) {
                    f += 8.0f;
                    q[8] += 8;
                }
                q[0] = SX(f2);
                q[2] = SX(f) - q[0];
                flps0008(q);
            }
        }
    }
    if (text != 0) {
        ty = FS16(fr, 2);
        SetTrnslMode(4, 5);
        flfntSetSize(fr[4], fr[5]);
        font_set_palette(FS16(fr, 8));
        for (n = rows; n > 0; n--) {
            d = buf;
            for (;;) {
                if (*text == '\n') {
                    *d = 0;
                    text++;
                    break;
                }
                *d = *text;
                if (*text == 0) {
                    n = 0;      /* the end of the text: this line is the last */
                    break;
                }
                text++;
                d++;
                if (d >= buf + sizeof buf - 1) {    /* PC: guard the stack copy */
                    *d = 0;
                    break;
                }
            }
            flfntLocate(FS16(fr, 0), ty);
            font_print_sp(lit_2244, buf);
            ty = (s16)(ty + lh);
        }
    }
}

/* DispFrameMessage (0x276160, alpha 0xB2) is in chat_nm.c */
