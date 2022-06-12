#include "AudioDevice.h"

bool AudioDevice::operator<(AudioDevice& other)
{
	if (direction < other.direction) return true;
	if (direction > other.direction) return false;

	if (state < other.state) return true;
	if (state > other.state) return false;

	for (auto& prop : properties)
	{
		if (prop.first.find("GUID") != std::string::npos)
		{
			continue;
		}

		auto& propOther = *find_if(other.properties.begin(), other.properties.end(), [&](auto& x) { return x.first == prop.first; });
		if (prop.second < propOther.second) return true;
		if (prop.second > propOther.second) return false;
	}

	return false;
}
