/**

  本模块提供GF(256)上的加减乘除运算，是matrix模块中矩阵运算的基础。
  参见http://www.logic.at/wiki/index.php/GF(256)

  注：
  1.域上的"+"操作定义为异或(XOR)，

  2."*"操作定义为模既约多项式的乘法。(既约 = 不可再约 = 不可约 irreducible)
	具体采用的多项式为 f(2,8) = X^8 + X^4 + X^3 + X^2 + 1
	[1, 0, 0, 0, 1, 1, 1, 0, 1]，进一步了解可以访问：
	http://www.math.rwth-aachen.de/~Frank.Luebeck/ConwayPol/cp2.html (broken)
	http://www.math.rwth-aachen.de/~Frank.Luebeck/data/ConwayPol/CP2.html

  为了提高运算速度，乘除运算通过查表完成。

*/


#ifndef GF256_H
#define GF256_H

#define cTRUE 1
#define cFALSE 0

/*GF的域大小为256，用unsigned char即可*/
typedef unsigned char FIELD;

typedef unsigned long ulong;
extern FIELD* mtab;
extern FIELD* dtab;

/*GF(256)上的乘除运算用查表完成，此函数从文件fileName载入乘除表*/
int initMulDivTab(char* fileName);

/*GF(256)上的加减乘除运算*/
#define gfadd(x,y) (FIELD)((x)^(y))
#define gfsub(x,y) (FIELD)((x)^(y))
#define gfmul(x,y) (FIELD)(mtab[(x)*256+(y)])
#define gfdiv(x,y) (FIELD)(dtab[(x)*256+(y)])

#endif
