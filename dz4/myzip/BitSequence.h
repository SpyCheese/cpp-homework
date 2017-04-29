#ifndef __MYZIP_BIT_SEQUENCE_H__
#define __MYZIP_BIT_SEQUENCE_H__

#include <vector>
#include <cstdlib>
#include <cstdint>

namespace myzip
{
	class BitSequence
	{
	public:
		BitSequence();

		size_t size() const;
		bool empty() const;
		bool operator [] (size_t i) const;
		uint8_t * data();

		void addBit(bool a);
		void addChar(uint8_t c);
		void addSeq(BitSequence const & b);
		void pop();
		void clear();
		void resize(size_t n);

		BitSequence subsequence(size_t start, size_t end) const;
	
	private:
		std::vector < uint8_t > s;
		size_t len;

		BitSequence(std::vector < uint8_t > && ns, size_t nlen);
	};
}

#endif // __MYZIP_BIT_SEQUENCE_H__

