#pragma once

#include <vector>
#include <string>

struct AudioDevice {
	std::string id;
	std::string direction;
	std::string state;
	std::string defaultInfo;
	std::vector<std::pair<std::string, std::string>> properties;

	bool operator <(AudioDevice& other);
};
