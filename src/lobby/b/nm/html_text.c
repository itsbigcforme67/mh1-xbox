/* html_text - lobby.bin 0x005BF810-0x005BF91C Split_TagCode, 0x005BFD60-0x005BFFFC Analysis_StringData,
 * 0x005C0000-0x005C0560 Display_StringData: the small HTML text renderer behind nwDispStr_Html (server messages:
 * the 6706 admin message, error texts, the top information page).
 * Written from the asm for the PC (agent B, 11 Oct 2026), not compared with check.py.
 *
 * How it works: nwDispStr_Html repeats Analysis_StringData + Display_StringData once per line.
 * Analysis_StringData cuts the next line of html_string_ptr into display_buffer: an array of 12-byte runs
 * (string pointer, tag bits, colour, size, line feeds, byte length). A run starts at every tag; tags are
 * <BODY> <SIZE=n> <COLOR=n> <BR> <CENTER> <LEFT> <RIGHT> <END> <LF=n> <C=n> (Analysis_TagCode sets the bits),
 * a backslash escapes the next byte. Display_StringData walks the runs, applies each run's tag bits (position,
 * size, colour) and prints its text with cnWrap_FontDisp. */
#include "lobby_a.h"

typedef struct {
    char *str;      /* 0x0 first byte of the run's text */
    u32 flags;      /* 0x4 tag bits: 1 BODY, 2 SIZE, 4 COLOR, 8 BR, 0x10 CENTER, 0x20 LEFT, 0x40 RIGHT, 0x80 END, 0x100 LF */
    u8 color;       /* 0x8 COLOR / C value (index into color_tbl) */
    u8 size;        /* 0x9 SIZE value (index into font_size_tbl) */
    u8 lf;          /* 0xA LF value (number of lines to feed) */
    u8 len;         /* 0xB text bytes in the run */
} HTML_RUN;

extern char *html_string_ptr;
extern HTML_RUN *dp;
extern HTML_RUN *dp_before;
extern u8 html_end_flag;
extern u8 html_start_flag;
extern u8 html_tag_flag;
extern u8 html_layout;
extern u16 html_line_len;
extern s32 html_size;
extern u32 tag_flag;
extern f32 html_x;
extern f32 html_y;
extern f32 html_z;
extern f32 html_default_x;
extern char tag_buffer[];               /* 0x6E9950, 30 bytes */
extern char disp_buffer[];              /* 0x6E9970, 768 bytes: one run's text, NUL-terminated */
extern HTML_RUN display_buffer[];       /* 0x6E9C70, 600 bytes = 50 runs */
extern u8 color_tbl_3745[];             /* 0x617F10 */
extern f32 font_size_tbl_3746[];        /* 0x617F20 */

void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
char *strcpy(char *, const char *);
void check_halfcode(void);
void Analysis_TagCode(void);
void cnWrap_SetFontColor(int color);
void cnWrap_SetFontSize(f32 size);
void cnWrap_FontDisp(f32 x, f32 y, f32 z, char *s);   /* PC order, see lb_bz155.c */
void flfntSetSize(int w, int h);

/* the same Shift-JIS lead byte test the asm inlines everywhere */
static int html_is_sjis(u8 c) {
    if (c >= 0x80 && c < 0xA0) {
        return 1;
    }
    return c >= 0xE0;
}

/* 0x5BF810: copy the tag after '<' up to and including '>' into tag_buffer (html_string_ptr ends after '>').
 * A tag of 25 bytes or more, or a Shift-JIS byte inside it, turns into "END" and html_end_flag 2. */
void Split_TagCode(void) {
    char *p = tag_buffer;
    u16 n = 0;
    char c;

    memset(tag_buffer, 0, 30);
    for (;;) {
        c = *html_string_ptr;
        *p = c;
        if (c == '>') {
            html_string_ptr++;
            return;
        }
        html_string_ptr++;
        n++;
        p++;
        if (n >= 25 || html_is_sjis((u8)*html_string_ptr)) {
            break;
        }
    }
    memset(tag_buffer, 0, 30);
    strcpy(tag_buffer, "END");          /* lit 0x65EB18 */
    html_end_flag = 2;
}

