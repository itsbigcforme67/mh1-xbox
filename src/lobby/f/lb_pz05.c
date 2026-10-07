/* lb_pz05 - lobby.bin 0x005997C0-0x00599B08: Lb_put_new_mail, blinking new-mail icon next to the first unread RecvMailInfo slot (pNet->idx is the animation counter; the float fade is converted with (u32)). */
#pragma readonly_strings on
#include "lbui_proto.h"
int Lb_put_icon();
int SetTextureStage();
extern u8 RecvMailInfo[][0x9A];

void Lb_put_new_mail(x, y)
int x;
int y;
{
    s8 i;
    f32 a;
    u8 *p;
    int ys;
    s16 f;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    i = 0;
    p = (u8 *)RecvMailInfo;
    do {
        if (*p != 0) {
            if (++pNet->idx & 8) {
                a = 255.0f - 36.42857f * (f32)(pNet->idx & 7);
            } else {
                a = 36.42857f * (f32)(pNet->idx & 7);
            }
            ys = (s16)y;
            Lb_put_icon(x, (s16)(ys + 2), 9, -1);
            f = pNet->idx;
            if (f & 0x10) {
                if (f & 0x80) {
                    Lb_put_icon((s16)((s16)x - 0x10), (s16)(ys - 6), 0xC, ((u32)a << 24) | 0xFFFFFF);
                } else {
                    Lb_put_icon((s16)((s16)x - 0x10), (s16)(ys + 8), 0xC, ((u32)a << 24) | 0xFFFFFF);
                }
            } else {
                if (f & 0x80) {
                    Lb_put_icon((s16)((s16)x + 0xA), (s16)(ys + 0xA), 0xC, ((u32)a << 24) | 0xFFFFFF);
                } else {
                    Lb_put_icon((s16)((s16)x + 0xA), (s16)(ys - 8), 0xC, ((u32)a << 24) | 0xFFFFFF);
                }
            }
            break;
        }
        i++;
        p += 0x9A;
    } while (i < 8);
}
