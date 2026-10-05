/* NEAR-MATCH, not built: reward_itembox is complete but only ~200 of 314
 * instructions differ, all of them register allocation (the original keeps
 * several loop-invariant field addresses of the sprite struct in s1-s6 in
 * another order) and where a movn/branch lands.
 * Quest reward screen, item grid (0x293690-0x293B68): draws the 16-slot
 * reward item list as icons on a background grid, the cursor, and the name /
 * count of the selected item. Meanings are guesses. */
#include "reward.h"

typedef struct RWD_RECT {
    s16 x0, y0, x1, y1;
    u32 col;
} RWD_RECT;

typedef struct RWD_SPR {
    s16 pos[2];
    s16 size[2];
    u32 col;
    s16 uv[4];
} RWD_SPR;

extern u8 reward_item_base[];
extern u32 item_col_tbl[];
extern char *item_str[];
extern char lit_1001_00386A58[];
extern char lit_1002_00386A60[];
extern char lit_1003_00386A70[];
void DispFrameMessageA();
void flps0004();
void flps0008();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void font_print_uf();
int sprintf(char *, const char *, ...);

void reward_itembox(cur, mode, xb)
s16 cur;
u8 mode;
s8 xb;
{
    RWD_RECT rect;
    RWD_SPR spr;
    char buf[0x20];
    s16 i;
    u8 col;
    PL_ITEM *ip;
    u32 alpha;
    int id;
    u8 icon;

    col = (mode == 0) ? 0xFF : 0x60;
    DispFrameMessageA(reward_item_base, 0, col);
    rect.col = 0xFF200000;
    for (i = 0; i < 16; i++) {
        rect.x0 = (s16)(0.8f * (313.0f + 36.0f * (i & 7))) + 4;
        rect.x1 = rect.x0 + 0x19;
        rect.y0 = (i >> 3) * 32 + 0xD4;
        rect.y1 = rect.y0 + 0x1C;
        flps0004(&rect);
    }
    SetFilterMode(0);
    reload_tex(1, 0x118);
    SetTextureStage(0x118);
    alpha = (u32)col << 24;
    spr.size[0] = 0x20;
    spr.size[1] = 0x20;
    for (i = 0, ip = game_w.reward_item; i < 16; i++, ip++) {
        spr.pos[0] = (s16)(0.8f * (313.0f + 36.0f * (i & 7)));
        spr.pos[1] = (i >> 3) * 32 + 0xD2;
        id = ip->id;
        if (id != 0) {
            icon = Item_data[id][5];
            if (icon != 0xFF) {
                spr.uv[0] = ((icon + 1) & 7) * 32 + 1;
                spr.uv[1] = ((icon + 1) >> 3) * 32 + 1;
                spr.uv[2] = ((icon + 1) & 7) * 32 + 0x1F;
                spr.uv[3] = ((icon + 1) >> 3) * 32 + 0x1F;
                spr.col = item_col_tbl[Item_data[id][6]];
                spr.col = (spr.col & 0xFFFFFF) | alpha;
                flps0008(&spr);
            }
        }
    }
    if (cur >= 0) {
        spr.pos[0] = (s16)(0.8f * (313.0f + 36.0f * (cur & 7)));
        spr.pos[1] = (cur >> 3) * 32 + 0xD2;
        *(u32 *)&spr.uv[0] = 0;
        *(u32 *)&spr.uv[2] = 0x200020;
        spr.col = alpha | 0xFFFFFF;
        flps0008(&spr);
        flfntSetSize(0x12, 0x12);
        if (mode != 0) {
            font_set_palette(0xA);
        } else {
            font_set_palette(0);
        }
        id = game_w.reward_item[cur].id;
        if (id == 0) {
            sprintf(buf, item_str[0]);
        } else {
            switch (Item_data[id][3]) {
            case 1:
                sprintf(buf, item_str[id]);
                break;
            case 0xFF:
                sprintf(buf, lit_1001_00386A58, item_str[id]);
                break;
            default:
                sprintf(buf, lit_1002_00386A60, item_str[id], game_w.reward_item[cur].num);
                break;
            }
        }
        flfntLocate((s16)(0x1CB - (unsigned)strlen(buf) * 9 / 2), 0x13B);
        font_print_uf(buf);
        if (xb >= 0) {
            font_set_palette(2);
            flfntLocate(0x13B, 0x125);
            font_print_uf(lit_1003_00386A70);
        }
    }
}
