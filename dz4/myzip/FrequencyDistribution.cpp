#include "FrequencyDistribution.h"
#include <algorithm>

namespace myzip
{
	FrequencyDistribution::FrequencyDistribution()
	{
		std::fill(freq, freq + 256, 0);
	}

	FrequencyDistribution::FrequencyDistribution(buffer const & s) :
		FrequencyDistribution()
	{
		add(s);
	}

	void FrequencyDistribution::add(buffer const & s)
	{
		for (uint8_t c : s)
			++freq[c];
	}

	size_t FrequencyDistribution::getFrequency(uint8_t c) const
	{
		return freq[c];
	}
}
