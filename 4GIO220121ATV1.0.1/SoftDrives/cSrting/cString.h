#ifndef CSTRING_H_
#define CSTRING_H_
#include "stm32f1xx_hal.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
void cStringFree(void);
void cStringRestore(void);
char *StrBetwString(char *Str,char *StrBegin,char *StrEnd);
int split(char *src,const char *separator,char **dest,int DestLen);
void HexToStr(char *pbSrc,char *pbDest,int nlen);
void StrToHex(char *pbSrc, char *pbDest, int nLen);
#endif