/* 0x5BFD60: split the next line into runs (display_buffer). html_end_flag 1 at the end of the string or <END>. */
void Analysis_StringData(void) {
    u8 n = 0;
    s8 c;
    u8 t;

    memset(display_buffer, 0, 600);
    html_line_len = 0;
    dp = display_buffer;
    dp_before = display_buffer;
    html_tag_flag = 0;
    for (;;) {
        if (n >= 31) {
            html_end_flag = 1;
            return;
        }
        c = *html_string_ptr;
        if (html_is_sjis((u8)c)) {
            /* a two-byte character: after <LF> (start flag 2) it opens a new run */
            if (html_start_flag == 2) {
                html_start_flag = 1;
                dp->flags = 1;
                dp->str = html_string_ptr;
                dp->len = 0;
                dp_before = dp;
                dp++;
            }
            html_string_ptr += 2;
            dp_before->len += 2;
            html_line_len += 2;
            continue;
        }
        if (c == 0) {
            html_end_flag = 1;
            return;
        }
        html_tag_flag = 0;
        check_halfcode();
        t = html_tag_flag;
        if (t == 0) {
            html_line_len++;
            if (*html_string_ptr == '\\') {
                html_string_ptr++;
                n++;
                dp->flags = 1;
                dp->str = html_string_ptr;
                dp->len = 1;
                dp_before = dp;
                dp++;
            } else {
                html_string_ptr++;
                dp_before->len++;
            }
            continue;
        }
        if (t == 8 || t == 4 || t == 9) {   /* <END>, <BR>, <LF=n> end the line */
            if (html_end_flag != 2 && t == 8) {
                html_end_flag = 1;
            }
            return;
        }
        if (*html_string_ptr == '\\') {
            html_string_ptr++;
            dp->len = 1;
            html_line_len++;
        } else {
            dp->len = 0;
        }
        n++;
        dp->str = html_string_ptr;
        dp_before = dp;
        dp++;
    }
}

/* 0x5C0000: draw the runs. size: font size (0 = 22 * font_size_tbl[html_size]); line_h: <BR> step (0 = 24 * table).
 * In the asm the two floats arrive in f12 / f13 (nwDispStr_Html passes 0, 0). */
void Display_StringData(f32 size, f32 line_h) {
    s32 i;
    s32 r;

    if (html_end_flag == 2) {
        return;
    }
    dp = display_buffer;
    for (r = 0; r < 30; r++) {
        if (dp->len == 0 && dp->flags == 0) {
            return;
        }
        tag_flag = dp->flags;
        for (i = 0; i < 16; i++) {
            if (dp->flags & 2) {
                html_size = dp->size;
            }
            if (!(tag_flag & 1)) {
                tag_flag >>= 1;
                if (tag_flag == 0) {
                    break;
                }
                continue;
            }
            tag_flag >>= 1;
            switch (i + 1) {    /* = the tag's number in Analysis_TagCode */
            case 1:             /* BODY: back to the line start of the current layout */
                switch (html_layout) {
                case 7:
                    html_x = html_default_x
                           + (396.0f - (f32)html_line_len * (20.0f * font_size_tbl_3746[html_size] / 2.0f));
                    break;
                case 6:
                    html_x = html_default_x;
                    break;
                case 5:
                    html_x = (f32)(320 - (html_line_len >> 1) * 10);
                    break;
                }
                break;
            case 2:             /* SIZE */
                html_size = dp->size;
                break;
            case 3:             /* COLOR */
            case 10:            /* C */
                cnWrap_SetFontColor(color_tbl_3745[dp->color]);
                break;
            case 4:             /* BR */
                html_x = html_default_x;
                if (line_h == 0.0f) {
                    html_y += 24.0f * font_size_tbl_3746[html_size];
                } else {
                    html_y += line_h;
                }
                break;
            case 5:             /* CENTER */
                html_layout = 5;
                html_x = (f32)(320 - (html_line_len >> 1) * 10);
                break;
            case 6:             /* LEFT */
                html_layout = 6;
                html_x = html_default_x;
                break;
            case 7:             /* RIGHT */
                html_layout = 7;
                html_x = html_default_x
                       + (396.0f - (f32)html_line_len * (22.0f * font_size_tbl_3746[html_size] / 2.0f));
                break;
            case 8:             /* END */
                return;
            case 9:             /* LF=n */
                html_x = html_default_x;
                html_y += (f32)dp->lf * (24.0f * font_size_tbl_3746[html_size]);
                break;
            }
        }
        if (dp->len != 0 && dp->str != 0) {
            memset(disp_buffer, 0, 768);
            memcpy(disp_buffer, dp->str, dp->len);
            disp_buffer[dp->len] = 0;
            if (size == 0.0f) {
                flfntSetSize(20, 20);
                cnWrap_SetFontSize(22.0f * font_size_tbl_3746[html_size]);
            } else {
                cnWrap_SetFontSize(size);
            }
            cnWrap_FontDisp(html_x, html_y, html_z, disp_buffer);
            html_x += (f32)dp->len * (22.0f * font_size_tbl_3746[html_size] / 2.0f);
        }
        dp++;
    }
}
