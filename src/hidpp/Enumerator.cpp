#include "Enumerator.h"

#include <hidsdi.h>

#include "ErrorCategories.h"

namespace Device {

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
		PathResolver device( m_DeviceInfoHandle, &interfaceData );
		m_devices.push_back( std::move( device ) );
		++index;
	}
}

} // namespace Device {
