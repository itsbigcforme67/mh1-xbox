/* G online (PC): DispSceneTitle, written from Monster Hunter G's code at lobby 0x5A0B30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_2TF();
s32 SetFilterMode();
s32 SetTextureStage();
s32 flSetRenderState();
s32 flfntSetSize();
s32 font_print_double();
s32 lobby_func_0059DA90();
s32 reload_tex();
extern u8 subTitleCol;
extern u8 helpLineTbl;

void DispSceneTitle(void) {
    s32 temp_a0;

    reload_tex(1, 0x180);
    SetTextureStage(0x180);
    SetFilterMode(1);
    flSetRenderState(0x60, 0, M2C_ERROR(/* Read from unset register $a2 */));
    temp_a0 = (*(s32 *)&subTitleCol);
    if (temp_a0 != 0) {
        (*(s16 *)((u8 *)&helpLineTbl + 0x4)) = (((s64) (lobby_func_0059DA90(temp_a0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */), M2C_ERROR(/* Read from unset register $a4 */), M2C_ERROR(/* Read from unset register $a5 */), M2C_ERROR(/* Read from unset register $a6 */), M2C_ERROR(/* Read from unset register $a7 */)) << 0x30) >> 0x30) * 0xC) + 0x30;
        Put_2TF(&helpLineTbl);
        flfntSetSize(0x18, 0x18);
        font_print_double(0x2D, 0x22, 1, 0, (void *) (*(s32 *)&subTitleCol));
    }
}

