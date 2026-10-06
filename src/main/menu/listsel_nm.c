/* listsel_nm.c - menu cursor helpers (SLPM_654.95 main 0x00274EC0-0x00275128):
 * ListSelect, PageSelect, Menu_select_mv. Written by hand from the asm for
 * the PC port (the start menu, item lists); NOT built for the PS2, not
 * compared with check.py. Pad bits: 0x2000 up, 0x1000 down, 0x800 left,
 * 0x400 right (Psw trigger / repeat bits). */
#include "types.h"

void se_req(int, int, int);

/* ListSelect (0x274EC0): *cur in 0..n-1, up/down with wrap; returns 1 (and
 * the cursor sound 7/22) when it moved */
int ListSelect(u8 *cur, int sw, int n) {
    u8 old = *cur;
    u8 c = old;

    sw &= 0xFFFF;
    if (sw & 0x2000) {
        c = (old == 0) ? (u8)((n & 0xFF) - 1) : (u8)(old - 1);
    } else if (sw & 0x1000) {
        c = (u8)(old + 1);
        if (!(c < (n & 0xFF))) {
            c = 0;
        }
    }
    *cur = c;
    if (c != old) {
        se_req(7, 22, 0);
        return 1;
    }
    return 0;
}

/* PageSelect (0x274F70): the same with left (0x800) / right (0x400) */
int PageSelect(u8 *cur, int sw, int n) {
    u8 old = *cur;
    u8 c = old;

    sw &= 0xFFFF;
    if (sw & 0x800) {
        c = (old == 0) ? (u8)((n & 0xFF) - 1) : (u8)(old - 1);
    } else if (sw & 0x400) {
        c = (u8)(old + 1);
        if (!(c < (n & 0xFF))) {
            c = 0;
        }
    }
    *cur = c;
    if (c != old) {
        se_req(7, 22, 0);
        return 1;
    }
    return 0;
}

/* Menu_select_mv (0x275020): a two-page list of n entries (n / 2 a page):
 * up/down inside the page, left/right to the same row of the other page
 * (sound 7/17) */
void Menu_select_mv(u8 *cur, int sw, int n) {
    u8 half = (u8)((n & 0xFF) >> 1);
    u8 all = (u8)(n & 0xFF);

    if (*cur < half) {
        ListSelect(cur, sw, half);
    } else {
        *cur = (u8)(*cur - half);
        ListSelect(cur, sw, half);
        *cur = (u8)(*cur + half);
    }
    sw &= 0xFFFF;
    if (sw & 0xC00) {
        if (sw & 0x800) {
            if (*cur < half) {
                *cur = (u8)(*cur + n);
            }
            *cur = (u8)(*cur - half);
        } else {
            *cur = (u8)(*cur + half);
            if (!(*cur < all)) {
                *cur = (u8)(*cur - n);
            }
        }
        se_req(7, 17, 0);
    }
}
