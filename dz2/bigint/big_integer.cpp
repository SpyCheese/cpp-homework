#include "big_integer.h"
#include <algorithm>
#include <climits>
#include <cassert>

big_integer::big_integer() :
	big_integer(0)
{
}

big_integer::big_integer(big_integer const & b) :
	size(b.size), data(new uint32_t[b.size])
{
	std::copy(b.data, b.data + size, data);
}

big_integer::big_integer(big_integer && b) :
	size(b.size), data(b.data)
{
	b.size = 0;
	b.data = nullptr;
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
	size(1), data(new uint32_t[1])
{
	data[0] = b;
}

big_integer::big_integer(uint32_t b)
{
	if (b < (uint32_t)1 << 31)
	{
		size = 1;
		data = new uint32_t[1];
		data[0] = b;
	} else {
		size = 2;
		data = new uint32_t[2];
		data[0] = b;
		data[1] = 0;
	}
}

big_integer::~big_integer()
{
	delete [] data;
}

void big_integer::swap(big_integer & b)
{
	std::swap(size, b.size);
	std::swap(data, b.data);
}

big_integer & big_integer::operator = (big_integer const & b)
{
	uint32_t * ndata = new uint32_t[b.size];
	std::copy(b.data, b.data + b.size, ndata);
	delete [] data;
	data = ndata;
	size = b.size;
	return *this;
}

big_integer & big_integer::operator = (big_integer && b)
{
	swap(b);
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
	if (size < b.size)
		resize(b.size);
	for (size_t i = 0; i < b.size; ++i)
		data[i] &= b.data[i];
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = b.size; i < size; ++i)
		data[i] &= bs;
	shrink();
	return *this;
}

big_integer & big_integer::operator |= (big_integer const & b)
{
	if (size < b.size)
		resize(b.size);
	for (size_t i = 0; i < b.size; ++i)
		data[i] |= b.data[i];
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = b.size; i < size; ++i)
		data[i] |= bs;
	shrink();
	return *this;
}

big_integer & big_integer::operator ^= (big_integer const & b)
{
	if (size < b.size)
		resize(b.size);
	for (size_t i = 0; i < b.size; ++i)
		data[i] ^= b.data[i];
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = b.size; i < size; ++i)
		data[i] ^= bs;
	shrink();
	return *this;
}

