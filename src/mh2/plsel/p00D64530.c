/* Monster Hunter 2 plsel.bin. Written from our own disassembly of the owner's disc (docs/mh2.md). */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern u8 D_00D76008[];
int plsel_func_00D64530(int i) { if (i > 3) { i = 4; } return D_00D76008[i]; }
