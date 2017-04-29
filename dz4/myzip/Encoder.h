#ifndef __MYZIP_ENCODER_H__
#define __MYZIP_ENCODER_H__

#include "FrequencyDistribution.h"
#include "BitSequence.h"

namespace myzip
{
	class Encoder
	{
	public:
		explicit Encoder(FrequencyDistribution const & distrib);

		BitSequence encodeHuffmanTree() const;
		BitSequence encode(buffer const & s) const;
		void encode(buffer const & s, BitSequence & result) const;
	
	private:
		BitSequence code[256];
		BitSequence huffmanTreeCode;
	};
}

#endif // __MYZIP_ENCODER_H__

