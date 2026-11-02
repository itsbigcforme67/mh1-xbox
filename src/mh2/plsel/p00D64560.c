/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 *D_00D7BD80;
extern u8 D_006ADCBE;
extern u8 D_005BB0AC;

void plsel_func_00D64560(void) {
    D_00D7BD80[0x48] = 0;
    D_006ADCBE = 0;
    D_005BB0AC = 0;
}
