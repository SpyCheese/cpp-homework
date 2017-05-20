#include "Decoder.h"
#include "Exceptions.h"

namespace myzip
{
	Decoder::Decoder()
	{
	}

	static std::shared_ptr < _HuffmanTree > decodeTree(BitSequence const & seq, size_t & i)
	{
		if (i == seq.size())
			throw DecoderTreeException();
		if (seq[i++])
		{
			uint8_t c = 0;
			if (i + 8 > seq.size())
				throw DecoderTreeException();
			for (size_t j = 0; j < 8; ++j)
				if (seq[i++])
					c |= 1 << j;
			return std::shared_ptr<_HuffmanTree>(new _HuffmanTree({nullptr, nullptr, c}));
		} else {
			auto l = decodeTree(seq, i);
			auto r = decodeTree(seq, i);
			return std::shared_ptr<_HuffmanTree>(new _HuffmanTree({l, r, 0}));
		}
	}

	void Decoder::decodeHuffmanTree(BitSequence const & encodedTree)
	{
		if (encodedTree.empty())
		{
			tree = nullptr;
			return;
		}
		size_t i = 0;
		tree = decodeTree(encodedTree, i);
		if (i != encodedTree.size() || tree->l == nullptr)
			throw DecoderTreeException();
	}

	buffer Decoder::decode(BitSequence const & seq, size_t start, size_t & end) const
	{
		if (tree == nullptr)
			throw DecoderNoTreeException();
		buffer result;
		end = start;
		size_t i = start;
		while (true)
		{
			auto cur = tree;
			while (i < seq.size() && cur->l != nullptr)
				cur = (seq[i++] ? cur->r : cur->l);
			if (cur->l != nullptr)
				break;
			result.push_back(cur->c);
			end = i;
		}
		return result;
	}

	buffer Decoder::decode(BitSequence const & seq, size_t & end) const
	{
		return decode(seq, 0, end);
	}
}

