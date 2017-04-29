#ifndef __MYZIP_DECODER_H__
#define __MYZIP_DECODER_H__

#include <string>
#include <memory>
#include "BitSequence.h"
#include "FrequencyDistribution.h"
#include "_HuffmanTree.h"

namespace myzip
{
	class Decoder
	{
	public:
		Decoder();
		void decodeHuffmanTree(BitSequence const & encodedTree);
		buffer decode(BitSequence const & seq, size_t start, size_t & end) const;
		buffer decode(BitSequence const & seq, size_t & end) const;
	
	private:
		std::shared_ptr < _HuffmanTree > tree;
	};
}

#endif // __MYZIP_DECODER_H__

