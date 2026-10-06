/* chat19: DispFrameListOptionArrowC. Run built from the whole-file C in chat_nm.c (context there). */
/* chat_nm - f_chat (SLPM_654.95 0x001755D0-0x0017BF80, main.bin): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Near-match C, not built; matching runs are
 * built from it as chatNN.c. Field meanings are guesses. */
#include "types.h"
#include "menu.h"
#include "ud.h"

#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define F32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define FS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PM ((u8 *)&PitMenu)
void KinshiYogo_chk(char *);
struct PIT_CHAT;
void chat_log_add(int, s8 *, struct PIT_CHAT *);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);

extern u16 System_timer;
f32 flSin(f32);
void flps0004(void *);
void flps0008(void *);
void SetTextureStage(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void flfntLocate(int, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void font_print_sp(void *, ...);
void Put_sprite_rotate(void *, int);
void DispFrameListA(void *, char *, int, int);
void DispFrameList(void *, char *, int);
void DispFrameListOptionArrowC(void *, int);
void DispFrameMessageA(void *, void *, int);
void DispFrameMessage(void *, void *);
void PutButtonICON(void *, int);
void disp_cursorC(s16, s16, s16, s16, s16, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);

typedef struct PFLP4 { s16 p[4]; u32 col; } PFLP4;
typedef struct PFLP8 { s16 p[4]; u32 col; s16 uv[4]; } PFLP8;

/* the highlight bar of row n of a list at y with rows h high, from x0 to
 * x1 (rect {x0, y0, x1, y1}; asm 0x2755D0) */
void DispFrameListOptionArrowC(void *fr, int col) {
    PFLP8 q;

    q.p[2] = 0xE;
    q.p[1] = FS16(fr, 2);
    q.p[3] = F8(fr, 5);
    q.col = col;
    q.p[0] = 0.8f * ((f32)FS16(fr, 0) - 8.0f);
    *(u32 *)&q.uv[0] = 0x1A00A6;
    *(u32 *)&q.uv[2] = 0x2E0094;
    flps0008(&q);
    q.p[0] = 0.8f * ((8.0f + (f32)(FS16(fr, 0) + (F8(fr, 4) * F8(fr, 6)))) - 18.0f);
    q.uv[0] = 0x94;
    q.uv[2] = 0xA6;
    flps0008(&q);
}
