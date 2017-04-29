#include "Encoder.h"
#include "Exceptions.h"
#include "_HuffmanTree.h"
#include <memory>
#include <queue>
#include <algorithm>

namespace myzip
{
	Encoder::Encoder(FrequencyDistribution const & distrib)
	{
		typedef std::pair < size_t, std::shared_ptr < _HuffmanTree > > qpair;
		std::priority_queue < qpair, std::vector<qpair>, std::greater<qpair> > s;
		for (size_t i = 0; i < 256; ++i)
		{
			size_t freq = distrib.getFrequency(i);
			if (freq != 0)
				s.emplace(freq, new _HuffmanTree({nullptr, nullptr, (uint8_t)i}));
		}
		if (s.empty())
			return;
		if (s.size() == 1)
			s.push(s.top());

		while (s.size() > 1)
		{
			auto v1 = s.top();
			s.pop();
			auto v2 = s.top();
			s.pop();
			s.emplace(v1.first + v2.first, new _HuffmanTree({v1.second, v2.second, 0}));
		}

		auto root = s.top().second;
		root->encode(huffmanTreeCode);
		root->getCodes(BitSequence(), code);
	}

	BitSequence Encoder::encodeHuffmanTree() const
	{
		return huffmanTreeCode;
	}

	BitSequence Encoder::encode(buffer const & s) const
	{
		BitSequence result;
		encode(s, result);
		return result;
	}

	void Encoder::encode(buffer const & s, BitSequence & result) const
	{
		for (uint8_t c : s)
		{
			BitSequence const & x = code[c];
			if (x.empty())
				throw EncoderIncorrectCharacterException();
			result.addSeq(x);
		}
	}
}
