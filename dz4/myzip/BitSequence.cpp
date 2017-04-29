#include "BitSequence.h"

namespace myzip
{
	BitSequence::BitSequence() : 
		len(0)
	{
	}

	size_t BitSequence::size() const
	{
		return len;
	}

	bool BitSequence::empty() const
	{
		return len == 0;
	}

	bool BitSequence::operator [] (size_t i) const
	{
		return (s[i >> 3] >> (i & 0b111)) & 1;
	}

	uint8_t * BitSequence::data()
	{
		return s.data();
	}

	void BitSequence::addBit(bool a)
	{
		if ((len & 0b111) == 0)
			s.push_back(a);
		else if (a)
			s.back() |= 1 << (len & 0b111);
		++len;
	}

	void BitSequence::addChar(uint8_t c)
	{
		uint8_t x = c;
		for (size_t i = 0; i < 8; ++i)
		{
			addBit(x & 1);
			x >>= 1;
		}
	}

	void BitSequence::addSeq(BitSequence const & b)
	{
		for (size_t i = 0; i < b.size(); ++i)
			addBit(b[i]);
	}

	void BitSequence::pop()
	{
		--len;
		s.back() &= ~(1 << (len & 0b111));
		if ((len & 0b111) == 0)
			s.pop_back();
	}
	
	void BitSequence::clear()
	{
		s.clear();
		len = 0;
	}

	void BitSequence::resize(size_t n)
	{
		s.resize((n + 7) / 8);
		len = n;
		if (n < len && (len & 0b111) != 0)
			s.back() &= (1 << (len & 0b111)) - 1;
	}

	BitSequence BitSequence::subsequence(size_t start, size_t end) const
	{
		if ((start & 0b111) == 0)
		{
			BitSequence r(std::vector<uint8_t>(s.begin() + (start >> 3), s.begin() + ((end + 7) >> 3)), end - start);
			if ((r.len & 0b111) != 0)
				r.s.back() &= (1 << (r.len & 0b111)) - 1;
			return r;
		} else
		{
			BitSequence r;
			for (size_t i = start; i < end; ++i)
				r.addBit((*this)[i]);
			return r;
		}
	}

	BitSequence::BitSequence(std::vector < uint8_t > && ns, size_t nlen) :
		s(ns), len(nlen)
	{
	}
}
