#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct AudioDevice {
	std::string id;
	std::string direction;
	std::string state;
	std::string defaultInfo;
	std::vector<std::pair<std::string, std::string>> properties;
};

std::vector<AudioDevice> GetAudioDevices();
