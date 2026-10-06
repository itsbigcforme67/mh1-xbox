/* cpinet21 - http_wait_thread and CpInetHttpSignalThread (SLPM_654.95 0x002378B0-0x002378C8): both sleep 64 (ms; a guess) through
   CpInetDelayThread. Written new in this pass. */
#include "types.h"
void CpInetDelayThread();

void http_wait_thread(void) {
    CpInetDelayThread(64);
}

void CpInetHttpSignalThread(void) {
    CpInetDelayThread(64);
}
