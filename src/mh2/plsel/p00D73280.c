/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 *D_006AE018;

int plsel_func_00D73280(void) {
    int i;
    u8 *p;
    i = 0;
    p = D_006AE018;
    do {
        if (*(u16 *)(p + 0x3984) != 0) {
            return 1;
        }
        i++;
        p += 2;
    } while (i < 0x39);
    return 0;
}
