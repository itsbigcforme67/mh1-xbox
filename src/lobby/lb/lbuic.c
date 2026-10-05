/* lbui, run 3: SetSceneTitle .. SetHelpLineMsg (lobby.bin 0x00591E30-0x00591EF8): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void SetSceneTitle(a, b)
int a;
int b;
{
    pSceneTitle = (int)text_lobby_msg[a];
    pSceneTitle = pSceneTitle + b * 8;
}

void SetSceneSubTitle(a, b, c)
int a;
int b;
char *c;
{
    subTitleCol = 0xFF2A0000;
    pSceneSubTitle = text_lobby_msg[a];
    pSceneSubTitle = pSceneSubTitle + b * 8;
    strcpy(*(char **)(pSceneSubTitle + 4), c);
}

void SetSceneSubTitleColor(c)
int c;
{
    subTitleCol = c;
}

void SetHelpLineMsg(a, b)
int a;
int b;
{
    *(u8 **)helpLineStr = text_lobby_msg[a];
    *(int *)(helpLineStr + 4) = 0;
    *(int *)(helpLineStr + 8) = 0;
    *(u8 **)helpLineStr = *(u8 **)helpLineStr + b * 8;
}
