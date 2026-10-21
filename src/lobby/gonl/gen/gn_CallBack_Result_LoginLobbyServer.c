/* G online (PC): CallBack_Result_LoginLobbyServer, written from Monster Hunter G's code at lobby 0x57C4B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
s32 lobby_func_0058BAA0();
extern u8 CnetWork;
extern u8 cw;
s32 cnLBS_Get_LoginWarningMessage();  /* extern */
s32 cnLBS_Get_ServerMessage();        /* extern */
s32 cnLBS_Send_LoginUserAccount(); /* extern */
s32 cnetGet_Login_DecideUserHandle();     /* extern */
s32 cnetGet_Login_DecideUserID();         /* extern */
s32 fade_set();                      /* extern */
s32 memcpy();         /* extern */
s32 memset();        /* extern */
extern u8 my_user_handle;
extern u8 my_user_id;
extern u8 my_user_mini_data;

void CallBack_Result_LoginLobbyServer(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    s64 temp_a0;
    s64 temp_v1;
    u8 *temp_v0;

    sp18 = arg0;
    if ((s8) sp18 != -1) {
        temp_v1 = ((s8) ((s64) (((s8 *)&sp18)[1])));
        switch (temp_v1) {                          /* switch 1 */
        case 4:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 2;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            cnLBS_Get_LoginWarningMessage((*(void **)&cw) + 0xFA8A, (((s8 *)&sp18)[1]));
            return;
        case 1:                                     /* switch 1 */
            switch ((*(u8 *)((u8 *)&CnetWork + 0x5))) {                   /* switch 2; irregular */
            case 0:                                 /* switch 2 */
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 4;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                fade_set(1, (*(u8 *)((u8 *)&CnetWork + 0x5)));
                return;
            case 2:                                 /* switch 2 */
            case 1:                                 /* switch 2 */
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 4;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 6;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
                lobby_func_0058BAA0(&my_user_mini_data);
                temp_v0 = (*(void **)&cw);
                cnLBS_Send_LoginUserAccount(temp_v0 + 0x1C0, temp_v0 + 0x1C8, &my_user_mini_data);
                return;
            case 3:                                 /* switch 2 */
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 4;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 6;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
                lobby_func_0058BAA0(&my_user_mini_data);
                cnLBS_Send_LoginUserAccount(&my_user_id, &my_user_handle, &my_user_mini_data);
                return;
            }
            break;
        case 2:                                     /* switch 1 */
            memset((*(void **)&cw) + 0x1C0, 0, 8);
            cnetGet_Login_DecideUserID((*(void **)&cw) + 0x1C0);
            memset(&my_user_id, 0, 8);
            memcpy(&my_user_id, (*(void **)&cw) + 0x1C0, 8);
            memset((*(void **)&cw) + 0x1C8, 0, 0x10);
            cnetGet_Login_DecideUserHandle((*(void **)&cw) + 0x1C8);
            memcpy(&my_user_handle, (*(void **)&cw) + 0x1C8, 0x10);
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 6;
            return;
        case 5:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 1;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            return;
        case 3:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 3;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            return;
        case 0:                                     /* switch 1 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 7;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            return;
        }
    } else {
        temp_a0 = ((s8) ((s64) (((s8 *)&sp18)[1])));
        switch (temp_a0) {                          /* switch 3; irregular */
        case 7:                                     /* switch 3 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 6;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 2)) = 1;
            fade_set(1, (u8) (((s8 *)&sp18)[1]));
            cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
            return;
        case 8:                                     /* switch 3 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 6;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 2)) = 2;
            cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, (((s8 *)&sp18)[1]));
            return;
        case 9:                                     /* switch 3 */
            To_LogOut(4);
        default:                                    /* switch 1 */
            return;
        }
    }
}

