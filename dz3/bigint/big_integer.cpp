#include "big_integer.h"
#include <algorithm>
#include <climits>
#include <cassert>

static uint32_t* dataAlloc(size_t s)
{
    size_t* data = (size_t*)new uint8_t[sizeof(size_t) + s * sizeof(uint32_t)];
    *data = 0;
    return (uint32_t*)(data + 1);
}

static uint32_t* dataAlloc(size_t s, std::nothrow_t)
{
    size_t* data = (size_t*)new(std::nothrow) uint8_t[sizeof(size_t) + s * sizeof(uint32_t)];
    if (data == nullptr)
        return nullptr;
    *data = 0;
    return (uint32_t*)(data + 1);
}

static void dataRefcntInc(uint32_t * x)
{
    size_t * r = (size_t*)x - 1;
    ++*r;
}

static void dataRefcntDec(uint32_t * x)
{
    size_t * r = (size_t*)x - 1;
    assert(*r != 0);
    --*r;
    if (*r == 0)
        delete [] (uint8_t*)r;
}

big_integer::big_integer() :
    big_integer(0)
{
}

big_integer::big_integer(big_integer const & b) :
    size(b.size), dataUnion(b.dataUnion)
{
    if (size > SMALL_SIZE)
        dataRefcntInc(dataUnion.ptr);
}

big_integer::big_integer(std::string const & s) :
    big_integer(0)
{
    if (s == "")
        return;
    bool neg = s[0] == '-';
    for (size_t i = neg; i < s.size(); ++i)
    {
        *this *= 10;
        *this += s[i] - '0';
    }
    if (neg)
        *this = -*this;
}

big_integer::big_integer(int b) :
    size(1)
{
    dataUnion.arr[0] = b;
}

big_integer::big_integer(uint32_t b)
{
    dataUnion.arr[0] = b;
    if (b < (uint32_t)1 << 31)
        size = 1;
    else
    {
        size = 2;
        dataUnion.arr[1] = 0;
    }
}

big_integer::~big_integer()
{
    if (size > SMALL_SIZE)
        dataRefcntDec(dataUnion.ptr);
}

void big_integer::swap(big_integer & b)
{
    std::swap(size, b.size);
    std::swap(dataUnion, b.dataUnion);
}

big_integer & big_integer::operator = (big_integer const & b)
{
    if (size > SMALL_SIZE)
        dataRefcntDec(dataUnion.ptr);
    dataUnion = b.dataUnion;
    size = b.size;
    if (size > SMALL_SIZE)
        dataRefcntInc(dataUnion.ptr);
    return *this;
}

big_integer & big_integer::operator += (big_integer const & b)
{
    return *this = *this + b;
}
big_integer & big_integer::operator -= (big_integer const & b)
{
    return *this = *this - b;
}
big_integer & big_integer::operator *= (big_integer const & b)
{
    return *this = *this * b;
}

big_integer & big_integer::operator /= (big_integer const & b)
{
    return *this = *this / b;
}

big_integer & big_integer::operator %= (big_integer const & b)
{
    return *this = *this % b;
}

big_integer & big_integer::operator &= (big_integer const & b)
{
    return *this = *this & b;
}

big_integer & big_integer::operator |= (big_integer const & b)
{
    return *this = *this | b;
}

big_integer & big_integer::operator ^= (big_integer const & b)
{
    return *this = *this ^ b;
}

big_integer & big_integer::operator <<= (int b)
{
    return *this = *this << b;
}
big_integer & big_integer::operator >>= (int b)
{
    return *this = *this >> b;
}

big_integer big_integer::operator + () const
{
    return *this;
}
big_integer big_integer::operator - () const
{
    big_integer b = ~*this;
    return ++b;
}
big_integer big_integer::operator ~ () const
{
    big_integer r;
    r.resize(size);
    uint32_t * rdata = r.getData();
    uint32_t const * adata = getData();
    for (size_t i = 0; i < size; ++i)
        rdata[i] = ~adata[i];
    return r;
}

