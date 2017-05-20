#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <utility>
#include <bitset>
#include "gtest/gtest.h"

#include "Encoder.h"
#include "Decoder.h"
#include "Exceptions.h"

TEST(correctness, frequency_distribution_add)
{
	myzip::FrequencyDistribution distrib;
	for (size_t i = 0; i < 256; ++i)
		EXPECT_EQ(distrib.getFrequency(i), 0);
	std::vector < size_t > mydistrib(256, 0);
	for (size_t iter = 0; iter < 10; ++iter)
	{
		myzip::buffer s;
		size_t len = rand() % 1000;
		for (size_t i = 0; i < len; ++i)
		{
			uint8_t c = rand() % 256;
			++mydistrib[c];
			s.push_back(c);
		}
		distrib.add(s);
		for (size_t i = 0; i < 256; ++i)
			EXPECT_EQ(distrib.getFrequency(i), mydistrib[i]);
	}
	distrib.add(myzip::buffer());
	for (size_t i = 0; i < 256; ++i)
		EXPECT_EQ(distrib.getFrequency(i), mydistrib[i]);
}

TEST(correctness, frequency_distribution_constructor)
{
	std::vector < size_t > mydistrib(256, 0);
	myzip::buffer s;
	size_t len = 1000;
	for (size_t i = 0; i < len; ++i)
	{
		uint8_t c = rand() % 256;
		++mydistrib[c];
		s.push_back(c);
	}
	myzip::FrequencyDistribution distrib(s);
	for (size_t i = 0; i < 256; ++i)
		EXPECT_EQ(distrib.getFrequency(i), mydistrib[i]);
}

TEST(correctness, bit_sequence_add)
{
	for (size_t iter = 0; iter < 5; ++iter)
	{
		myzip::BitSequence seq;
		std::vector < int > v;
		for (size_t i = 0; i < 30; ++i)
		{
			if (rand() % 2)
			{
				v.push_back(rand() % 2);
				seq.addBit(v.back());
			} else {
				uint8_t c = rand() % 256;
				seq.addChar(c);
				for (size_t j = 0; j < 8; ++j)
				{
					v.push_back(c & 1);
					c >>= 1;
				}
			}
		}
		EXPECT_EQ(v.size(), seq.size());
		for (size_t i = 0; i < v.size(); ++i)
			EXPECT_EQ(v[i], (int)seq[i]);
	}
}

TEST(correctness, bit_sequence_size_empty_clear)
{
	myzip::BitSequence seq;
	EXPECT_TRUE(seq.empty());
	EXPECT_EQ(seq.size(), 0);
	seq.addBit(1);
	seq.addBit(0);
	seq.addBit(0);
	EXPECT_FALSE(seq.empty());
	EXPECT_EQ(seq.size(), 3);
	seq.clear();
	EXPECT_TRUE(seq.empty());
	EXPECT_EQ(seq.size(), 0);
}

TEST(correctness, bit_sequence_add_pop)
{
	myzip::BitSequence seq;
	std::vector < int > v;
	for (size_t iter = 0; iter < 2000; ++iter)
	{
		if (v.size() > 0 && rand() % 7 < 3)
		{
			v.pop_back();
			seq.pop();
		} else {
			v.push_back(rand() % 2);
			seq.addBit(v.back());
		}
		EXPECT_EQ(v.size(), seq.size());
	}
	for (size_t i = 0; i < v.size(); ++i)
		EXPECT_EQ(v[i], (int)seq[i]);
}

TEST(correctness, bit_sequence_add_seq)
{
	for (size_t iter = 0; iter < 20; ++iter)
	{
		myzip::BitSequence seq1;
		std::vector < int > v1(rand() % 100, 0);
		for (int & x : v1)
		{
			x = rand() % 2;
			seq1.addBit(x);
		}
		myzip::BitSequence seq2;
		std::vector < int > v2(rand() % 100, 0);
		for (int & x : v2)
		{
			x = rand() % 2;
			seq2.addBit(x);
		}
		seq1.addSeq(seq2);
		v1.insert(v1.end(), v2.begin(), v2.end());
		EXPECT_EQ(v1.size(), seq1.size());
		for (size_t i = 0; i < v1.size(); ++i)
			EXPECT_EQ(v1[i], (int)seq1[i]);
	}
}

