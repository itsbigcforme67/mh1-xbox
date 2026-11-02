/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 D_006ADC98;

u8 plsel_func_00D61710(void) {
    return D_006ADC98 + 1;
}