big_integer & big_integer::operator <<= (int b)
{
	if (b < 0)
		return *this >>= -b;
	size_t bc = b >> 5, br = b & 31;
	resize(size + bc + 1);
	for (size_t i = size - 1; i != (size_t)-1; --i)
		data[i] = (i >= bc ? data[i - bc] : 0);
	if (br != 0)
		for (size_t i = size - 1; i != (size_t)-1; --i)
		{
			data[i] <<= br;
			if (i != 0)
				data[i] |= (data[i - 1] >> (32 - br));
		}
	shrink();
	return *this;
}
big_integer & big_integer::operator >>= (int b)
{
	if (b < 0)
		return *this <<= -b;
	uint32_t as = (data[size - 1] >> 31 ? UINT32_MAX : 0);
	size_t bc = b >> 5, br = b & 31;
	for (size_t i = 0; i < size; ++i)
		data[i] = (i + bc < size ? data[i + bc] : as);
	if (br != 0)
		for (size_t i = 0; i < size; ++i)
		{
			data[i] >>= br;
			data[i] |= (i + 1 == size ? as : data[i + 1]) << (32 - br);
		}
	shrink();
	return *this;
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
	for (size_t i = 0; i < size; ++i)
		r.data[i] = ~data[i];
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

big_integer operator + (big_integer a, big_integer const & b)
{
	if (a.size < b.size)
		a.resize(b.size);
	uint32_t carry = 0;
	uint32_t as = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = 0; i < b.size; ++i)
	{
		uint64_t sum = (uint64_t)a.data[i] + b.data[i] + carry;
		a.data[i] = (uint32_t)sum;
		carry = sum >> 32;
	}
	for (size_t i = b.size; i < a.size && bs + carry; ++i)
	{
		uint64_t sum = (uint64_t)a.data[i] + bs + carry;
		a.data[i] = (uint32_t)sum;
		carry = sum >> 32;
	}
	uint32_t as1 = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	if (as + bs + carry != as1)
	{
		a.resize(a.size + 1);
		a.data[a.size - 1] = as + bs + carry;
	}
	a.shrink();
	return a;
}
big_integer operator - (big_integer a, big_integer const & b)
{
	if (a.size < b.size)
		a.resize(b.size);
	uint32_t carry = 0;
	uint32_t as = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = 0; i < b.size; ++i)
	{
		uint64_t diff = (uint64_t)a.data[i] - b.data[i] - carry;
		a.data[i] = (uint32_t)diff;
		carry = diff >> 63;
	}
	for (size_t i = b.size; i < a.size && bs + carry; ++i)
	{
		uint64_t diff = (uint64_t)a.data[i] - bs - carry;
		a.data[i] = (uint32_t)diff;
		carry = diff >> 63;
	}
	uint32_t as1 = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	if (as - bs - carry != as1)
	{
		a.resize(a.size + 1);
		a.data[a.size - 1] = as - bs - carry;
	}
	a.shrink();
	return a;
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
	for (size_t i = 0; i < a.size; ++i)
	{
		uint32_t carry = 0;
		for (size_t j = 0; j < b.size; ++j)
		{
			uint64_t res = (uint64_t)a.data[i] * b.data[j] + carry + r.data[i + j];
			r.data[i + j] = (uint32_t)res;
			carry = res >> 32;
		}
		for (size_t j = b.size; i + j < r.size && carry; ++j)
		{
			uint64_t res = (uint64_t)carry + r.data[i + j];
			r.data[i + j] = (uint32_t)res;
			carry = res >> 32;
		}
	}
	r.shrink();
	return r;
}

void big_integer::subShifted(big_integer const & b, size_t shift)
{
	uint32_t carry = 0;
	for (size_t i = 0; i < b.size && i + shift < size; ++i)
	{
		uint64_t diff = (uint64_t)data[i + shift] - b.data[i] - carry;
		data[i + shift] = (uint32_t)diff;
		carry = diff >> 63;
	}
}

bool big_integer::lessShifted(big_integer const & b, size_t shift)
{
	for (size_t i = b.size - 1; i != (size_t)-1; --i)
		if ((i + shift >= size ? 0 : data[i + shift]) != b.data[i])
			return data[i + shift] < b.data[i];
	return false;
}

void big_integer::divModImpl(big_integer const & b, big_integer & r)
{
	if (b == *this)
	{
		*this = 0;
		r = 1;
		return;
	}
	size_t blen = 1;
	for (size_t i = b.size - 1; i > 0; --i)
		if (b.data[i] != 0)
		{
			blen = i + 1;
			break;
		}

	r = 0;
	if (blen > size)
		return;
	r.resize(size - blen + 1);
	for (size_t i = size - blen; i != (size_t)-1; --i)
	{
		uint32_t p1 = (i + blen >= size ? 0 : data[i + blen]);
		int ss = (p1 == 0 ? 32 : __builtin_clz(p1));
		size_t i2 = i + blen - 2;
		uint64_t q1 = ((uint64_t)p1 << (32 + ss)) | ((uint64_t)data[i + blen - 1] << ss);
		if (i2 < size && ss != 0)
			q1 |= data[i2] >> (32 - ss);
		uint64_t q2 = ((uint64_t)b.data[blen - 1] << ss);
		if (blen > 1 && ss != 0)
			q2 |= b.data[blen - 2] >> (32 - ss);
		++q2;
	
		uint32_t w = q1 / q2;
		subShifted(b * w, i);
		while (!lessShifted(b, i))
		{
			++w;
			subShifted(b, i);
		}
		r.data[i] = w;
	}
	r.shrink();
	shrink();
}

