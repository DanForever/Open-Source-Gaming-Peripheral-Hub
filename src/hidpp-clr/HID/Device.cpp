#include "Device.h"
#include <system_error>

#include "../../hidpp/HID/Device.h"

using namespace System::Runtime::InteropServices;

Managed::HID::Device::Device()
{
}

bool Managed::HID::Device::Open( PathCollection^ paths )
{
	m_paths = paths;

	auto* rawNativeDevice = new Native::HID::Device();
	m_device = gcnew HID::NativeDevice( rawNativeDevice );

	int size = m_paths->PathCount;
	for ( m_validPathIndex = 0; m_validPathIndex < size; ++m_validPathIndex )
	{
		String^ path = m_paths->Path[ m_validPathIndex ];

		const wchar_t* rawPath = (const wchar_t*)( Marshal::StringToHGlobalUni( path ) ).ToPointer();

		try
		{
			m_device->GetInstance()->Open( rawPath );
			m_device->GetInstance()->RetrieveProductInformation();
			m_device->GetInstance()->RetrieveProductCapabilities();
		}
		catch( std::system_error error )
		{
			//std::wcout << L"Failed to open device: " << error.what() << std::endl;
			continue;
		}
	}

	m_device = nullptr;
	m_paths = nullptr;
	return false;
}

bool Managed::HID::Device::Open( String^ path )
{
	auto* rawNativeDevice = new Native::HID::Device();
	m_device = gcnew HID::NativeDevice( rawNativeDevice );

	const wchar_t* rawPath = (const wchar_t*)( Marshal::StringToHGlobalUni( path ) ).ToPointer();

	try
	{
		m_device->GetInstance()->Open( rawPath );
		m_device->GetInstance()->RetrieveProductInformation();
		m_device->GetInstance()->RetrieveProductCapabilities();
	}
	catch ( std::system_error error )
	{
		return false;
	}

	return true;
}

String^ Managed::HID::Device::GetProduct()
{
	if ( !m_device )
		return String::Empty;

	return gcnew String( m_device->GetInstance()->GetProduct() );
}

String^ Managed::HID::Device::GetManufacturer()
{
	if ( !m_device )
		return String::Empty;

	return gcnew String( m_device->GetInstance()->GetManufacturer() );
}

unsigned short Managed::HID::Device::GetProductId()
{
	if ( !m_device )
		return 0;

	return m_device->GetInstance()->GetProductId();
}

unsigned short Managed::HID::Device::GetVendorId()
{
	if ( !m_device )
		return 0;

	return m_device->GetInstance()->GetVendorId();
}

unsigned short Managed::HID::Device::GetVersion()
{
	if ( !m_device )
		return 0;

	return m_device->GetInstance()->GetVersion();
}

String^ Managed::HID::Device::GetProtocol()
{
	if ( !m_device )
		return String::Empty;

	if ( m_device->GetInstance()->GetHidppVersionMajor() == 0 )
		return "HID";

	return String::Format( "{0}.{1}", m_device->GetInstance()->GetHidppVersionMajor(), m_device->GetInstance()->GetHidppVersionMinor() );
}
