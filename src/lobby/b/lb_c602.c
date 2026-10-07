/* lb_c602 - agent C round 7 0x005BF090-0x005BF20C: lbc_admin_message_00 (plain x/60, float prototypes, last case falls out). */
#include "lobby_a.h"
extern char lit_3323[];
void cnWrap_SetFontSize(f32);
void cnWrap_FontDisp(f32, f32, f32, char *);
void lbc_admin_message_00(void) {
    char sp10[0x20];
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
        Lbc_init_network_work();
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
        font_set_stack_no(3);
        cnWrap_SetFontColor(0);
        cnWrap_SetFontSize(20.0f);
        temp_v1_3 = (int)cw;
        sprintf(sp10, &lit_3323, F(s16, temp_v1_3, 0x2F74) / 60);
        cnWrap_FontDisp(460.0f, 80.0f, 2.0f, sp10);
    }
}
