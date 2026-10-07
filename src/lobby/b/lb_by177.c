/* lb_by177 - 0x0053BE00-0x0053BF30: Lb_put_materialItem (material row of the forge list: name, owned/stock counts, colour by availability). id is reused for the stock count (the original reuses its register). */
#include "lobby_a.h"
extern char lit_1287_006555F0[];
extern char item_str[];
void Lb_put_materialItem(y, id, need)
int y;
int id;
int need;
{
    int num;
    int r;
    int u;

    u = id & 0xFFFF;
    num = (s16)Ud_item_num_ck(u);
    r = Ud_stock_item_num_ck3(u);
    u = (s16)id;
    id = (s16)r;
    if (u != 0) {
        if ((s16)id > 99) {
            id = 99;
        }
        if ((s16)need <= (s16)num) {
            font_set_palette(0);
        } else if ((s16)need <= (s16)num + (s16)id) {
            font_set_palette(6);
        } else {
            font_set_palette(10);
        }
        flfntLocate(0x12C, y);
        font_print(&lit_1287_006555F0, ((s32 *)&item_str)[u], (s16)num, (s16)id, (s16)need);
    }
}
