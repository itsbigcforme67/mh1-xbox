/* tools/ps2launch/syms_mhg.ld - the addresses the launcher's in-game blob (src/ps2patch/launch_hook.c + online.c)
 * links against for Monster Hunter G (Japan) SLPM-65869: main (SLPM_658.69) and lobby.bin (loaded at 0x542600).
 * From config/mhg/symbols, each checked against G's own code (tools/mhg_fdiff.py: internet_browser's gp-relative
 * stores, create_server_table / tcp_init / lbc_login_init's bsCsvWork offsets). Addresses only, no game bytes. */
PatchExecCS = 0x237960;
memset = 0x160708;
strncpy = 0x164238;
bsCsvWork = 0x308DA0;
D_3A3B71 = 0x308DB1;        /* bsCsvWork + 0x11, the MMBB id (G's lbc_login_init copies the key from there) */
ConnectLbsId = 0x308AE0;
BsLbsInfo = 0x301590;
BsLbsCount = 0x2EF2B0;
COM_R_No_2 = 0x2EFBF4;
Net_Err_No = 0x2EF2AC;
MMBB_LOGIN = 0x2EF2D8;
Net_fade_kill = 0x2174F0;
cnLbc_LoadNetModel = 0x5C16B0;
cnLbc_LoadModelWait = 0x5C16C0;
