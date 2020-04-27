#pragma once

#include "../Utilities/ManagedObject.h"
#include "../Enumerator.h"

namespace Native::HID { class Device; }

namespace Managed::HID
{
	using NativeDevice = Utilities::ManagedObject<Native::HID::Device>;

	public ref class Device
	{
	public:
		Device();

		bool Open( PathCollection^ paths );
		bool Open( String^ path );

		property String^ Product { String^ get() { return GetProduct(); } }
		property String^ Manufacturer { String^ get() { return GetManufacturer(); } }
		property unsigned short ProductId { unsigned short get() { return GetProductId(); } }
		property unsigned short VendorId { unsigned short get() { return GetVendorId(); } }
		property unsigned short Version { unsigned short get() { return GetVersion(); } }
		property String^ Protocol { String^ get() { return GetProtocol(); } }

	internal:
		property NativeDevice^ NativeDevice { HID::NativeDevice^ get() { return m_device; } }

	private:
		String^ GetProduct();
		String^ GetManufacturer();
		unsigned short GetProductId();
		unsigned short GetVendorId();
		unsigned short GetVersion();
		String^ GetProtocol();

		HID::NativeDevice^ m_device;
		PathCollection^ m_paths;

		int m_validPathIndex;
	};
}
