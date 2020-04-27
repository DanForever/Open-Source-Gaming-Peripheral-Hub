#include "hidppDevice.h"

#include "../HID/Device.h"

#include "../../hidpp/HID/Device.h"
#include "../../hidpp/HIDPP/hidppDevice.h"

Managed::HIDPP::Device::Device( HID::Device^ hidDevice, unsigned char deviceIndex )
{
	m_device = new Native::HIDPP::Device( hidDevice->NativeDevice->GetInstance(), deviceIndex );

	m_device.GetInstance()->QueryProtocol();
}

bool Managed::HIDPP::Device::IsValid::get()
{
	return m_device.GetInstance()->IsValid();
}

String^ Managed::HIDPP::Device::GetProtocolVersion()
{
	return String::Format( "{0}.{1}", m_device.GetInstance()->GetVersionMajor(), m_device.GetInstance()->GetVersionMinor() );
}
