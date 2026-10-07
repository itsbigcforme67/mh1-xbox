#include <stdio.h>
#include <string.h>
int sceMcInit(void); int sceMcSync(int,int*,int*); int sceMcGetInfo(int,int,int*,int*,int*);
int sceMcGetDir(int,int,const char*,unsigned,int,void*); int sceMcOpen(int,int,const char*,int);
int sceMcClose(int); int sceMcRead(int,void*,int); int sceMcWrite(int,const void*,int); int sceMcMkdir(int,int,const char*); int sceMcDelete(int,int,const char*);
static int r(void){int c,x;sceMcSync(0,&c,&x);return x;}
int main(void){char tb[0x40*8];int t,f,fm;
 sceMcInit(); sceMcGetInfo(0,0,&t,&f,&fm); printf("info %d type %d free %d\n",r(),t,f);
 sceMcGetDir(0,0,"/*",0,8,tb); printf("root listing %d\n",r());
 sceMcMkdir(0,0,"BISLPM-65495MH"); printf("mkdir %d\n",r());
 sceMcOpen(0,0,"BISLPM-65495MH/BISLPM-65495MH",0x203); int fd=r(); printf("open %d\n",fd);
 sceMcWrite(fd,"hello",5); printf("write %d\n",r()); sceMcClose(fd); r();
 sceMcGetDir(0,0,"/*",0,8,tb); printf("root listing %d first '%s'\n",r(),tb+0x20);
 sceMcGetDir(0,0,"BISLPM-65495MH/*",0,8,tb); printf("dir listing %d first '%s'\n",r(),tb+0x20);
 sceMcGetDir(0,0,"BISLPM-65495MH/BISLPM-65495MH",0,8,tb); int n=r(); printf("single %d size %u\n",n,*(unsigned*)(tb+0x10));
 sceMcOpen(0,0,"BISLPM-65495MH/BISLPM-65495MH",1); fd=r(); char b[8]={0}; sceMcRead(fd,b,5); printf("read %d '%s'\n",r(),b); sceMcClose(fd);r();
 sceMcDelete(0,0,"BISLPM-65495MH/BISLPM-65495MH"); printf("delete %d\n",r()); return 0;}
