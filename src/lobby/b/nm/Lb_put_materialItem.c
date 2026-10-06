#include "lobby_a.h"
extern char lit_1287_006555F0[];
extern char item_str[];
void Lb_put_materialItem(y, id, need)
int y;
int id;
int need;
{
    int num;
    int new_var;
    int stock;
    int idx;
    int u;

    u = id & 0xFFFF;
    num = (s16)Ud_item_num_ck(u);
    stock = (s16)Ud_stock_item_num_ck3(u);
    idx = (s16)id;
    if (idx != 0) {
        if ((s16)stock >= 100) {
            stock = 99;
        }
        num = (new_var = (s16)num);
        need = (s16)need;
        if (need <= num) {
            font_set_palette(0);
        } else if (num + (s16)stock >= need) {
            font_set_palette(6);
        } else {
            font_set_palette(10);
        }
        flfntLocate(0x12C, y);
        font_print(&lit_1287_006555F0, ((s32 *)&item_str)[idx], new_var, (s16)stock, need);
    }
}
