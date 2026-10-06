#include "lobby_a.h"

void Lb_put_armorIcon(s32 arg0, int arg1, int arg2, int arg3, int arg4) {
    s32 temp_s0_2;
    s32 temp_s0_3;
    int temp_s0;
    int temp_s2;

    temp_s0 =  (arg3 << 0x30) >> 0x30;
    temp_s2 = arg4;
    if (temp_s0 != 7) {
        if (temp_s0 == 6) {
            goto block_3;
        }
        reload_tex(1, 0x118);
        SetTextureStage(0x118);
        switch (temp_s0) {                          /* irregular */
        case 0:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        }
        Lb_put_icon_free2(arg0, arg1, arg2, Equip_icon_color_rare(Get_equip_rare(arg3 & 0xFF, temp_s2 & 0xFFFF) & 0xFF, 0xFF, 0));
        reload_tex(1, 0x157);
        SetTextureStage(0x157);
        return;
    }
block_3:
    if (( (temp_s2 << 0x30) >> 0x30) == 0x3E7) {
        Lb_put_job(arg0, arg1, arg2, -1);
        return;
    }
    temp_s0_2 = temp_s2 & 0xFFFF;
    temp_s0_3 = Equip_icon_color_rare(Get_equip_rare(arg3 & 0xFF) & 0xFF, 0xFF, 0);
    Get_weapon_job2(arg3 & 0xFF);
    Lb_put_job(arg0, arg1, arg2);
}
