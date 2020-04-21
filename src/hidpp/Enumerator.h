#pragma once

#include "os.h"
#include <SetupAPI.h>

#include <vector>

#include "PathResolver.h"

namespace Device
{
	class Enumerator
	{
	public:
		Enumerator()
		{
			RequestDeviceHandle();
			EnumerateDevices();
		}

		auto begin() const { return m_devices.begin(); }
		auto end() const { return m_devices.end(); }

	private:
		void RequestDeviceHandle();
		void EnumerateDevices();

		GUID m_hidGuid;
		HDEVINFO m_DeviceInfoHandle;
		std::vector<PathResolver> m_devices;
	};
}