big_integer operator / (big_integer a, big_integer const & b)
{
	if (a < 0)
	{
		if (b < 0)
			return (-a) / (-b);
		else
			return -((-a) / b);
	} else if (b < 0)
		return -(a / (-b));
	big_integer r;
	a.divModImpl(b, r);
	return r;
}

big_integer operator % (big_integer a, big_integer const & b)
{
	if (a < 0)
	{
		if (b < 0)
			return -((-a) % (-b));
		else
			return -((-a) % b);
	} else if (b < 0)
		return a % (-b);
	big_integer r;
	a.divModImpl(b, r);
	return a;
}
big_integer operator & (big_integer a, big_integer const & b)
{
	return a &= b;
}
big_integer operator | (big_integer a, big_integer const & b)
{
	return a |= b;
}
big_integer operator ^ (big_integer a, big_integer const & b)
{
	return a ^= b;
}

big_integer operator << (big_integer a, int b)
{
	return a <<= b;
}

big_integer operator >> (big_integer a, int b)
{
	return a >>= b;
}

bool operator == (big_integer const & a, big_integer const & b)
{
	uint32_t as = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	for (size_t i = 0; i < std::min(a.size, b.size); ++i)
		if (a.data[i] != b.data[i])
			return false;
	for (size_t i = a.size; i < b.size; ++i)
		if (as != b.data[i])
			return false;
	for (size_t i = b.size; i < a.size; ++i)
		if (a.data[i] != bs)
			return false;
	return true;
}
bool operator != (big_integer const & a, big_integer const & b)
{
	return !(a == b);
}
bool operator < (big_integer const & a, big_integer const & b)
{
	uint32_t as = (a.data[a.size - 1] >> 31 ? UINT32_MAX : 0);
	uint32_t bs = (b.data[b.size - 1] >> 31 ? UINT32_MAX : 0);
	if (as != bs)
		return as > bs;
	for (size_t i = b.size - 1; i >= a.size; --i)
		if (as != b.data[i])
			return as < b.data[i];
	for (size_t i = a.size - 1; i >= b.size; --i)
		if (a.data[i] != bs)
			return a.data[i] < bs;
	for (size_t i = std::min(a.size, b.size) - 1; i != (size_t)-1; --i)
		if (a.data[i] != b.data[i])
			return a.data[i] < b.data[i];
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
		for (size_t i = a.size - 1; i != (size_t)-1; --i)
			rem = ((rem << 32) + a.data[i]) % 10;
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

void big_integer::resize(size_t nsize)
{
	uint32_t * ndata;
	if (nsize > size)
		ndata = new uint32_t[nsize];
	else
	{
		ndata = new(std::nothrow) uint32_t[nsize];
		if (ndata == nullptr)
			return;
	}

	std::copy(data, data + std::min(size, nsize), ndata);
	if (nsize > size)
		std::fill(ndata + size, ndata + nsize, (data[size - 1] >> 31 ? UINT32_MAX : 0));
	delete [] data;
	size = nsize;
	data = ndata;
}

void big_integer::shrink()
{
	uint64_t as = data[size - 1] >> 31 ? UINT32_MAX : 0;
	for (size_t i = size - 1; true; --i)
		if (data[i] != as || i == 0)
		{
			size_t cur_size = ((data[i] >> 31) == (as >> 31) ? i + 1 : i + 2);
			size_t nsize = size;
			while ((nsize >> 1) >= cur_size)
				nsize >>= 1;
			if (nsize != size)
				resize(nsize);
			return;
		}
}

void big_integer::shiftL(size_t b)
{
	resize(size + b);
	for (size_t i = size - 1; i != (size_t)-1; --i)
		data[i] = (i >= b ? data[i - b] : 0);
}
