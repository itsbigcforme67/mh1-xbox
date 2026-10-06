/* cpinet22 - CpInetDelayThread (SLPM_654.95 0x00237900-0x0023794C): sleeps the calling thread for ms milliseconds with an alarm
   (al_cbfunc wakes it up). K&R definition: callers in cpinet21 pass the argument through. Written new in this pass. */
#include "types.h"
int GetThreadId();
int SetAlarm();
int SleepThread();
int CancelWakeupThread();
void al_cbfunc();
void CpInetDelayThread();

void CpInetDelayThread(ms)
int ms;
{
    int id;

    id = GetThreadId();
    SetAlarm(ms & 0xFFFF, al_cbfunc, &id);
    SleepThread();
    CancelWakeupThread(id);
}
