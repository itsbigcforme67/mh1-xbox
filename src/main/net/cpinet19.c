/* cpinet19 - CpInetHttpInitialize (SLPM_654.95 0x002377F0-0x002378AC): creates the two HTTP semaphores and clears the file descriptor / cancel
   tables and the error numbers. Written new in this pass. */
#include "types.h"
typedef struct SEMAP { int cur, max, init, wait, attr, opt; } SEMAP;
void *memset();
int CreateSema();
extern s32 Cancel_id[3];
extern s32 Err_hno[4];
extern s32 Err_no[4];
extern s32 Fd_tbl[3];
extern s32 Inet_http_static_sema[4];
extern s32 Inet_http_thread_sema[4];

void CpInetHttpInitialize(void) {
    SEMAP sp;

    if (Inet_http_static_sema[0] == 0) {
        memset(&sp, 0, 0x18);
        sp.init = 1;
        sp.max = 1;
        Inet_http_static_sema[0] = CreateSema(&sp);
    }
    if (Inet_http_thread_sema[0] == 0) {
        memset(&sp, 0, 0x18);
        sp.max = 1;
        sp.init = 0;
        Inet_http_thread_sema[0] = CreateSema(&sp);
    }
    memset(Fd_tbl, 0xFF, 0xC);
    memset(Cancel_id, 0xFF, 0xC);
    Err_no[0] = 0;
    Err_hno[0] = 0;
}
