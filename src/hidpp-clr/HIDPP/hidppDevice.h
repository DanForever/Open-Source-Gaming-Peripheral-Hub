#pragma once

#include "../Utilities/ManagedObject.h"

using namespace System;

namespace Native::HIDPP { class Device; }
namespace Managed::HID { ref class Device; }

namespace Managed::HIDPP
{
	public ref class Device
	{
		using NativeDevice = Utilities::ManagedObject<Native::HIDPP::Device>;
	public:
		Device( HID::Device^ hidDevice, unsigned char deviceIndex );

		property bool IsValid { bool get(); }
		property String^ ProtocolVersion { String^ get() { return GetProtocolVersion(); } }

	private:
		String^ GetProtocolVersion();

		NativeDevice m_device;
	};
}
