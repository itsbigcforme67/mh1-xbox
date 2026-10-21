/* lb_by125 - agent B 0x0053BFD0-0x0053C1A8: Lb_put_armorIcon (equipment icon of a shop list entry: weapon/armor type icon or job icon). */
#include "lobby_s.h"
unsigned char Get_equip_rare();   /* its real return type: was implicitly int (tools/check_kr_returns.py) */
unsigned char Get_weapon_job2();   /* its real return type: was implicitly int (tools/check_kr_returns.py) */
#ifdef VERSION_MHG
#define ICON(mh1, g) (g)       /* G's icon sheet starts one cell earlier: every armour icon is one further */
#else
#define ICON(mh1, g) (mh1)
#endif
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
            #ifdef VERSION_MHG
            Lb_put_job(x, y, z, -1, 7, 1);      /* G: the job icon sheet has two more entries */
#else
            Lb_put_job(x, y, z, -1, 5, 1);
#endif
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
        icon = ICON(0x17, 0x18);
        break;
    case 2:
        icon = ICON(0x13, 0x14);
        break;
    case 3:
        icon = ICON(0x14, 0x15);
        break;
    case 4:
        icon = ICON(0x16, 0x17);
        break;
    case 5:
        icon = ICON(0x15, 0x16);
        break;
    }
    Lb_put_icon_free2(x, y, z, Equip_icon_color_rare(Get_equip_rare(kind & 0xFF, id & 0xFFFF) & 0xFF, 0xFF, 0), (s16)icon);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
}
