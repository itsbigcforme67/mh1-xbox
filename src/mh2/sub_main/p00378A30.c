/* Monster Hunter 2 sub_main 0x378A30: MH2's stage_set_set (G: src/main/stage/stage_set.c, sub_main 0x4A0E50). Written from our own
   disassembly of the owner's disc. Spawns each stage's set objects when a stage is entered. Against G: the sun block takes fixed
   values (G read them from tables per stage), a 4-byte table per stage (0x4485D0) names four more spawns (0xFF = none), and the
   switch covers MH2's stages (up to 0xC4; the day/night copies of a stage share a case). MWCC emits this sparse switch as a compare
   chain in reverse case order, so the case order below is the original's. Game-side callees go by address (func_6F4900 ...:
   splat's build/mh2/auto/sub_main_undefined_funcs_auto.txt defines them); the names in comments are the map's
   (docs/survey/mh2_map.csv), not checked. */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef float f32;

typedef struct {
    u8 _pad00[0x4D];
    u8 x4D;             /* 0x4D environment block version: 2 = the stage file gave the sun */
    u8 _pad4E[0xC8 - 0x4E];
    f32 sun_pos[3];     /* 0xC8 */
    s16 sun_tb[2];      /* 0xD4 */
    int sun_col[8];     /* 0xD8 */
    u8 _padF8[0xF9 - 0xF8];
    u8 xF9;             /* 0xF9 */
    u8 _padFA[0xFD - 0xFA];
    u8 xFD;             /* 0xFD cleared when the stage has no fourth table spawn */
} STGW2;

extern STGW2 stage_work;
extern u8 D_004485D0[][4];
extern u8 D_006ADCF1;

void clr_flash();
void sub_main_func_00383130();
void sub_main_func_003789C0();
void set12_set();
void func_6F4900();             /* set09_set */
void func_6FCCD0();
void func_88B3F0();
void func_6F9830();
void func_6F1540();             /* Set05_set */
void func_6F0AB0();             /* Set04_set */
void func_6D36C0();             /* Shell10_set in G's place */
void func_6FE4B0();             /* Set20_set */
void func_86BBC0();             /* Eft14_set2 in G's place */
void func_6FEEA0();             /* Set22_set */
void func_6F9CA0();
void func_6EE990();             /* set00_set */
void func_6F2A60();
void func_6FA530();
void func_6FC760();
void func_6EF8F0();
void func_7DA940();
void func_6BE840();
void func_6EF280();             /* Set13_set */
void func_6F4470();
void func_700670();
void func_6FC350();