static void testSubseq(std::vector < int > const & v, myzip::BitSequence const & seq, size_t l, size_t r)
{
	std::vector < int > v1(v.begin() + l, v.begin() + r);
	myzip::BitSequence seq1 = seq.subsequence(l, r);
	EXPECT_EQ(v1.size(), seq1.size());
	for (size_t i = 0; i < v1.size(); ++i)
		EXPECT_EQ(v1[i], (int)seq1[i]);
}

TEST(correctness, bit_sequence_subseq)
{
	std::vector < int > v;
	myzip::BitSequence seq;
	for (size_t i = 0; i < 100; ++i)
	{
		v.push_back(rand() % 2);
		seq.addBit(v.back());
	}
	for (size_t i = 0; i < 100; ++i)
		testSubseq(v, seq, 0, i);
	for (size_t i = 1; i < 100; ++i)
		testSubseq(v, seq, i, 100);
	testSubseq(v, seq, 23, 23);
	testSubseq(v, seq, 0, 0);
	testSubseq(v, seq, 100, 100);
	for (size_t i = 0; i < 100; ++i)
	{
		size_t l = rand() % 100, r = rand() % 100;
		if (l > r)
			std::swap(l, r);
		testSubseq(v, seq, l, r);
	}

	myzip::BitSequence empty;
	EXPECT_TRUE(empty.subsequence(0, 0).empty());
}

TEST(correctness, bit_sequence_resize)
{
	myzip::BitSequence seq, seq1;
	for (size_t i = 0; i < 100; ++i)
	{
		bool b = rand() % 2;
		seq.addBit(b);
		seq1.addBit(b);
	}

	seq1.resize(100);
	EXPECT_EQ(seq1.size(), 100);
	for (size_t i = 0; i < 100; ++i)
		EXPECT_EQ(seq[i], seq1[i]);

	seq1.resize(150);
	EXPECT_EQ(seq1.size(), 150);
	for (size_t i = 0; i < 100; ++i)
		EXPECT_EQ(seq[i], seq1[i]);
	for (size_t i = 100; i < 150; ++i)
		EXPECT_EQ(0, seq1[i]);

	seq1.resize(400);
	EXPECT_EQ(seq1.size(), 400);
	for (size_t i = 0; i < 100; ++i)
		EXPECT_EQ(seq[i], seq1[i]);
	for (size_t i = 100; i < 400; ++i)
		EXPECT_EQ(0, seq1[i]);

	seq1.resize(75);
	EXPECT_EQ(seq1.size(), 75);
	for (size_t i = 0; i < 75; ++i)
		EXPECT_EQ(seq[i], seq1[i]);

	seq1.resize(40);
	EXPECT_EQ(seq1.size(), 40);
	for (size_t i = 0; i < 40; ++i)
		EXPECT_EQ(seq[i], seq1[i]);
}

TEST(correctness, bit_sequence_data)
{
	myzip::BitSequence seq;
	for (size_t i = 0; i < 123; ++i)
		seq.addBit(rand() % 2);
	for (size_t i = 0; i < 123; ++i)
		EXPECT_EQ((seq.data()[i / 8] >> (i % 8)) & 1, seq[i]);
	for (size_t i = 0; i < (123 + 7) / 8; ++i)
		seq.data()[i] = rand() % 256;
	for (size_t i = 0; i < 123; ++i)
		EXPECT_EQ((seq.data()[i / 8] >> (i % 8)) & 1, seq[i]);
}

TEST(correctness, encoder_exception)
{
	myzip::Encoder encoder(myzip::FrequencyDistribution({1, 1, 2, 3}));
	bool ok = false;
	try
	{
		encoder.encode({1, 2, 4});
	} catch (myzip::EncoderIncorrectCharacterException const &)
	{
		ok = true;
	}
	EXPECT_TRUE(ok);
	try
	{
		encoder.encode({1, 2, 3});
		encoder.encode({});
	} catch (myzip::EncoderIncorrectCharacterException const &)
	{
		EXPECT_FALSE(true);
	}
}

