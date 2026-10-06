/* lbui - lobby.bin 0x00590D40-0x0059DB40: plaza / lobby UI (eat scene, dialogs, plaza menus, chat, mail, friends).
 * Near-match file in address order (tools/lbmerge.py ... include/lbui_proto.h lbui.h). */
#pragma readonly_strings on
#include "lbui_proto.h"


/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();

/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */

void Paint_square();

/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */

void put_button_help(int a, int b, int c, u16 d);

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

void event_eat_trans_ot0(a)
u8 *a;
{
    int x;
    s16 y;
    s16 i;

    font_set_stack_no(*(int *)(a + 0x18));
    if (LBS8(6) == 3) {
        x = pfl_menu_449[0];
        y = pfl_menu_449[1];
        flfntSetSize(0x12, 0x12);
        font_set_palette(0);
        i = 0;
        do {
            y += 0x16;
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[i]);
            i++;
        } while (i < 10);
        switch (pNet->depth) {
        case 0:
            DispFrameList(pfl_menu_449, lit_474_0065B950, pNet->menu);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(3);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_476_0065B970);
            break;
        case 1:
            DispFrameList(pfl_menu_449, lit_474_0065B950, pNet->cur);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(0);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->menu]);
            DispFrameMessage(frame_matB_451, 0);
            font_set_palette(5);
            flfntLocate(frame_matB_451[0], frame_matB_451[1]);
            font_print(lit_477_0065B980);
            font_set_palette(3);
            y = frame_matB_451[1] + 0x16;
            x = frame_matB_451[0];
            flfntLocate(x, y);
            font_print(lit_476_0065B970);
            break;
        case 2:
            DispFrameList(pfl_menu_449, lit_474_0065B950, -1);
            DispFrameMessage(frame_matA_450, 0);
            font_set_palette(5);
            flfntLocate(frame_matA_450[0], frame_matA_450[1]);
            font_print(lit_475_0065B960);
            font_set_palette(0);
            y = frame_matA_450[1] + 0x16;
            x = frame_matA_450[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->menu]);
            DispFrameMessage(frame_matB_451, 0);
            font_set_palette(5);
            flfntLocate(frame_matB_451[0], frame_matB_451[1]);
            font_print(lit_477_0065B980);
            font_set_palette(0);
            y = frame_matB_451[1] + 0x16;
            x = frame_matB_451[0];
            flfntLocate(x, y);
            font_print(lit_473_0065B948, eat_data_name[pNet->cur]);
            DispFrameList(eat_command_452, 0, (u8)pNet->x0A);
            break;
        }
    }
}
