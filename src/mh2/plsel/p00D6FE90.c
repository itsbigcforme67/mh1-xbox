/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

typedef struct {
    char pad[0x39FC];
    int count;
    s16 list[0x20];
} PLSEL_W;

extern PLSEL_W *D_006AE018;

void plsel_func_00D6FE90(s16 v) {
    PLSEL_W *w = D_006AE018;
    if (w->count < 0x20) {
        w->list[w->count] = v;
        w->count++;
    }
}
