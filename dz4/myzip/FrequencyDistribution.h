#ifndef __MYZIP_FREQUENCY_DISTRIBUTION_H__
#define __MYZIP_FREQUENCY_DISTRIBUTION_H__

#include <vector>
#include <cstdlib>
#include <cstdint>

namespace myzip
{
	typedef std::vector<uint8_t> buffer;

	class FrequencyDistribution
	{
	public:
		FrequencyDistribution();
		explicit FrequencyDistribution(buffer const & s);

		void add(buffer const & s);
		size_t getFrequency(uint8_t c) const;
	
	private:
		size_t freq[256];
	};
}

#endif // __MYZIP_FREQUENCY_DISTRIBUTION_H__

