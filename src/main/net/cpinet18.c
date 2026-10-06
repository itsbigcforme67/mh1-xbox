/* cpinet18 - CpInetPppGetATScript (SLPM_654.95 0x00235CD0-0x00235DF4): builds the modem AT script text for a provider kind
   (1 = empty, 2 = dial-up: the Conex/Suntac script or a default "send/wait" script) and returns the two script buffers in out[0], out[1].
   Parameter meanings are guesses from use (kind, provider; c and d are not used; out is the 5th argument). */
#include "types.h"
extern char PppAtScript0[0x100];
extern char PppAtScript1[0x100];
extern char _ConexScriptBase[0x100];
extern char _SuntacScriptBase[0x100];
extern char lit_937_0036CE50[];
char *strcat();
int sprintf();
int CpInetPppGetATScript(int kind, int prov, int c, int d, char **out) {
    switch (kind) {
    case 2:
        PppAtScript0[0] = 0;
        switch (prov) {
        case 2:
        case 5:
            strcat(PppAtScript0, _ConexScriptBase);
            break;
        case 4:
            strcat(PppAtScript0, _SuntacScriptBase);
            break;
        default:
            sprintf(PppAtScript0, lit_937_0036CE50);
            break;
        }
        PppAtScript1[0] = 0;
        out[0] = PppAtScript0;
        out[1] = PppAtScript1;
        break;
    case 3:
        break;
    case 1:
        PppAtScript0[0] = 0;
        PppAtScript1[0] = 0;
        out[0] = PppAtScript0;
        out[1] = PppAtScript1;
        break;
    }
    return 0;
}
