/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 *D_006AE018;

void plsel_func_00D63A20(void) {
    u8 *p = D_006AE018;
    if (*(int *)(p + 4) != 0) {
        *(int *)(p + 4) -= 1;
    }
    if (*(s16 *)(p + 0x13D8) != 0) {
        *(s16 *)(p + 0x13D8) -= 1;
    }
}
