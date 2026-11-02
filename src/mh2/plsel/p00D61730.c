/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 D_006ADC93;
extern u8 D_006ADC98;

void plsel_func_00D61730(u8 *p) {
    p[8]++;
    p[9] = 0;
    D_006ADC93 = 0;
    D_006ADC98 = 0;
}
