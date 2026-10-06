/* lb_by125 - agent B 0x0053BFD0-0x0053C1A8: Lb_put_armorIcon (equipment icon of a shop list entry: weapon/armor type icon or job icon). */
#include "lobby_s.h"
void Lb_put_job();
void Lb_put_icon_free2();
void Lb_put_armorIcon(int x, int y, int z, int kind, int id) {
    int icon;
    int k;
    int uid;
    int col;

    k = (s16)kind;
    if (k == 7 || k == 6) {
        if ((s16)id == 0x3E7) {
            Lb_put_job(x, y, z, -1, 5, 1);
            return;
        }
        uid = id & 0xFFFF;
        col = Equip_icon_color_rare(Get_equip_rare(kind & 0xFF, uid) & 0xFF, 0xFF, 0);
        Lb_put_job(x, y, z, col, Get_weapon_job2((u8)kind, uid) & 0xFF, 1);
        return;
    }
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    switch (k) {
    case 0:
        icon = 0x17;
        break;
    case 2:
        icon = 0x13;
        break;
    case 3:
        icon = 0x14;
        break;
    case 4:
        icon = 0x16;
        break;
    case 5:
        icon = 0x15;
        break;
    }
    Lb_put_icon_free2(x, y, z, Equip_icon_color_rare(Get_equip_rare(kind & 0xFF, id & 0xFFFF) & 0xFF, 0xFF, 0), (s16)icon);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
}
