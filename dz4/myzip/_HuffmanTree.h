#ifndef __MYZIP_HUFFMAN_TREE_H__
#define __MYZIP_HUFFMAN_TREE_H__

#include <memory>
#include "BitSequence.h"

namespace myzip
{
	struct _HuffmanTree
	{
		std::shared_ptr < _HuffmanTree > l, r;
		uint8_t c;
		void encode(BitSequence & result);
		void getCodes(BitSequence cur, BitSequence * result);
	};
}

#endif // __MYZIP_HUFFMAN_TREE_H__

