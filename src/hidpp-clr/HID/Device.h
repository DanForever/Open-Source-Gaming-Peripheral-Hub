#pragma once

#include "../Enumerator.h"
#include "../../hidpp/HID/Device.h"

namespace Managed::HID
{
	using NativeDevice = CLI::ManagedObject<Native::HID::Device>;

	public ref class Device
	{
	public:
		Device();

		bool Open( PathCollection^ paths );

		property String^ Manufacturer { String^ get() { return GetManufacturer(); } }

	private:
		String^ GetManufacturer();

		NativeDevice^ m_device;
		PathCollection^ m_paths;

		int m_validPathIndex;
	};
}
