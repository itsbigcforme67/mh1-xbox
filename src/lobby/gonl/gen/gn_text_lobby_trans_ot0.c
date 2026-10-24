/* G online (PC): text_lobby_trans_ot0, written from Monster Hunter G's code at lobby 0x57A210 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispHelpLine();
s32 DispSceneTitle();
s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 font_set_stack_no();
s32 lobby_func_005936B0();
s32 reload_tex();
extern u8 CnetWork;
extern u8 cw;
extern u8 textLobbyTbl;
s32 Disp_back();                         /* extern */
s32 Online_ck();                                    /* extern */

void text_lobby_trans_ot0(void *arg0) {
    u8 temp_a0;
    u8 temp_v1;
    u8 *temp_a1;
    u8 *temp_v0;

    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    if (Online_ck() == 1) {
        temp_v0 = (*(void **)&cw);
        temp_v1 = (*(u8 *)((u8 *)(temp_v0) + 0xB99));
        if ((temp_v1 == 0) && ((s32) (*(u8 *)((u8 *)(temp_v0) + 0xB9B)) < 7) && ((*(u8 *)((u8 *)&CnetWork + 0x5)) == 0)) {
            reload_tex(1, 0x174);
            SetTextureStage(0x174);
            SetFilterMode(1);
            flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
            Put_2TF(&(*(s32 *)((u8 *)&textLobbyTbl + 0x28)));
        } else if (((u32) (temp_v1 - 5) < 2U) || (temp_v1 == 3)) {
            Disp_back(1);
        } else {
            lobby_func_005936B0(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
    } else {
        reload_tex(1, 0x140);
        SetTextureStage(0x140);
        Put_2TF(&(*(s32 *)((u8 *)&textLobbyTbl + 0x28)));
    }
    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    temp_a1 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) {
        temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB9B));
        if ((temp_a0 == 4) && (temp_a0 == 3)) {
            DispSceneTitle((s32) temp_a0, (s32) temp_a1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            DispHelpLine(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
    }
}

