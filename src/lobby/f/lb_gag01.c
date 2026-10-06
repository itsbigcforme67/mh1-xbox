/* lb_gag01 - stage 0x005D7B80-0x005D7C24: Lb_put_2TF. Whole file in lb_ag.c. */
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

void Lb_put_2TF(u8 *p, int a) {
    struct F5 { f32 a, b, c, d, e; } t;
    t = *(struct F5 *)p;
    *(s16 *)&t.a = 0.8f * (f32) * (s16 *)&t.a;
    if (a != 0) {
        *(s16 *)&t.b = 0.8f * (f32) * (s16 *)&t.b;
    }
    flps0008(&t);
}
