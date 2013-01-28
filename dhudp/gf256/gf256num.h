#ifndef GF256NUM_H
#define GF256NUM_H

class GF256Num;
GF256Num operator+(const GF256Num&,const GF256Num&);
GF256Num operator-(const GF256Num&,const GF256Num&);
GF256Num operator*(const GF256Num&,const GF256Num&);
GF256Num operator/(const GF256Num&,const GF256Num&);

class GF256Num
{
public:
    GF256Num(unsigned int);
    GF256Num(const GF256Num&);
    GF256Num& operator +=(const GF256Num&);
    GF256Num& operator -=(const GF256Num&);

    unsigned char byte;
    static const unsigned int mtab[256][256];
    static const unsigned int dtab[256][256];
};

#endif // GF256NUM_H