big_integer & big_integer::operator ++ ()
{
    return *this += 1;
}
big_integer & big_integer::operator -- ()
{
    return *this -= 1;
}
big_integer big_integer::operator ++ (int)
{
    big_integer a = *this;
    ++*this;
    return a;
}
big_integer big_integer::operator -- (int)
{
    big_integer a = *this;
    --*this;
    return a;
}

big_integer operator + (big_integer const & a, big_integer const & b)
{
    big_integer r;
    r.resize(std::max(a.size, b.size));
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t carry = 0;
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
    {
        uint64_t sum = (uint64_t)(i >= a.size ? as : adata[i]) +
                                 (i >= b.size ? bs : bdata[i]) + carry;
        rdata[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
    uint32_t rs1 = (rdata[r.size - 1] >> 31 ? UINT32_MAX : 0);
    if (as + bs + carry != rs1)
    {
        r.resize(r.size + 1);
        rdata[r.size - 1] = as + bs + carry;
    }
    r.shrink();
    return r;
}
big_integer operator - (big_integer const & a, big_integer const & b)
{
    big_integer r;
    r.resize(std::max(a.size, b.size));
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t carry = 0;
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
    {
        uint64_t diff = (uint64_t)(i >= a.size ? as : adata[i]) -
                                  (i >= b.size ? bs : bdata[i]) - carry;
        rdata[i] = (uint32_t)diff;
        carry = diff >> 63;
    }
    uint32_t rs1 = (rdata[r.size - 1] >> 31 ? UINT32_MAX : 0);
    if (as - bs - carry != rs1)
    {
        r.resize(r.size + 1);
        rdata[r.size - 1] = as - bs - carry;
    }
    r.shrink();
    return r;
}
big_integer operator * (big_integer const & a, big_integer const & b)
{
    if (a < 0)
    {
        if (b < 0)
            return (-a) * (-b);
        else
            return -((-a) * b);
    } else if (b < 0)
        return -(a * (-b));
    big_integer r;
    r.resize(a.size + b.size);
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    for (size_t i = 0; i < a.size; ++i)
    {
        uint32_t carry = 0;
        for (size_t j = 0; j < b.size; ++j)
        {
            uint64_t res = (uint64_t)adata[i] * bdata[j] + carry + rdata[i + j];
            rdata[i + j] = (uint32_t)res;
            carry = res >> 32;
        }
        for (size_t j = b.size; i + j < r.size && carry; ++j)
        {
            uint64_t res = (uint64_t)carry + rdata[i + j];
            rdata[i + j] = (uint32_t)res;
            carry = res >> 32;
        }
    }
    r.shrink();
    return r;
}

static void subShifted(uint32_t * adata, size_t asize, uint32_t const * bdata, size_t bsize, size_t shift)
{
    uint32_t carry = 0;
    for (size_t i = 0; i < bsize && i + shift < asize; ++i)
    {
        uint64_t diff = (uint64_t)adata[i + shift] - bdata[i] - carry;
        adata[i + shift] = (uint32_t)diff;
        carry = diff >> 63;
    }
}

static bool lessShifted(uint32_t const * adata, size_t asize, uint32_t const * bdata, size_t bsize, size_t shift)
{
    for (size_t i = bsize - 1; i != (size_t)-1; --i)
        if ((i + shift >= asize ? 0 : adata[i + shift]) != bdata[i])
            return adata[i + shift] < bdata[i];
    return false;
}

std::pair < big_integer, big_integer > big_integer::divModImpl(big_integer const & ina, big_integer const & b)
{
    big_integer a = ina;
    a.duplicate();
    uint32_t * adata = a.getData();
    uint32_t const * bdata = b.getData();
    size_t blen = 1;
    for (size_t i = b.size - 1; i > 0; --i)
        if (bdata[i] != 0)
        {
            blen = i + 1;
            break;
        }

    big_integer r;
    if (blen > a.size)
        return {r, a};
    r.resize(a.size - blen + 1);
    uint32_t * rdata = r.getData();

    for (size_t i = a.size - blen; i != (size_t)-1; --i)
    {
        uint32_t p1 = (i + blen >= a.size ? 0 : adata[i + blen]);
        int ss = (p1 == 0 ? 32 : __builtin_clz(p1));
        size_t i2 = i + blen - 2;
        uint64_t q1 = ((ss == 32 ? 0 : (uint64_t)p1 << (32 + ss))) | ((uint64_t)adata[i + blen - 1] << ss);
        if (i2 < a.size && ss != 0)
            q1 |= adata[i2] >> (32 - ss);
        uint64_t q2 = ((uint64_t)bdata[blen - 1] << ss);
        if (blen > 1 && ss != 0)
            q2 |= bdata[blen - 2] >> (32 - ss);
        ++q2;
    
        uint32_t w = q1 / q2;
        big_integer bw = b * w;
        subShifted(adata, a.size, bw.getData(), bw.size, i);
        while (!lessShifted(adata, a.size, bdata, b.size, i))
        {
            ++w;
            subShifted(adata, a.size, bdata, b.size, i);
        }
        rdata[i] = w;
    }
    r.shrink();
    a.shrink();
    return {r, a};
}

big_integer operator / (big_integer const & a, big_integer const & b)
{
    if (a < 0)
    {
        if (b < 0)
            return (-a) / (-b);
        else
            return -((-a) / b);
    } else if (b < 0)
        return -(a / (-b));
    return big_integer::divModImpl(a, b).first;
}

big_integer operator % (big_integer const & a, big_integer const & b)
{
    if (a < 0)
    {
        if (b < 0)
            return -((-a) % (-b));
        else
            return -((-a) % b);
    } else if (b < 0)
        return a % (-b);
    return big_integer::divModImpl(a, b).second;
}
big_integer operator & (big_integer const & a, big_integer const & b)
{
    big_integer r;
    r.resize(std::max(a.size, b.size));
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
        rdata[i] = (i >= a.size ? as : adata[i]) & (i >= b.size ? bs : bdata[i]);
    r.shrink();
    return r;
}
big_integer operator | (big_integer const & a, big_integer const & b)
{
    big_integer r;
    r.resize(std::max(a.size, b.size));
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
        rdata[i] = (i >= a.size ? as : adata[i]) | (i >= b.size ? bs : bdata[i]);
    r.shrink();
    return r;
}
big_integer operator ^ (big_integer const & a, big_integer const & b)
{
    big_integer r;
    r.resize(std::max(a.size, b.size));
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
        rdata[i] = (i >= a.size ? as : adata[i]) ^ (i >= b.size ? bs : bdata[i]);
    r.shrink();
    return r;
}

big_integer operator << (big_integer const & a, int b)
{
    if (b < 0)
        return a >> -b;
    big_integer r;
    size_t bc = b >> 5, br = b & 31;
    r.resize(a.size + bc + 1);
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < r.size; ++i)
        rdata[i] = (i >= bc ? (i - bc < a.size ? adata[i - bc] : as) : 0);
    if (br != 0)
        for (size_t i = r.size - 1; i != (size_t)-1; --i)
        {
            rdata[i] <<= br;
            if (i != 0)
                rdata[i] |= (rdata[i - 1] >> (32 - br));
        }
    r.shrink();
    return r;
}

big_integer operator >> (big_integer const & a, int b)
{
    if (b < 0)
        return a << -b;
    big_integer r;
    r.resize(a.size);
    uint32_t * rdata = r.getData();
    uint32_t const * adata = a.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    size_t bc = b >> 5, br = b & 31;
    for (size_t i = 0; i < r.size; ++i)
        rdata[i] = (i + bc < a.size ? adata[i + bc] : as);
    if (br != 0)
        for (size_t i = 0; i < r.size; ++i)
        {
            rdata[i] >>= br;
            rdata[i] |= (i + 1 == a.size ? as : rdata[i + 1]) << (32 - br);
        }
    r.shrink();
    return r;
}

bool operator == (big_integer const & a, big_integer const & b)
{
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    for (size_t i = 0; i < std::min(a.size, b.size); ++i)
        if (adata[i] != bdata[i])
            return false;
    for (size_t i = a.size; i < b.size; ++i)
        if (as != bdata[i])
            return false;
    for (size_t i = b.size; i < a.size; ++i)
        if (adata[i] != bs)
            return false;
    return true;
}
bool operator != (big_integer const & a, big_integer const & b)
{
    return !(a == b);
}
bool operator < (big_integer const & a, big_integer const & b)
{
    uint32_t const * adata = a.getData();
    uint32_t const * bdata = b.getData();
    uint32_t as = (adata[a.size - 1] >> 31 ? UINT32_MAX : 0);
    uint32_t bs = (bdata[b.size - 1] >> 31 ? UINT32_MAX : 0);
    if (as != bs)
        return as > bs;
    for (size_t i = b.size - 1; i >= a.size; --i)
        if (as != bdata[i])
            return as < bdata[i];
    for (size_t i = a.size - 1; i >= b.size; --i)
        if (adata[i] != bs)
            return adata[i] < bs;
    for (size_t i = std::min(a.size, b.size) - 1; i != (size_t)-1; --i)
        if (adata[i] != bdata[i])
            return adata[i] < bdata[i];
    return false;
}
bool operator > (big_integer const & a, big_integer const & b)
{
    return b < a;
}
bool operator <= (big_integer const & a, big_integer const & b)
{
    return !(b < a);
}
bool operator >= (big_integer const & a, big_integer const & b)
{
    return !(a < b);
}

std::string to_string(big_integer a)
{
    if (a == 0)
        return "0";
    std::string s;
    bool neg = false;
    if (a < 0)
    {
        neg = true;
        a = -a;
    }
    while (a != 0)
    {
        uint64_t rem = 0;
        uint32_t const * adata = a.getData();
        for (size_t i = a.size - 1; i != (size_t)-1; --i)
            rem = ((rem << 32) + adata[i]) % 10;
        s += '0' + rem;
        a /= 10;
    }
    std::reverse(s.begin(), s.end());
    if (neg)
        s = "-" + s;
    return s;
}
std::istream & operator >> (std::istream & in, big_integer & a)
{
    std::string s;
    in >> s;
    a = big_integer(s);
    return in;
}
std::ostream & operator << (std::ostream & out, big_integer const & a)
{
    return out << to_string(a);
}

uint32_t * big_integer::getData()
{
    return size > SMALL_SIZE ? dataUnion.ptr : dataUnion.arr;
}

uint32_t const * big_integer::getData() const
{
    return size > SMALL_SIZE ? dataUnion.ptr : dataUnion.arr;
}

void big_integer::resize(size_t nsize)
{
    if (size == nsize)
        return;
    uint32_t * data = getData();
    uint32_t * ndata;
    if (nsize <= SMALL_SIZE)
        ndata = dataUnion.arr;
    else if (nsize > size)
        ndata = dataAlloc(nsize);
    else
    {
        ndata = dataAlloc(nsize, std::nothrow);
        if (ndata == nullptr)
            return;
    }

    if (data != ndata)
        std::copy(data, data + std::min(size, nsize), ndata);
    if (nsize > size)
        std::fill(ndata + size, ndata + nsize, (data[size - 1] >> 31 ? UINT32_MAX : 0));
    if (size > SMALL_SIZE)
        dataRefcntDec(data);
    size = nsize;
    if (size > SMALL_SIZE)
    {
        dataRefcntInc(ndata);
        dataUnion.ptr = ndata;
    }
}

void big_integer::duplicate()
{
    if (size <= SMALL_SIZE)
        return;
    uint32_t * data = dataUnion.ptr;
    uint32_t * ndata = dataAlloc(size);
    std::copy(data, data + size, ndata);
    dataRefcntDec(data);
    dataRefcntInc(ndata);
    dataUnion.ptr = ndata;
}

void big_integer::shrink()
{
    uint32_t * data = getData();
    uint32_t as = data[size - 1] >> 31 ? UINT32_MAX : 0;
    for (size_t i = size - 1; true; --i)
        if (data[i] != as || i == 0)
        {
            size_t nsize = ((data[i] >> 31) == (as >> 31) ? i + 1 : i + 2);
            resize(nsize);
            return;
        }
}