void stage_set_set(int stage) {
    STGW2 *w = &stage_work;
    f32 pos[3];
    s16 i;

    if (stage_work.x4D < 2) {
        w->sun_pos[0] = 5000.0f;
        w->sun_pos[1] = 5000.0f;
        w->sun_pos[2] = 5000.0f;
        w->sun_tb[0] = 0;
        w->sun_tb[1] = 0;
        w->sun_col[0] = w->sun_col[1] = w->sun_col[2] = w->sun_col[3] = w->sun_col[4] = w->sun_col[5] = w->sun_col[6] =
            w->sun_col[7] = 0xFFFFFF;
    }
    clr_flash();
    if (D_004485D0[(u16)stage][0] != 0xFF) {
        sub_main_func_00383130(D_004485D0[(u16)stage][0]);
    }
    if (D_004485D0[(u16)stage][1] != 0xFF) {
        func_6F4900();
    }
    if (D_004485D0[(u16)stage][2] != 0xFF) {
        func_6FCCD0(D_004485D0[(u16)stage][2]);
    }
    if (D_004485D0[(u16)stage][3] != 0xFF) {
        func_88B3F0(D_004485D0[(u16)stage][3]);
    } else {
        w->xFD = 0;
    }
    switch ((u16)stage) {
    case 0xA:
    case 0x4C:
        func_6F9830();
        break;
    case 0xB:
        func_6F1540(0);
        break;
    case 0xC:
        func_6F1540(1);
        break;
    case 0x4E:
        func_6F1540(6);
        break;
    case 0xE:
    case 0x4F:
        func_6F0AB0();
        break;
    case 0x11:
        for (i = 0; i < 10; i++) {
            pos[0] = 1400.0f + 300.0f * (f32)i;
            pos[1] = 30.0f;
            pos[2] = 2000.0f + 300.0f * (f32)i;
            func_6D36C0(pos, 0, stage, 0);
        }
        break;
    case 0x14:
        func_6FE4B0(0);
        break;
    case 0x15:
    case 0xB8:
        pos[0] = 11250.0f;
        pos[1] = 40.0f;
        pos[2] = 9500.0f;
        func_86BBC0(pos, 2);
        func_86BBC0(pos, 3);
        break;
    case 0x19:
        func_6F1540(2);
        func_6FE4B0(1);
        func_6FEEA0();
        break;
    case 0x1C:
    case 0x50:
        func_6F1540(3);
        func_6F0AB0();
        func_6F9CA0();
        break;
    case 0x1E:
        func_6F1540(4);
        break;
    case 0x51:
        func_6F1540(7);
        break;
    case 0x29:
    case 0xC2:
        func_6EE990();
        break;
    case 0x2A:
    case 0xC3:
        func_6EE990();
        func_6F2A60();
        func_6FA530();
        break;
    case 0x2B:
    case 0xC4:
        func_6EE990();
        break;
    case 0x35:
        func_6FA530();
        break;
    case 0x48:
    case 0x4B:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x51, 0x11, pos, 0x3C, 3);
        break;
    case 0x4D:
        func_6F1540(5);
        break;
    case 0x56:
        break;
    case 0x2E:
    case 0x59:
        func_6F0AB0();
        break;
    case 0x39:
    case 0x5A:
        func_6FA530();
        func_6F0AB0();
        break;
    case 0x42:
    case 0x5B:
        func_6F0AB0();
        break;
    case 0x5C:
    case 0x65:
        func_6EE990();
        break;
    case 0x5E:
    case 0x67:
        func_6EE990();
        break;
    case 0x61:
    case 0x6A:
        func_6F2A60();
        func_6F0AB0();
        break;
    case 0x69:
    case 0x6D:
        func_6F2A60();
        break;
    case 0x1:
    case 0x6E:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x48, 0x11, pos, 0xB4, 1);
        break;
    case 0x2:
    case 0x6F:
        func_6FC760();
        break;
    case 0x4:
    case 0x71:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x48, 0x11, pos, 0xB4, 1);
        break;
    case 0x5:
    case 0x72:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x48, 0x11, pos, 0xB4, 1);
        break;
    case 0x13:
    case 0x74:
        func_6FC760();
        break;
    case 0x78:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x48, 0x11, pos, 0xB4, 1);
        break;
    case 0x82:
        func_6EE990();
        func_6EF8F0();
        break;
    case 0x83:
        if (D_006ADCF1 != 0) {
            sub_main_func_003789C0(7);
            if (w->xF9 != 0) {
                func_7DA940();
            }
            func_6BE840(0);
        }
        break;
    case 0x84:
        sub_main_func_003789C0(6);
        break;
    case 0x85:
        sub_main_func_003789C0(2);
        break;
    case 0x87:
        break;
    case 0x88:
        sub_main_func_003789C0(2);
        break;
    case 0x89:
        sub_main_func_003789C0(1);
        func_6BE840(1);
        break;
    case 0x30:
    case 0x8E:
        func_6FA530();
        break;
    case 0x31:
        func_6EF280(0);
    case 0x8F:
        func_6FA530();
        break;
    case 0x32:
    case 0x90:
        func_6EE990();
        func_6EF8F0();
        break;
    case 0x37:
    case 0x95:
        func_6FA530();
        break;
    case 0x38:
    case 0x96:
        func_6EF8F0();
        func_6FA530();
        break;
    case 0x44:
    case 0x45:
    case 0x9A:
    case 0x9B:
        func_6EE990();
        func_6EF8F0();
        break;
    case 0x46:
    case 0x9C:
        func_6F4470();
        break;
    case 0x49:
    case 0x9F:
        func_6EF8F0();
        break;
    case 0x3A:
    case 0xA1:
        pos[0] = 0.0f;
        pos[1] = 0.0f;
        pos[2] = 0.0f;
        set12_set(7, 0x48, 0x11, pos, 0xB4, 1);
        break;
    case 0xA2:
    case 0xA3:
    case 0xA6:
    case 0xA7:
        func_700670();
        break;
    case 0xAA:
        sub_main_func_003789C0(1);
        break;
    case 0xAB:
        sub_main_func_003789C0(1);
        break;
    case 0xAE:
        sub_main_func_003789C0(1);
        break;
    case 0xAF:
        sub_main_func_003789C0(1);
        break;
    case 0xB0:
    case 0xB4:
        func_6F1540(8);
        break;
    case 0xB2:
    case 0xB6:
        func_6F1540(9);
        break;
    case 0xB3:
        func_6FC350();
        break;
    case 0xB7:
        func_6FC350();
        func_6F2A60();
        break;
    }
}
