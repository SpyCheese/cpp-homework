#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iostream>
#include <string>

class big_integer
{
public:
	big_integer();
	big_integer(big_integer const & b);
	big_integer(big_integer && b);
	explicit big_integer(std::string const & s);
	big_integer(int b);
	big_integer(uint32_t b);

	~big_integer();

	void swap(big_integer & b);
	big_integer & operator = (big_integer const & b);
	big_integer & operator = (big_integer && b);

	big_integer & operator += (big_integer const & b);
	big_integer & operator -= (big_integer const & b);
	big_integer & operator *= (big_integer const & b);
	big_integer & operator /= (big_integer const & b);
	big_integer & operator %= (big_integer const & b);
	big_integer & operator &= (big_integer const & b);
	big_integer & operator |= (big_integer const & b);
	big_integer & operator ^= (big_integer const & b);
	big_integer & operator <<= (int b);
	big_integer & operator >>= (int b);

	big_integer operator + () const;
	big_integer operator - () const;
	big_integer operator ~ () const;

	big_integer & operator ++ ();
	big_integer & operator -- ();
	big_integer operator ++ (int);
	big_integer operator -- (int);

	friend big_integer operator + (big_integer a, big_integer const & b);
	friend big_integer operator - (big_integer a, big_integer const & b);
	friend big_integer operator * (big_integer const & a, big_integer const & b);
	friend big_integer operator / (big_integer a, big_integer const & b);
	friend big_integer operator % (big_integer a, big_integer const & b);
	friend big_integer operator & (big_integer a, big_integer const & b);
	friend big_integer operator | (big_integer a, big_integer const & b);
	friend big_integer operator ^ (big_integer a, big_integer const & b);
	friend big_integer operator << (big_integer a, int b);
	friend big_integer operator >> (big_integer a, int b);

	friend bool operator == (big_integer const & a, big_integer const & b);
	friend bool operator != (big_integer const & a, big_integer const & b);
	friend bool operator < (big_integer const & a, big_integer const & b);
	friend bool operator > (big_integer const & a, big_integer const & b);
	friend bool operator <= (big_integer const & a, big_integer const & b);
	friend bool operator >= (big_integer const & a, big_integer const & b);

	friend std::string to_string(big_integer a);

private:
	size_t size;
	uint32_t * data;

	void resize(size_t nsize);
	void shrink();

	void shiftL(size_t b);
	void divModImpl(big_integer const & b, big_integer & r);
	void subShifted(big_integer const & b, size_t shift);
	bool lessShifted(big_integer const & b, size_t shift);
};

big_integer operator + (big_integer a, big_integer const & b);
big_integer operator - (big_integer a, big_integer const & b);
big_integer operator * (big_integer const & a, big_integer const & b);
big_integer operator / (big_integer a, big_integer const & b);
big_integer operator % (big_integer a, big_integer const & b);
big_integer operator & (big_integer a, big_integer const & b);
big_integer operator | (big_integer a, big_integer const & b);
big_integer operator ^ (big_integer a, big_integer const & b);
big_integer operator << (big_integer a, int b);
big_integer operator >> (big_integer a, int b);

bool operator == (big_integer const & a, big_integer const & b);
bool operator != (big_integer const & a, big_integer const & b);
bool operator < (big_integer const & a, big_integer const & b);
bool operator > (big_integer const & a, big_integer const & b);
bool operator <= (big_integer const & a, big_integer const & b);
bool operator >= (big_integer const & a, big_integer const & b);

std::string to_string(big_integer a);
std::istream & operator >> (std::istream & in, big_integer & a);
std::ostream & operator << (std::ostream & out, big_integer const & a);

#endif // BIG_INTEGER_H
