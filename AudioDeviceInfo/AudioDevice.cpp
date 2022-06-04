#include "AudioDevice.h"

bool AudioDevice::operator<(AudioDevice& other)
{
	if (direction < other.direction) return true;
	if (direction > other.direction) return false;

	if (state < other.state) return true;
	if (state > other.state) return false;

	for (auto& propFirst : properties)
	{
		auto& propSecond = *find_if(other.properties.begin(), other.properties.end(), [&](auto& x) { return x.first == propFirst.first; });
		if (propFirst.first < propSecond.first) return true;
		if (propFirst.first > propSecond.first) return false;
	}

	return false;
}
