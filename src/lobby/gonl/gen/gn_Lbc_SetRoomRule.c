/* G online (PC): Lbc_SetRoomRule (G lobby 0x5821B0; HAND-FIXED: m2c's unrolled copy of the chosen rules read the stack
 * pointer as a variable). The room's rules to the server: the rule block (G 0x16F bytes: MH1's 0x16B, the room name
 * from RoomRule+0x12, the password from RoomRule+2, the message from mhRule+0x12, the chosen value of each rule at
 * +0x14B) plus four G bytes at +0x16B (the quest number, low / high byte, and two quest attributes, lobby_func_0055A020 /
 * 0055A0D0) that G's set burst sends as 6415 before 660C (src/lobby/cnet/cnet_g.c lobby_func_005BC240). Step 2 enters
 * the room (To_EnterRoom), a failure logs out (To_LogOut(4)). docs/network.md 3.12. */
#include "../gonl.h"

int CallBack_Result_Lobby_SetRoomRule();
int Check_CallBackWait();
int To_LogOut();
void *memcpy();
void *memset();
extern u8 *cw;
extern u8 mhRule[];
int To_EnterRoom();
s8 lobby_func_0055A020();
s8 lobby_func_0055A0D0();
int lobby_func_005BC240(u8 *rule, void *cb);
extern u8 RoomRule[];

s32 Lbc_SetRoomRule(void)
{
    u8 rule[0x170];
    u8 *st = cw + 0xB9D;
    u16 q;
    int i;

    switch (*st) {
    case 0:
        *st = 1;
        *(s32 *)(cw + 0xFA68) = 0xE10;  /* CallBackWaitInit */
        cw[0xFA65] = 0;
        cw[0xBA9] = 0x21;
        memset(rule, 0, sizeof rule);
        memcpy(rule, RoomRule + 0x12, 0x41);
        memcpy(rule + 0x41, RoomRule + 2, 9);
        memcpy(rule + 0x4A, mhRule + 0x12, 0x3D);
        for (i = 0; i < RoomRule[0x54]; i++)
            rule[0x14B + i] = RoomRule[0x14A8 * i + 0x9A];
        q = *(u16 *)(mhRule + 0x52);
        rule[0x16B] = (u8)q;
        rule[0x16C] = (u8)(q >> 8);
        rule[0x16D] = (u8)lobby_func_0055A020(q);
        rule[0x16E] = (u8)lobby_func_0055A0D0(q);
        lobby_func_005BC240(rule, (void *)CallBack_Result_Lobby_SetRoomRule);
        return 0;
    case 1:
        Check_CallBackWait();
        return 0;
    case 2:
        To_EnterRoom();
        return 1;
    default:
        To_LogOut(4);
        return 0;
    }
}
