/* lb_s23 - parsetag_init 0x005F1760-0x005F19C4: parsetag_init (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 *bsw;
extern char lit_1405_00667348[];
extern char lit_1406_00667350[];
extern char lit_1407_00667358[];
extern char lit_1408_00667360[];

void parsetag_init(s8 arg0) {
    u8 *temp_a0;
    u8 *temp_s0;

    F(s32, bsw, 0) = 0;
    F(s8, bsw, 0x186) = arg0;
    F(s16, bsw, 0x188) = 0;
    F(s8, bsw, 0x17F) = 0;
    F(s32, bsw, 8) = 0;
    F(s32, bsw, 0xC) = 0;
    F(s32, bsw, 4) = 0;
    F(s8, bsw, 0xD8E4) = 0;
    F(s16, bsw, 0xD8C0) = 0;
    F(s16, bsw, 0xD8BC) = 0;
    F(s16, bsw, 0xD8C2) = 0;
    F(s16, bsw, 0xD8BE) = 0;
    F(s16, bsw, 0x18) = 0;
    F(s16, bsw, 0xD8C8) = 0;
    F(s8, bsw, 0xD8CC) = 0;
    F(s32, bsw, 0x1C) = 0;
    F(s16, bsw, 0xD890) = 0;
    F(s8, bsw, 0xD892) = 0;
    F(s16, bsw, 0xD894) = 0;
    F(s16, bsw, 0xD896) = 0;
    F(s8, bsw, 0x18B) = 0;
    F(s8, bsw, 0x18C) = 0;
    F(s8, bsw, 0x18D) = 0;
    F(s8, bsw, 0x14) = 0;
    F(u8, bsw, 0x17C) = 0U;
    F(s8, bsw, 0x17E) = 0;
    F(s8, bsw, 0x17D) = 0;
    F(s16, bsw, 0x12) = 0;
    strcpy(bsw + 0x2A3, &lit_1405_00667348);
    strcpy(bsw + 0x2B3, &lit_1406_00667350);
    strcpy(bsw + 0x193, &lit_1407_00667358);
    strcpy(bsw + 0x1A3, &lit_1408_00667360);
    F(s16, bsw, 0x124) = 0;
    temp_a0 = bsw;
    BSC1(s8, temp_a0, F(s16, temp_a0, 0x124), 0x168) = 3;
    temp_s0 = bsw;
    BSC4(s32, temp_s0, F(s16, temp_s0, 0x124), 0x128) = get_numeric_parameter5(temp_s0 + 0x2A3);
    F(s32, bsw, 0x120) = get_numeric_parameter5(bsw + 0x193);
    F(s32, bsw, 0x178) = get_numeric_parameter5(bsw + 0x2B3);
    DispFontSize(F(u8, bsw, 0x17C));
    memset(bsw + 0x1120, 0, 0x108);
    F(s8, bsw, 0x18E) = 0;
    F(s8, bsw, 0x18F) = 0;
    F(s8, bsw, 0x190) = 0;
    F(s8, bsw, 0x191) = 0;
    F(s8, bsw, 0x192) = 0;
    memset(bsw + 0xFAC1, 0, 0x100);
}
