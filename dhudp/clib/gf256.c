#include "stdio.h"
#include "stdlib.h"
#include "gf256.h"

FIELD* mtab=NULL;
FIELD* dtab=NULL;
int initMulDivTab(char* fileName)
{
	FILE* f;
    if((f=fopen(fileName,"rb"))==NULL)
        return cFALSE;

	mtab=(FIELD*)malloc(sizeof(FIELD)*65536);
	dtab=(FIELD*)malloc(sizeof(FIELD)*65536);
	fread(mtab,65536L,1,f);		//read from f 1 element of size 65536L bytes (the first 64KB)to mtab	// "muldiv.tab" size=128KB
	fread(dtab,65536L,1,f);		//read from f 1 element of size 65536L bytes (the last 64KB)to mtab		// "muldiv.tab" size=128KB
	fclose(f);
	return cTRUE;
}

