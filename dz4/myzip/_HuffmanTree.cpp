#include "_HuffmanTree.h"

namespace myzip
{
	void _HuffmanTree::encode(BitSequence & result)
	{
		if (l == nullptr)
		{
			result.addBit(1);
			result.addChar(c);
		} else
		{
			result.addBit(0);
			l->encode(result);
			r->encode(result);
		}
	}

	void _HuffmanTree::getCodes(BitSequence cur, BitSequence * result)
	{
		if (l == nullptr)
		{
			result[c] = cur;
			return;
		} else {
			cur.addBit(0);
			l->getCodes(cur, result);
			cur.pop();
			cur.addBit(1);
			r->getCodes(cur, result);
		}
	}
}
