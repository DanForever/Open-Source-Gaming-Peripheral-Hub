#include "Enumerator.h"

#include <hidsdi.h>

#include "ErrorCategories.h"

namespace Native::Device {

void Enumerator::RequestDeviceHandle()
{
	HidD_GetHidGuid( &m_hidGuid );

	m_DeviceInfoHandle = SetupDiGetClassDevsW( &m_hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );

	if ( m_DeviceInfoHandle == INVALID_HANDLE_VALUE )
	{
		DWORD err = GetLastError();
		throw std::system_error( err, WinApiErrorCategory(), "SetupDiGetClassDevs" );
	}
}

void Enumerator::EnumerateDevices()
{
	SP_DEVICE_INTERFACE_DATA interfaceData;
	ZeroMemory( &interfaceData, sizeof( SP_DEVICE_INTERFACE_DATA ) );
	interfaceData.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );

	int index = 0;
	while ( SetupDiEnumDeviceInterfaces( m_DeviceInfoHandle, NULL, &m_hidGuid, index, &interfaceData ) )
	{
		PathResolver resolver( m_DeviceInfoHandle, &interfaceData );

		if ( !AddToExistingCollection( resolver.GetId(), resolver.GetDevicePath() ) )
			m_paths.push_back( { resolver.GetId(),{ resolver.GetDevicePath() } } );

		++index;
	}
}

bool Enumerator::AddToExistingCollection( const wchar_t* id, const wchar_t* path )
{
	for ( auto& collection : m_paths )
	{
		if ( collection.Id == id )
		{
			collection.Paths.push_back( path );

			return true;
		}
	}

	return false;
}

} // namespace Device {
