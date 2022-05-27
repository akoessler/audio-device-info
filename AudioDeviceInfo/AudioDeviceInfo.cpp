#include <iostream>
#include <iomanip>
#include <combaseapi.h>

#include "AudioFunctions.h"

using namespace std;

template<typename T> void printLine(const string name, const T& value)
{
	cout << left << setw(35) << name << value << endl;
}

int main()
{
	auto initResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(initResult)) {
		cout << "CoInitializeEx failed";
		return 1;
	}

	auto devices = GetAudioDevices();

	for (auto& device : devices)
	{
		printLine("ID", device.id);
		printLine("State", device.state);
		printLine("Default", device.defaultInfo);
		printLine("Direction", device.direction);
		for (auto& prop : device.properties)
		{
			printLine(prop.first, prop.second);
		}
		cout << endl;
		cout << endl;
	}

	return 0;
}
