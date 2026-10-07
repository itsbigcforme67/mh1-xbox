/*
 * xbox_compat.h - forced into the Xbox build's host-side C (tools/build_xbox.py;
 * not the game C). The PC code joins paths with '/'; the Xbox kernel wants
 * '\'. fopen is renamed here (before <stdio.h> declares it) to a wrapper in
 * xbox_libc.c that converts the separators and then calls pdclib's fopen.
 */
#ifndef XBOX_COMPAT_H
#define XBOX_COMPAT_H
#define fopen xbox_fopen
#endif
