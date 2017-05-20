#include "ZipUnzip.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <Encoder.h>

static myzip::FrequencyDistribution getFrequencyDistribution(std::istream & inFile)
{
	myzip::FrequencyDistribution distrib;
	myzip::buffer s;
	while (!inFile.eof())
	{
		s.resize(ZIP_BLOCK_SIZE);
		inFile.read((char*)s.data(), ZIP_BLOCK_SIZE);
		s.resize(inFile.gcount());
		distrib.add(s);
	}
	inFile.clear();
	return distrib;
}

static void writeCompressedData(myzip::Encoder const & encoder, std::istream & inFile, std::ostream & outFile)
{
	myzip::buffer s;
	myzip::BitSequence seq;
	while (!inFile.eof())
	{
		s.resize(ZIP_BLOCK_SIZE);
		inFile.read((char*)s.data(), ZIP_BLOCK_SIZE);
		s.resize(inFile.gcount());

		encoder.encode(s, seq);
		outFile.write((char*)seq.data(), seq.size() / 8);
		seq = seq.subsequence(seq.size() / 8 * 8, seq.size());
	}
	inFile.clear();
	outFile.write((char*)seq.data(), (seq.size() + 7) / 8);
}

int mainZip(std::string const & inFilename, std::string const & outFilename)
{
	std::ifstream inFile(inFilename, std::ios_base::in | std::ios_base::binary);
	if (inFile.fail())
	{
		std::cerr << "Failed to open source file " << inFilename << "\n";
        return 2;
	}
	std::ofstream outFile(outFilename, std::ios_base::out | std::ios_base::binary);
	if (outFile.fail())
	{
		std::cerr << "Failed to open destination file " << outFilename << "\n";
        return 2;
	}

	auto distrib = getFrequencyDistribution(inFile);
	uint64_t inSize = inFile.tellg();
	inFile.seekg(0, std::ios_base::beg);

	myzip::Encoder encoder(distrib);
	myzip::BitSequence tree = encoder.encodeHuffmanTree();
	uint16_t treeSize = tree.size();
	outFile.write((char*)&treeSize, 2);
	outFile.write((char*)tree.data(), (treeSize + 7) / 8);
	outFile.write((char*)&inSize, 8);
	writeCompressedData(encoder, inFile, outFile);
    return 0;
}
