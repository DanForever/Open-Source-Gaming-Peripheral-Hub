#include "Device.h"
#include <system_error>

using namespace System::Runtime::InteropServices;

Managed::HID::Device::Device()
{

}

bool Managed::HID::Device::Open( PathCollection^ paths )
{
	m_paths = paths;

	auto* rawNativeDevice = new Native::HID::Device();
	m_device = gcnew NativeDevice( rawNativeDevice );

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

			return true;
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

String^ Managed::HID::Device::GetManufacturer()
{
	if ( !m_device )
		return String::Empty;

	return gcnew String( m_device->GetInstance()->GetManufacturer() );
}
