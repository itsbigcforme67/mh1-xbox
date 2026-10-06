/* sk17 - f_sk 0x0025F980-0x0025F9B8: Softkey_free_0 and Softkey_free_1 (soft keyboard free-text hooks; _1 asks the player to do chat action n+1). */
#include "types.h"

extern u8 *lpSKey;
#define SKB(o) (*(u8 *)(lpSKey + (o)))
#define SKS8(o) (*(s8 *)(lpSKey + (o)))
#define SKP(o) (*(u8 **)(lpSKey + (o)))

void Pl_chat_act_req(int);
u8 *getPS2KbData(void);

int Softkey_free_0(void *a, void *b) {
    return 0;
}

int Softkey_free_1(void *a, void *b, int c) {
    Pl_chat_act_req((u8)((u8)c + 1));
    return 0;
}
