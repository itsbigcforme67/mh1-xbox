#include "lobby_a.h"
extern char lit_3323[];
void lbc_admin_message_00(void) {
    char sp10[0x2F74];
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;
    int temp_a0_3;
    int temp_a1_2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2F6F;
    temp_a0_2 = F(u8, temp_a0, 0x2F6F);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2F6F) = (u8) (temp_a0_2 + 1);
        Lbc_init_network_work(temp_a0_2, temp_a1);
        Info_Initialization();
        /* fallthrough */
    case 1:
        temp_a0_3 = (int)cw;
        F(u8, temp_a0_3, 0x2F6F) = (u8) (F(u8, temp_a0_3, 0x2F6F) + 1);
        F(s8, (u8 *)cw, 0x2C5C) = 2;
        SetDialogData_HTML((u8 *)cw + 0x2C6E);
        /* fallthrough */
    case 2:
        temp_a1_2 = (int)cw;
        F(u8, temp_a1_2, 0x2F6F) = (u8) (F(u8, temp_a1_2, 0x2F6F) + 1);
        F(s16, (u8 *)cw, 0x2F74) = 0x258;
        return;
    case 3:
        F(s8, pNet, 0xC) = 1;
        temp_v1 = (int)cw;
        F(s16, temp_v1, 0x2F74) = (s16) (F(s16, temp_v1, 0x2F74) - 1);
        temp_v1_2 = (int)cw;
        if (F(s16, temp_v1_2, 0x2F74) == 0) {
            F(u8, temp_v1_2, 0x2F6E) = (u8) (F(u8, temp_v1_2, 0x2F6E) + 1);
            F(u8, (u8 *)cw, 0x2F6F) = 0U;
            F(s16, (u8 *)cw, 0x2F74) = 0;
        }
        font_set_stack_no(3, temp_a1);
        cnWrap_SetFontColor(0);
        cnWrap_SetFontSize(0x41A00000);
        temp_v1_3 = (int)cw;
        sprintf(sp10, &lit_3323, (F(s16, temp_v1_3, 0x2F74) / 60) + ((u32) F(s16, temp_v1_3, 0x2F74) >> 0x1F));
        cnWrap_FontDisp(0x43E60000, 0x40000000, sp10);
        return;
    }
}
