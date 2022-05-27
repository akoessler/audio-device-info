#include <windows.h>
#include <atlbase.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>

#include "AudioFunctions.h"

using namespace std;

string WCharPtrToString(const LPCWSTR wcharptr)
{
	if (wcharptr == nullptr)
	{
		return string();
	}
	int length = WideCharToMultiByte(CP_UTF8, 0, wcharptr, -1, 0, 0, 0, 0);
	string buffer(length, 0);
	WideCharToMultiByte(CP_UTF8, 0, wcharptr, -1, (LPSTR)buffer.data(), length, 0, 0);
	buffer.resize(length - 1);
	return buffer;
}

string GetDirectionString(const EDataFlow direction)
{
	switch (direction)
	{
	case eCapture:
		return "INPUT";
	case eRender:
		return "OUTPUT";
	default:
		return "<UNKNOWN>";
	}
}

string GetStateString(const CComPtr<IMMDevice> comDevice)
{
	if (comDevice == nullptr)
	{
		return string();
	}

	DWORD nativeState;
	comDevice->GetState(&nativeState);
	switch (nativeState)
	{
	case DEVICE_STATE_ACTIVE:
		return "DEVICE_STATE_ACTIVE";
	case DEVICE_STATE_DISABLED:
		return "DEVICE_STATE_DISABLED";
	case DEVICE_STATE_NOTPRESENT:
		return "DEVICE_STATE_NOTPRESENT";
	case DEVICE_STATE_UNPLUGGED:
		return "DEVICE_STATE_UNPLUGGED";
	default:
		return "<UNKNOWN>";
	}
}

string GetDeviceId(const CComPtr<IMMDevice> comDevice)
{
	if (comDevice == nullptr)
	{
		return string();
	}

	LPWSTR nativeID;
	comDevice->GetId(&nativeID);
	return WCharPtrToString(nativeID);
}

string GetPropertyValueString(PROPVARIANT& propertyValue)
{
	WCHAR buffer[1024];
	auto hresult = PropVariantToString(propertyValue, buffer, ARRAYSIZE(buffer));
	return WCharPtrToString(buffer);
}

string GetPropertyNameString(PROPERTYKEY& key)
{
	PWSTR buffer;
	auto hresult = PSGetNameFromPropertyKey(key, &buffer);
	return WCharPtrToString(buffer);
}

string GetProperty(const CComPtr<IMMDevice> comDevice, const PROPERTYKEY& propertyKey)
{
	if (comDevice == nullptr)
	{
		return string();
	}

	CComPtr<IPropertyStore> propertyStore;
	comDevice->OpenPropertyStore(STGM_READ, &propertyStore);

	PROPVARIANT propertyValue;
	propertyStore->GetValue(propertyKey, &propertyValue);
	return GetPropertyValueString(propertyValue);
}

string GetDefaultAudioDeviceID(const CComPtr<IMMDeviceEnumerator> comDeviceEnumerator, const EDataFlow direction, const ERole role)
{
	CComPtr<IMMDevice> comDevice;
	comDeviceEnumerator->GetDefaultAudioEndpoint(direction, role, &comDevice);
	return GetDeviceId(comDevice);
}


void FillDefaultInfo(const CComPtr<IMMDeviceEnumerator> comDeviceEnumerator, vector<AudioDevice>& devices)
{
	unordered_map<string, string> defaultsMap;
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eCapture, eCommunications)] = "communications";
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eCapture, eConsole)] = "console";
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eCapture, eMultimedia)] = "multimedia";
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eRender, eCommunications)] = "communications";
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eRender, eConsole)] = "console";
	defaultsMap[GetDefaultAudioDeviceID(comDeviceEnumerator, eRender, eMultimedia)] = "multimedia";

	for (auto& device : devices)
	{
		device.defaultInfo = defaultsMap[device.id];
	}
}

void AddAudioDevices(const EDataFlow direction, const CComPtr<IMMDeviceEnumerator> comDeviceEnumerator, vector<AudioDevice>& devices)
{
	CComPtr<IMMDeviceCollection> comDeviceCollection;
	comDeviceEnumerator->EnumAudioEndpoints(direction, DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED, &comDeviceCollection);

	UINT deviceCount;
	comDeviceCollection->GetCount(&deviceCount);

	for (UINT i = 0; i < deviceCount; ++i)
	{
		CComPtr<IMMDevice> comDevice;
		comDeviceCollection->Item(i, &comDevice);

		AudioDevice audioDevice;
		audioDevice.id = GetDeviceId(comDevice);
		audioDevice.direction = GetDirectionString(direction);
		audioDevice.state = GetStateString(comDevice);

#define ADD_PROPERTY_VALUE(key) audioDevice.properties.push_back(pair<string, string>(#key, GetProperty(comDevice, key)))

		ADD_PROPERTY_VALUE(PKEY_DeviceInterface_FriendlyName);
		ADD_PROPERTY_VALUE(PKEY_Device_DeviceDesc);
		ADD_PROPERTY_VALUE(PKEY_Device_FriendlyName);
		ADD_PROPERTY_VALUE(PKEY_AudioEndpoint_GUID);

#undef ADD_PROPERTY_VALUE

		devices.push_back(audioDevice);
	}
}

vector<AudioDevice> GetAudioDevices()
{
	vector<AudioDevice> devices;

	CComPtr<IMMDeviceEnumerator> comDeviceEnumerator;
	auto createResult = comDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	if (!SUCCEEDED(createResult))
	{
		return devices;
	}

	AddAudioDevices(EDataFlow::eRender, comDeviceEnumerator, devices);
	AddAudioDevices(EDataFlow::eCapture, comDeviceEnumerator, devices);

	FillDefaultInfo(comDeviceEnumerator, devices);

	return devices;
}
