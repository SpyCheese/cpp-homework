#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iostream>
#include <string>

class big_integer
{
public:
	big_integer();
	big_integer(big_integer const & b);
	explicit big_integer(std::string const & s);
	big_integer(int b);
	big_integer(uint32_t b);

	~big_integer();

	void swap(big_integer & b);
	big_integer & operator = (big_integer const & b);

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

	friend big_integer operator + (big_integer const & a, big_integer const & b);
	friend big_integer operator - (big_integer const & a, big_integer const & b);
	friend big_integer operator * (big_integer const & a, big_integer const & b);
	friend big_integer operator / (big_integer const & a, big_integer const & b);
	friend big_integer operator % (big_integer const & a, big_integer const & b);
	friend big_integer operator & (big_integer const & a, big_integer const & b);
	friend big_integer operator | (big_integer const & a, big_integer const & b);
	friend big_integer operator ^ (big_integer const & a, big_integer const & b);
	friend big_integer operator << (big_integer const & a, int b);
	friend big_integer operator >> (big_integer const & a, int b);

	friend bool operator == (big_integer const & a, big_integer const & b);
	friend bool operator != (big_integer const & a, big_integer const & b);
	friend bool operator < (big_integer const & a, big_integer const & b);
	friend bool operator > (big_integer const & a, big_integer const & b);
	friend bool operator <= (big_integer const & a, big_integer const & b);
	friend bool operator >= (big_integer const & a, big_integer const & b);

	friend std::string to_string(big_integer a);

private:
	size_t size;
    static size_t const SMALL_SIZE = 2;
    union
    {
        uint32_t * ptr;
        uint32_t arr[SMALL_SIZE];
    } dataUnion;
    uint32_t * getData();
    uint32_t const * getData() const;

	void resize(size_t nsize);
    void duplicate();
	void shrink();

    static std::pair < big_integer, big_integer > divModImpl(big_integer const & ina, big_integer const & b);
};

big_integer operator + (big_integer const & a, big_integer const & b);
big_integer operator - (big_integer const & a, big_integer const & b);
big_integer operator * (big_integer const & a, big_integer const & b);
big_integer operator / (big_integer const & a, big_integer const & b);
big_integer operator % (big_integer const & a, big_integer const & b);
big_integer operator & (big_integer const & a, big_integer const & b);
big_integer operator | (big_integer const & a, big_integer const & b);
big_integer operator ^ (big_integer const & a, big_integer const & b);
big_integer operator << (big_integer const & a, int b);
big_integer operator >> (big_integer const & a, int b);

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
