/* install.h - one-time install of the game's data files from the player's own ISO (install.c). */
#ifndef INSTALL_H
#define INSTALL_H
#include <stddef.h>

int install_is_iso_name(const char *p);                 /* ends in .iso / .ISO */
/* a folder that already holds the game's files (exe/data, exe/disc, the user data folder); 1 = found */
int install_has_data(const char *dir);                /* dir holds AFS_DATA.AFS and SLPM_654.95 */
int install_find_data(char *out, size_t n);
/* where an install goes: next to the exe if writable (Windows), else the user data folder */
void install_default_dir(char *out, size_t n);
/* read the ISO9660 image, check it is the Japanese MH1 disc, copy the files into destdir with a progress
 * window (gui) and stderr output, write installed.ok. 0 = ok. */
int install_from_iso(const char *iso, const char *destdir, int gui);
/* ask the player for the ISO (message box + drop window; a file dialog on Windows); 0 = got one in out */
int install_prompt(char *out, size_t n);
#endif
