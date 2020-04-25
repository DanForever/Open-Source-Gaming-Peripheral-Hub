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

		property String^ Product { String^ get() { return GetProduct(); } }
		property String^ Manufacturer { String^ get() { return GetManufacturer(); } }
		property unsigned short ProductId { unsigned short get() { return GetProductId(); } }
		property unsigned short VendorId { unsigned short get() { return GetVendorId(); } }
		property unsigned short Version { unsigned short get() { return GetVersion(); } }
		property String^ Protocol { String^ get() { return GetProtocol(); } }

	private:
		String^ GetProduct();
		String^ GetManufacturer();
		unsigned short GetProductId();
		unsigned short GetVendorId();
		unsigned short GetVersion();
		String^ GetProtocol();

		NativeDevice^ m_device;
		PathCollection^ m_paths;

		int m_validPathIndex;
	};
}
