/* fl library log output (SLPM_654.95 0x0018D790-0x0018D874): flLogOut formats a line into a 0x800 byte stack buffer, appends CR LF and appends it to the log
 * file (the first call writes the file header instead of appending). The file name and the header are strings in .rodata. */
#include "types.h"
#include "va.h"

extern int bflLogOutFirst_628;
extern char lit_636_0035C0C0[];
extern char lit_637_0035C0D0[];

int vsprintf(char *, const char *, va_list);
int strlen(char *);
void flFileWrite();
void flFileAppend();

int flLogOut(char *fmt, ...) {
    char buf[0x800];
    va_list ap;
    char *p;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    p = buf + strlen(buf);
    *p++ = 0xD;
    *p++ = 0xA;
    *p = 0;
    if (bflLogOutFirst_628 != 0) {
        flFileWrite(lit_636_0035C0C0, lit_637_0035C0D0, strlen(lit_637_0035C0D0));
        bflLogOutFirst_628 = 0;
    }
    flFileAppend(lit_636_0035C0C0, buf, strlen(buf));
    return 1;
}
