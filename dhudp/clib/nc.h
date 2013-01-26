/**

  本模块是编码解码系统的核心，包括:
  1.种子文件的生成
  2.稀疏编码
  3.数据块的线性相关性检测，以及数据块的保存或丢弃
  4.最终文件的解码
  作为整个系统的一部分，本程序将以.dll形式作为python扩展模块。
  对外提供的接口包括：
	encode();
	recv();
	preNC();
	initNC();
	genSeedFile();
	decodeFile();
	clearNC();
	
*/

#ifndef NC_H
#define NC_H

#define MUL_DIV_TABLE_FN "muldiv.tab"

#include "gf256.h"
#include "matrix.h"

/*编码控制信息*/
typedef struct NCCB
{
	char fileName[64];	/*编码时读取的文件，也是接收时保存的文件*/
	ulong blockSize;	/*编码块中文件数据的大小*/
	ulong blockNum;		/*整个文件分成的块数*/
	ulong size;			/*编码块大小，包括文件数据大小和系数数据大小。size=blockSize+blockNum*/
	ulong holdNum;		/*自己拥有的块数*/
	ulong d;			/*稀疏编码参数d*/
	FIELD** cMatrix;	/*经过初等行变换的系数矩阵，用于接收数据块时的线性相关性检测*/
	//FIELD** tMatrix;
	long* nonZero;		/*初等变换系数矩阵用到该数组*/
	//long* tnonZero;
} NCCB;

/*编码开始前的准备工作，目前是初始化GF(256)的乘除表*/
int preNC();

/*初始化编码系统，并且从文件里载入系数矩阵，用于进一步接收数据块时的线性相关性检测*/
int initNC(NCCB* cb,char* fName,ulong blockSize,ulong blockNum,ulong d);

/*在cb各个参数控制下，对文件中的[from,to)块进行稀疏编码，并返回该编码块*/
FIELD* encode(NCCB* cb,ulong from,ulong to);

/* return encoded block i = from */
FIELD* encodeIdentity(NCCB* cb,ulong from);

/*接收到数据块block后，提取系数信息，判断线性相关性，并采取丢弃或保存到文件的操作*/
int recv1(NCCB* cb,FIELD* block);

/*提取系数信息，判断线性相关性。若线性无关，则cMatrix行数增加1*/
int appendM(NCCB* cb,FIELD* block);
/*not used: same as appendM but work on n blocks, check and save coeff into tMatrix */
int check(NCCB* cb,FIELD* block,ulong n);

/*把所有数据块载入内存，准备解码 read from *file into **data */
int loadData(char* file,FIELD*** data,ulong bSize,ulong bNum);
/*对文件进行解码*/
int decodeFile(char* sourceFile,char* destFile,ulong bSize,ulong bNum,ulong chunkNum);

/*从原始文件生成文件: 添加了系数信息(one byte 0x01)的若干数据块文件;以及描述文件信息的种子文件*/
int genSeedFile(char* rfName,char* sfName,ulong chunkSize, ulong blockSize);

/*停止编码解码工作，释放资源(比如内存中的系数矩阵)*/
void clearNC(NCCB* cb);

#endif
