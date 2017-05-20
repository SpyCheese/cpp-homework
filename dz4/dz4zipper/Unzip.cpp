#include "ZipUnzip.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <Decoder.h>
#include <Exceptions.h>

struct InvalidFileException : public std::exception
{
    char const * what() const noexcept
    {
        return "Invalid source file";
    }
};

static myzip::BitSequence readHuffmanTree(std::istream & inFile)
{
	uint16_t treeSize = 0;
	inFile.read((char*)&treeSize, 2);
	myzip::BitSequence seq;
	seq.resize(treeSize);
	inFile.read((char*)seq.data(), (treeSize + 7) / 8);
	if (inFile.fail())
        throw InvalidFileException();
	return seq;
}

static void writeDecompressedData(myzip::Decoder const & decoder, std::istream & inFile, std::ostream & outFile, size_t outSize)
{
	myzip::BitSequence seq;
	size_t pos = 0, seqSize = 0;
	while (outSize > 0 && !inFile.eof())
	{
		seqSize = seq.size() / 8;
		seq.resize((seqSize + UNZIP_BLOCK_SIZE) * 8);
		inFile.read((char*)seq.data() + seqSize, UNZIP_BLOCK_SIZE);
		seq.resize((seqSize + inFile.gcount()) * 8);

		size_t npos = 0;
		myzip::buffer data = decoder.decode(seq, pos, npos);
		pos = npos % 8;
		seq = seq.subsequence(npos / 8 * 8, seq.size());

		if (outSize > data.size())
		{
			outFile.write((char*)data.data(), data.size());
			outSize -= data.size();
		} else
		{
			outFile.write((char*)data.data(), outSize);
			outSize = 0;
		}
	}
	inFile.clear();
	if (outSize > 0)
        throw InvalidFileException();
}

int mainUnzip(std::string const & inFilename, std::string const & outFilename)
{
    try
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

        myzip::BitSequence tree = readHuffmanTree(inFile);
        myzip::Decoder decoder;
        try
        {
            decoder.decodeHuffmanTree(tree);
        } catch (myzip::DecoderTreeException const &)
        {
            throw InvalidFileException();
        }

        uint64_t outSize = 0;
        inFile.read((char*)&outSize, 8);
        if (inFile.fail())
            throw InvalidFileException();

        writeDecompressedData(decoder, inFile, outFile, outSize);
    } catch (InvalidFileException const & ex)
    {
        std::cerr << ex.what() << "\n";
        return 3;
    }
    return 0;
}