TEST(correctness, encoder_encode_variants)
{
	for (size_t len1 : {0, 8, 5, 15, 123, 128})
	{
		myzip::BitSequence seq1, seq2;
		for (size_t i = 0; i < len1; ++i)
			seq1.addBit(rand() % 2);
		myzip::buffer s(rand() % 16);
		for (uint8_t & c : s)
			c = rand() % 256;
		myzip::Encoder encoder = myzip::Encoder(myzip::FrequencyDistribution(s));
		seq2 = encoder.encode(s);
		encoder.encode(s, seq1);
		EXPECT_EQ(seq2.size() + len1, seq1.size());
		for (size_t i = 0; i < seq2.size(); ++i)
			EXPECT_EQ(seq2[i], seq1[len1 + i]);
	}
}

TEST(correctness, decoder_no_tree_exception)
{
	bool ok = false;
	myzip::Decoder decoder;
	try
	{
		size_t e;
		decoder.decode(myzip::BitSequence(), 0, e);
	} catch (myzip::DecoderNoTreeException const &)
	{
		ok = true;
	}
	EXPECT_TRUE(ok);

	ok = false;
	decoder.decodeHuffmanTree(myzip::BitSequence());
	try
	{
		size_t e;
		decoder.decode(myzip::BitSequence(), 0, e);
	} catch (myzip::DecoderNoTreeException const &)
	{
		ok = true;
	}
	EXPECT_TRUE(ok);
}

TEST(correctness, encode_decode)
{
	for (size_t cc : {1, 2, 2, 3, 3, 10, 10, 100, 100, 200, 200, 256, 256})
	{
		myzip::buffer s(rand() % 1500 + 500);
		for (uint8_t & x : s)
			x = rand() % cc;
		
		myzip::Encoder encoder = myzip::Encoder(myzip::FrequencyDistribution(s));
		myzip::BitSequence tree = encoder.encodeHuffmanTree();
		myzip::BitSequence seq = encoder.encode(s);

		myzip::Decoder decoder;
		decoder.decodeHuffmanTree(tree);
		size_t end;
		EXPECT_EQ(s, decoder.decode(seq, end));
		EXPECT_EQ(end, seq.size());
	}
}

TEST(correctness, encode_decode_start)
{
	for (size_t cc : {1, 2, 2, 3, 3, 10, 10, 100, 100, 200, 200, 256, 256})
	{
		myzip::buffer s(rand() % 1500 + 500);
		for (uint8_t & x : s)
			x = rand() % cc;
		
		myzip::Encoder encoder = myzip::Encoder(myzip::FrequencyDistribution(s));
		myzip::BitSequence tree = encoder.encodeHuffmanTree();
		myzip::BitSequence seq;
		size_t len1 = rand() % 20 + 10;
		for (size_t i = 0; i < len1; ++i)
			seq.addBit(rand() % 2);
		encoder.encode(s, seq);

		myzip::Decoder decoder;
		decoder.decodeHuffmanTree(tree);
		size_t end;
		EXPECT_EQ(s, decoder.decode(seq, len1, end));
		EXPECT_EQ(end, seq.size());
	}
}

TEST(correctness, encode_concat)
{
	for (size_t cc : {1, 2, 2, 3, 3, 10, 10, 100, 100, 200, 200, 256, 256})
	{
		myzip::buffer s1(rand() % 1500 + 500);
		for (uint8_t & x : s1)
			x = rand() % cc;
		myzip::buffer s2(rand() % 1500 + 500);
		for (uint8_t & x : s2)
			x = rand() % cc;
		myzip::buffer ss = s1;
		ss.insert(ss.end(), s2.begin(), s2.end());

		myzip::FrequencyDistribution distrib;
		distrib.add(s1);
		distrib.add(s2);
		
		myzip::Encoder encoder = myzip::Encoder(distrib);
		myzip::BitSequence seq1 = encoder.encode(ss);
		myzip::BitSequence seq2 = encoder.encode(s1);
		encoder.encode(s2, seq2);
		
		EXPECT_EQ(seq1.size(), seq2.size());
		for (size_t i = 0; i < seq1.size(); ++i)
			EXPECT_EQ(seq1[i], seq2[i]);
	}
}
