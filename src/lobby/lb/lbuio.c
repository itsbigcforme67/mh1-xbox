/* lbui, run 15: Get_PlazaName .. Lbc_release (lobby.bin 0x0059DA40-0x0059DB3C): the matching functions of lbui_nm.c. */
#pragma readonly_strings on
#include "lbui_proto.h"

void Get_PlazaName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), PlazaInfo[ClassInfo.plaza - 1].name);
}

void Get_LobbyName(dst)
char *dst;
{
    sprintf(dst, lit_193_0065DBE8, Get_ServerName(), LobbyInfo[ClassInfo.lobby - 1].name);
}

void Lbs_load(void) {
    load_pit();
    load_texlist(*(int *)0x3876A8, 0x14D, 0);
}

void Lbc_release(void) {
    release_texture(0x118, 0x15);
}
