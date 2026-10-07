#pragma once
#include <dirent.h>
#include <fnmatch.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
typedef unsigned long DWORD; typedef int BOOL; typedef void *HANDLE;
typedef struct { DWORD l, h; } FILETIME;
typedef struct { int wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds; } SYSTEMTIME;
typedef struct { DWORD dwFileAttributes; FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime; DWORD nFileSizeHigh, nFileSizeLow; char cFileName[260]; } WIN32_FIND_DATAA;
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#define ROOTDIR "/tmp/mc_xbox_test_disk"
static void fix(char *o, const char *p){ snprintf(o,600,"%s/%s",ROOTDIR,p+3); for(char*s=o;*s;s++) if(*s=='\\')*s='/'; }
static DWORD GetFileAttributesA(const char*p){char q[600];fix(q,p);struct stat st; if(stat(q,&st))return INVALID_FILE_ATTRIBUTES; return S_ISDIR(st.st_mode)?0x10:0x80;}
static BOOL CreateDirectoryA(const char*p,void*x){char q[600];fix(q,p);return mkdir(q,0755)==0;}
static BOOL DeleteFileA(const char*p){char q[600];fix(q,p);return remove(q)==0;}
static BOOL RemoveDirectoryA(const char*p){char q[600];fix(q,p);return rmdir(q)==0;}
typedef struct { DIR*d; char pat[260]; char dir[600]; } FH;
static void fill(WIN32_FIND_DATAA*f,const char*dir,const char*n){char q[900];snprintf(q,sizeof q,"%s/%s",dir,n);struct stat st;stat(q,&st);memset(f,0,sizeof*f);f->dwFileAttributes=S_ISDIR(st.st_mode)?0x10:0x80;f->nFileSizeLow=st.st_size;snprintf(f->cFileName,260,"%s",n);}
static int nxt(FH*h,WIN32_FIND_DATAA*f){struct dirent*e;while((e=readdir(h->d))){if(fnmatch(h->pat,e->d_name,0)==0){fill(f,h->dir,e->d_name);return 1;}}return 0;}
static HANDLE FindFirstFileA(const char*p,WIN32_FIND_DATAA*f){char q[600];fix(q,p);FH*h=malloc(sizeof*h);char*s=strrchr(q,'/');*s=0;snprintf(h->dir,600,"%s",q);snprintf(h->pat,260,"%s",s+1);h->d=opendir(h->dir);if(!h->d){free(h);return INVALID_HANDLE_VALUE;}if(!nxt(h,f)){closedir(h->d);free(h);return INVALID_HANDLE_VALUE;}return h;}
static BOOL FindNextFileA(HANDLE h,WIN32_FIND_DATAA*f){return nxt((FH*)h,f);}
static BOOL FindClose(HANDLE h){closedir(((FH*)h)->d);free(h);return 1;}
static BOOL FileTimeToSystemTime(const FILETIME*a,SYSTEMTIME*b){memset(b,0,sizeof*b);b->wYear=2026;b->wMonth=10;b->wDay=9;return 1;}
static void GetLocalTime(SYSTEMTIME*b){FileTimeToSystemTime(0,b);}


static FILE*(*real_fopen)(const char*,const char*)=fopen;
static FILE*xfopen(const char*p,const char*m){char q[600];fix(q,p);return real_fopen(q,m);}
#define fopen xfopen
