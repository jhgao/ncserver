#include "gf256num.h"

GF256Num::GF256Num(unsigned int i):
    byte((unsigned char)i)
{
}

GF256Num::GF256Num(const GF256Num &rhs):
    byte((unsigned char)rhs.byte)
{
}

GF256Num operator +(const GF256Num& lhs,const GF256Num& rhs)
{
    GF256Num ret(lhs);
    ret += rhs;
    return ret;
}

GF256Num operator -(const GF256Num& lhs,const GF256Num& rhs)
{
    GF256Num ret(lhs);
    ret -= rhs;
    return ret;
}

GF256Num operator *(const GF256Num& lhs,const GF256Num& rhs)
{
    GF256Num ret(GF256Num::mtab[(unsigned int)lhs.byte][(unsigned int)rhs.byte]);
    return ret;
}

GF256Num operator /(const GF256Num& lhs,const GF256Num& rhs)
{
    GF256Num ret(GF256Num::dtab[(unsigned int)lhs.byte][(unsigned int)rhs.byte]);
    return ret;
}

GF256Num &GF256Num::operator +=(const GF256Num &rhs)
{
    this->byte = this->byte ^ rhs.byte;
    return *this;
}

GF256Num &GF256Num::operator -=(const GF256Num &rhs)
{
    this->byte = this->byte ^ rhs.byte;
    return *this;
}
