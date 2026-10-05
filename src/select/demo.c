/* select.bin 0x00533BE0-0x0053452C: title / logo / opening demo (Demo_task and its drawing helpers). */
#include "select.h"

void demo_task_sub(STASK *t);
void violence_logo(void);
void middle_logo(int n);
void capcom_logo(void);
void title_disp(int a);
void opening_demo(void);

void Demo_task(STASK *t) {
    switch (t->step) {
    case 0:
        t->step++;
        all_reset();
        init_demo_work();
        demo_w.mode = 0;
        load_texlist(PIT_TEX[0], 2, 0);
    case 1:
        demo_task_sub(t);
        break;
    }
}

extern char lit_105_0053B300[];

void demo_task_sub(STASK *t) {
    DEMO_W *d = &demo_w;
    switch (demo_w.mode) {
    case 0:
        init_demo_work();
        d->x10 = 0;
        d->mode = 1;
        d->step = 0;
        d->timer = 0x5A;
        fade_set(2);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
        switch (d->step) {
        case 0:
            if (Fade_busy_ck() != 1) {
                d->step++;
                if (d->mode == 6) {
                    d->timer = 0xF0;
                } else {
                    d->timer = 0x3C;
                }
            }
            break;
        case 1:
            if (--d->timer <= 0 && d->x10 == 0) {
                d->step++;
                fade_set(1);
            }
            break;
        case 2:
            if (Fade_busy_ck() != 1 && d->x10 == 0) {
                if (d->mode == 6) {
                    d->mode = 4;
                } else {
                    d->mode++;
                }
                d->step = 0;
                fade_set(2);
                if (d->mode == 4) {
                    Tsk_Execute(Select_task, 1);
                }
            }
            break;
        }
        switch (d->mode) {
        case 1:
            violence_logo();
            break;
        case 2:
            middle_logo(0);
            break;
        case 3:
            middle_logo(1);
            break;
        case 4:
            capcom_logo();
            break;
        case 6:
            title_disp(0xFF);
            if (++((s16 *)t)[7] & 0x20) {
                flfntSetSize(0x18, 0x18);
                font_print_double2(0x68, 0x15C, 2, 0, lit_105_0053B300);
            }
            font_draw();
            break;
        }
        break;
    case 5:
        opening_demo();
        break;
    }
}

extern char lit_112_0053B350[], lit_113_0053B370[];

void violence_logo(void) {
    SPR s;
    reload_tex(1, 6);
    SetTextureStage(6);
    s.w = 0x200;
    s.u2 = 0xFF;
    s.h = 0x1C0;
    s.v2 = 0xFF;
    s.x = 0;
    s.col = 0xFF606060;
    s.y = 0;
    s.u = 0;
    s.v = 0;
    flps0008(&s);
    flfntSetSize(0x1A, 0x1A);
    font_print_double2(0x63, 0xC8, 1, 0, lit_112_0053B350);
    font_print_double2(0x63, 0xF0, 1, 0, lit_113_0053B370);
    font_draw();
}

void capcom_logo(void) {
    SPR s;
    SetFilterMode(0);
    reload_tex(1, 2);
    SetTextureStage(2);
    s.col = -1;
    s.x = 0x34;
    s.y = 0xB0;
    s.w = 0x100;
    s.h = 0x60;
    s.u = 0;
    s.v = 0x30;
    s.u2 = 0xFF;
    s.v2 = 0x8F;
    flps0008(&s);
    s.x += 0x100;
    s.y -= 4;
    s.w = 0x98;
    s.u = 0;
    s.v = 0x90;
    s.u2 = 0x97;
    s.v2 = 0xEF;
    flps0008(&s);
    SetFilterMode(1);
}

void middle_logo(int n) {
    SPR s;
    int tex;
    SetFilterMode(0);
    tex = (n & 0xFF) + 4;
    reload_tex(1, tex);
    SetTextureStage(tex);
    s.w = 0x200;
    s.h = 0x200;
    s.col = -1;
    s.u2 = 0x1FF;
    s.v2 = 0x1FF;
    s.x = 0;
    s.y = 0;
    s.v = 0;
    s.u = 0;
    flps0008(&s);
    SetFilterMode(1);
}

void c_disp(s16 x, s16 y) {
    SPR s;
    reload_tex(1, 8);
    SetTextureStage(8);
    s.v = 0xEE;
    s.w = 0x11;
    s.u2 = 0x12;
    s.h = 0x11;
    s.v2 = 0xFF;
    s.x = x;
    s.col = -1;
    s.y = y;
    s.u = 0;
    Put_2TF(&s);
}

extern char lit_163_0053B3A0[];
extern char lit_164_0053B3D0[];
extern char lit_165_0053B3F0[];

void title_disp(int a) {
    SPR s;
    u8 m;
    SetFilterMode(1);
    reload_tex(1, 3);
    SetTextureStage(3);
    s.x = 0;
    s.y = 0;
    if (system_w.x31 == 0) {
        s.w = 0x280;
    } else {
        s.w = 0x200;
    }
    s.h = 0x1C0;
    s.col = ((a & 0xFF) << 24) | 0xFFFFFF;
    s.u = 1;
    s.v = 1;
    s.u2 = 0xFE;
    s.v2 = 0xFE;
    flps0008(&s);
    flfntSetSize(0x12, 0x12);
    flfntSetHalftype(1);
    m = system_w.x1A;
    switch (m) {
    case 0:
    case 2:
    case 3:
        c_disp(0x6C, 0x178);
        font_print_ex(0x80, 0x178, 0, lit_163_0053B3A0);
        break;
    default:
        c_disp(0x6C, 0x178);
        c_disp(0x6C, 0x18C);
        font_print_ex(0x80, 0x178, 0, lit_164_0053B3D0);
        font_print_ex(0x80, 0x18C, 0, lit_165_0053B3F0);
        break;
    }
}

void opening_demo(void) {
    switch (demo_w.step) {
    case 0:
        demo_w.step++;
        demo_w.mov = 0;
        break;
    case 1:
        demo_w.step++;
        movie_reset();
        movie_start(0);
        movie_request(0, 0);
        demo_w.timer = 0x15F9;
        break;
    case 2:
        demo_w.timer--;
        if (demo_w.timer <= 0) {
            demo_w.step++;
            fade_set(1);
        }
        if (movie_server() != 0) {
            if (demo_w.mov == 0) {
                demo_w.mov++;
                demo_w.timer = 0x15F9;
            }
            movie_draw();
        }
        break;
    case 3:
        if (Fade_busy_ck() != 1) {
            demo_w.step++;
            demo_w.timer = 0x1E;
            movie_exit();
            fade_set(2);
        } else if (movie_server() != 0) {
            movie_draw();
        }
        break;
    case 4:
        if (--demo_w.timer <= 0) {
            demo_w.step = 0;
            demo_w.mode = 6;
            fade_set(2);
        } else if (movie_server() != 0) {
            movie_draw();
        }
        break;
    }
    trans();
}
