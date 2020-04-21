#include "PathResolver.h"

#include "ErrorCategories.h"

#include <iostream>
#include <cassert>

namespace Device {

PathResolver::PathResolver( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData )
{
	DWORD bufferSize = CalculateBufferSizeRequired( deviceInfoHandle, interfaceData );
	m_deviceInterfaceDetailDataBuffer = malloc( bufferSize );

	m_interfaceDetailData = new( m_deviceInterfaceDetailDataBuffer ) SP_DEVICE_INTERFACE_DETAIL_DATA;
	m_interfaceDetailData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );

	m_infoData.cbSize = sizeof( SP_DEVINFO_DATA );

	if ( !SetupDiGetDeviceInterfaceDetail( deviceInfoHandle, interfaceData, m_interfaceDetailData, bufferSize, NULL, &m_infoData ) )
	{
		DWORD err = GetLastError();
		throw std::system_error( err, WinApiErrorCategory(), "SetupDiGetDeviceInterfaceDetail" );
	}

	m_devinst = m_infoData.DevInst;
	ResolveParent();
	m_id = ResolveId( m_devinst );

	std::wcout << L"Created device " << m_id << std::endl;
}

PathResolver::~PathResolver()
{
	if ( m_deviceInterfaceDetailDataBuffer )
	{
		std::wcout << L"Destroying device " << m_id << std::endl;

		m_interfaceDetailData->~SP_DEVICE_INTERFACE_DETAIL_DATA();
		free( m_deviceInterfaceDetailDataBuffer );
		m_deviceInterfaceDetailDataBuffer = nullptr;
	}
}

PathResolver::PathResolver( PathResolver&& other ) noexcept
	: m_deviceInterfaceDetailDataBuffer( other.m_deviceInterfaceDetailDataBuffer )
	, m_interfaceDetailData( other.m_interfaceDetailData )
	, m_infoData( std::move( other.m_infoData ) )
	, m_parent( other.m_parent )
	, m_devinst( other.m_devinst )
	, m_id( std::move( other.m_id ) )
{
	other.m_deviceInterfaceDetailDataBuffer = nullptr;
}

DWORD PathResolver::CalculateBufferSizeRequired( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData ) const
{
	DWORD len;
	auto result = SetupDiGetDeviceInterfaceDetail( deviceInfoHandle, interfaceData, NULL, 0, &len, NULL );
	assert( result == false );

	DWORD err = GetLastError();

	// We expect this error message
	if ( err != ERROR_INSUFFICIENT_BUFFER )
		throw std::system_error( err, WinApiErrorCategory(), "SetupDiGetDeviceInterfaceDetail" );

	return len;
}

void PathResolver::ResolveParent()
{
	ULONG status, problem_number;
	CONFIGRET cr = CM_Get_DevNode_Status( &status, &problem_number, m_infoData.DevInst, 0 );

	switch ( cr )
	{
	case CR_NO_SUCH_DEVINST:
		break;

	case CR_SUCCESS:
		cr = CM_Get_Parent( &m_parent, m_infoData.DevInst, 0 );
		if ( cr != CR_SUCCESS )
			throw std::system_error( cr, WinApiErrorCategory(), "CM_Get_Parent" );
		m_devinst = m_parent;

		break;

	default:
		throw std::system_error( cr, WinApiErrorCategory(), "CM_Get_DevNode_Status" );
	}
}

std::wstring PathResolver::ResolveId( DEVINST inst )
{
	ULONG len;
	CONFIGRET cr = CM_Get_Device_ID_Size( &len, inst, 0 );
	if ( cr != CR_SUCCESS )
		throw std::system_error( cr, WinApiErrorCategory(), "CM_Get_Device_ID_Size" );

	std::wstring id( len + 1, L'\0' );
	cr = CM_Get_Device_ID( inst, &id[ 0 ], id.size(), 0 );
	if ( cr != CR_SUCCESS )
		throw std::system_error( cr, WinApiErrorCategory(), "CM_Get_Device_ID" );

	id.resize( len );
	return id;
}

} // namespace Device {
