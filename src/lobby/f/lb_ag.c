/* Lobby: 2D draw helpers (buttons, icons, job icon, 2TF sprite, square outline), hand-written from m2c drafts. */
#include "lobby_f.h"
extern PLW player_work[];
extern u8 lb_button_tbl[];
void flps0008();
void flps0002();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Lb_put_icon_free();
void Lb_put_icon_free2();
void Sel_csr_disp();
int Lb_check_hotel();
int strlen();
void Lb_put_icon();
int Lb_put_button(a, b, c)
int a;
s16 b;
int c;
{
    s16 p0;
    s16 p2;
    s16 p4;
    s16 p6;
    int p8;
    s16 q0;
    s16 q2;
    s16 r0;
    s16 r2;
    u8 *t;
    t = lb_button_tbl + c * 8;
    p2 = b;
    p0 = 0.8f * (f32)a;
    p4 = 0.8f * (f32)(*(s16 *)(t + 4) - *(s16 *)t);
    p6 = *(s16 *)(t + 6) - *(s16 *)(t + 2);
    if (c == 9) {
        p4 = 0x32;
        p6 = 0x20;
    }
    p8 = -1;
    q0 = *(s16 *)t;
    q2 = *(s16 *)(t + 2);
    r0 = *(s16 *)(t + 4);
    r2 = *(s16 *)(t + 6);
    flps0008(&p0, &r0, &q0, &p4);
    return (s16)((s16)a + (*(s16 *)(t + 4) - *(s16 *)t));
}
void Lb_put_icon(a, b, c, d)
int a;
s16 b;
u32 c;
int d;
{
    s16 p0;
    s16 p2;
    s16 p4;
    s16 p6;
    int p8;
    s16 q0;
    s16 q2;
    s16 r0;
    s16 r2;
    u8 *t;
    s16 w;
    t = (u8 *)lb_icon_tbl + c * 8;
    p0 = 0.8f * (f32)a;
    p2 = b;
    p4 = 0.8f * (f32)(*(s16 *)(t + 4) - *(s16 *)t);
    w = p4;
    if (w < 0) {
        p4 = -w;
    }
    p6 = *(s16 *)(t + 6) - *(s16 *)(t + 2);
    switch (c) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        p4 = 0x14;
        p6 = 0x14;
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        p0 -= 4;
        p2 -= 2;
        p4 = 0x14;
        p6 = 0x16;
        break;
    case 12:
        p0 -= 4;
        p2 -= 4;
        p4 = 0x18;
        p6 = 0x18;
        break;
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
        p4 = 0x18;
        p6 = 0x1C;
        break;
    }
    p8 = d;
    q0 = *(s16 *)t;
    q2 = *(s16 *)(t + 2);
    r0 = *(s16 *)(t + 4);
    r2 = *(s16 *)(t + 6);
    flps0008(&p0, &r0, &q0);
}
void Lb_put_job(a, b, c, d, e, f)
int a;
int b;
int c;
int d;
int e;
s8 f;
{
    if (f != 0) {
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        Lb_put_icon_free2(a, b, c, d);
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        return;
    }
    Lb_put_icon_free(a, b, c, d);
}
void Lb_put_2TF(u8 *p, int a) {
    struct F5 { f32 a, b, c, d, e; } t;
    t = *(struct F5 *)p;
    *(s16 *)&t.a = 0.8f * (f32) * (s16 *)&t.a;
    if (a != 0) {
        *(s16 *)&t.b = 0.8f * (f32) * (s16 *)&t.b;
    }
    flps0008(&t);
}
void Lb_draw_square(a, x, y, w, color, scale)
int a;
int x;
int y;
int w;
int color;
int scale;
{
    s16 sp[8];
    s16 s3;
    s16 v0;
    s16 s2;
    int s0;
    *(int *)&sp[4] = color;
    s0 = y;
    s2 = 0.8f * (f32)a;
    if (scale != 0) {
        s0 = (s16)(0.8f * (f32)s0);
    }
    sp[2] = s2;
    sp[0] = s2;
    s3 = (s16)x + (s16)w;
    sp[1] = x;
    sp[3] = s3;
    flps0002(sp);
    v0 = s2 + (s16)s0;
    sp[2] = v0;
    sp[0] = v0;
    flps0002(sp);
    sp[3] = x;
    sp[1] = x;
    sp[0] = s2;
    flps0002(sp);
    sp[3] = s3;
    sp[1] = s3;
    flps0002(sp);
}
void lb_put_sprite(void) {
    PLW *pl;
    int a3;
    u8 *e;
    u16 t;
    int r;
    a3 = 0x1E;
    pl = &player_work[game_w.master];
    if (*((u8 *)&lb_sys + 0xA) != 0 || lb_sys.x68 == 8) {
        if (*((u8 *)&lb_sys + 0x28) != 0) {
            a3 = (s16)0x3C;
        }
        Sel_csr_disp(0x14, 0x1C, 0x1F4, a3);
        e = (u8 *)pl->fish878;
        if (e != 0 && lb_sys.x68 == 8) {
            if (*(u16 *)(e + 2) != 6) {
                return;
            }
        }
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        SetFilterMode(1);
        flSetRenderState(0x60, 0);
        e = (u8 *)pl->fish878;
        if (e != 0) {
            t = *(u16 *)(e + 2);
            switch (t) {
            case 18:
                Lb_put_button(0x14, 0x1E, 3);
                Lb_put_icon((s16)((s16)strlen((char *)&lb_sys + 0xA) * 0xA + 0x50), 0x1E, 0xD, -1);
                return;
            case 19:
                r = Lb_check_hotel(0x52);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen((char *)&lb_sys + 0xA) * 0xA + 0x50), 0x1E, 0xD, -1);
                case 1:
                case 3:
                    Lb_put_button(0x14, 0x1E, 3);
                    break;
                }
                r = Lb_check_hotel(0x53);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen((char *)&lb_sys + 0x28) * 0xA + 0x50), 0x38, 0xD, -1);
                case 1:
                case 3:
                    Lb_put_button(0x14, 0x38, 1);
                    return;
                }
                break;
            case 20:
                r = Lb_check_hotel(0x54);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen((char *)&lb_sys + 0xA) * 0xA + 0x50), 0x1E, 0xD, -1);
                case 1:
                case 3:
                    Lb_put_button(0x14, 0x1E, 3);
                    break;
                }
                r = Lb_check_hotel(0x55);
                switch (r) {
                case 2:
                    Lb_put_icon((s16)((s16)strlen((char *)&lb_sys + 0x28) * 0xA + 0x50), 0x38, 0xD, -1);
                case 1:
                case 3:
                    Lb_put_button(0x14, 0x38, 1);
                    return;
                }
                break;
            default:
                Lb_put_button(0x14, 0x1E, 3);
                break;
            }
        }
    }
}
