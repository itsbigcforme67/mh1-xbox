#include "lobby_a.h"
extern char D_3C738C[];
extern char D_3C738C[];
extern char D_3C738C[];
extern char D_3C738C[];
void Lb_set_mini_data(int arg0) {
    u8 sp46;
    u8 sp45;
    u8 sp44;
    int sp3E;
    int sp38;
    s32 sp34;
    u8 sp33;
    u8 sp32;
    u8 sp31;
    u8 sp30;
    int temp_s0;

    temp_s0 = (int)&player_work + (game_w.master * 0xA00);
    sp34 = F(s32, temp_s0, 0x5FC);
    memcpy(&sp3E, temp_s0 + 0x352, 6);
    sp33 = F(u8, temp_s0, 0x11);
    sp32 = F(u8, temp_s0, 0x915);
    sp45 = F(u8, temp_s0, 0x916);
    sp31 = *(u8 *)0x3C733B;
    F(s16, &sp38, 0) = (s16) F(s16, &D_3C738C, 0);
    F(s16, &sp38, 2) = (s16) F(s16, &D_3C738C, 2);
    F(s16, &sp38, 4) = (s16) F(s16, &D_3C738C, 4);
    sp30 = Get_weapon_job(&D_3C738C, F(s16, &D_3C738C, 0), &sp38);
    if (sp30 == 5) {
        sp30 = 1;
    }
    sp44 = *(u8 *)0x3C6FC3;
    sp46 = *(u8 *)0x3C7397;
    memcpy(arg0, &sp30, 0x18);
}
