/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 *D_006AE018;
void *memset(void *p, int c, unsigned n);

void plsel_func_00D6F8B0(void) {
    memset(D_006AE018, 0, 0x3B88);
}
