#ifndef __MYZIP_EXCEPTIONS_H__
#define __MYZIP_EXCEPTIONS_H__

#include <exception>
#include <string>

namespace myzip
{
	struct EncoderIncorrectCharacterException : public std::exception
	{
		char const * what() const noexcept
		{
			return "Encoder error: invalid character in the input";
		}
	};

	struct DecoderNoTreeException : public std::exception
	{
		char const * what() const noexcept
		{
			return "Decoder error: trying to decode before decoding huffman tree";
		}
	};

	struct DecoderTreeException : public std::exception
	{
		char const * what() const noexcept
		{
			return "Decoder error: failed to decode huffman tree";
		}
	};
}

#endif // __MYZIP_EXCEPTIONS_H__


