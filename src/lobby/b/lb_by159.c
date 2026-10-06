/* lb_by159 - agent B 0x005AF2C0-0x005AF42C: Lb_put_itemIcon (item icon sprite of a shop list entry, plus the 'new' badge). */
#include "lobby_s.h"
extern u32 item_col_tbl[];
void Lb_put_2TF();
/* original bytes: build/raw/Lb_put_itemIcon.inc (config/c_rawfuncs.txt); the C below is a near-match, used by the PC build */
#ifdef __MWERKS__
asm void Lb_put_itemIcon(s16 x, s16 y, s16 z, int id)
{
#include "Lb_put_itemIcon.inc"
}
#else
void Lb_put_itemIcon(s16 x, s16 y, s16 z, int id) {
    s16 spr[10];
    int icon;

    icon = (&Item_data[0][5])[id * 16] + 1;
    spr[3] = z;
    spr[1] = y;
    spr[0] = x;
    spr[2] = (s16)(1.25f * z);
    spr[6] = (icon & 7) * 32 + 1;
    spr[7] = (icon >> 3) * 32 + 1;
    spr[8] = (icon & 7) * 32 + 0x1F;
    spr[9] = (icon >> 3) * 32 + 0x1F;
    *(u32 *)&spr[4] = item_col_tbl[(&Item_data[0][6])[id * 16]];
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    Lb_put_2TF(spr, 1);
    if ((&Item_data[0][4])[id * 16] & 2) {
        spr[6] = 0x80;
        spr[7] = 0xE0;
        spr[8] = spr[6] + 0x20;
        spr[9] = spr[7] + 0x20;
        *(u32 *)&spr[4] = -1;
        Lb_put_2TF(spr, 1);
    }
}
#endif
