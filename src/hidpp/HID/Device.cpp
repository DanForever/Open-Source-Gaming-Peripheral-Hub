#include "Device.h"

#include "../Path.h"
#include "../ErrorCategories.h"

#include <system_error>

namespace HID {

Device::Device() = default;

Device::~Device()
{
	if ( m_handle )
	{
		CloseHandle( m_handle );
		m_handle = {};
	}
}

void Device::Open( const::Device::Path& path )
{
	constexpr DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
	constexpr DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	constexpr DWORD flags = FILE_FLAG_OVERLAPPED;

	m_handle = CreateFile( path, desiredAccess, shareMode, nullptr, OPEN_EXISTING, flags, nullptr );

	if ( m_handle == INVALID_HANDLE_VALUE )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "CreateFile" );

	m_event = CreateEvent( NULL, TRUE, TRUE, NULL );
	if ( m_event == INVALID_HANDLE_VALUE )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "CreateEvent" );
}

void Device::RetrieveProductInformation()
{
	if ( !HidD_GetAttributes( m_handle, &m_attributes ) )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetAttributes" );

	if ( !HidD_GetManufacturerString( m_handle, m_manufacturer, BUFFER_SIZE ) )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetManufacturerString" );

	if ( !HidD_GetProductString( m_handle, m_product, BUFFER_SIZE ) )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetProductString" );
}

void Device::RetrieveProductCapabilities()
{
	PHIDP_PREPARSED_DATA preparsedData;
	if ( !HidD_GetPreparsedData( m_handle, &preparsedData ) )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetPreparsedData" );

	HIDP_CAPS capabilities;
	if ( HIDP_STATUS_SUCCESS != HidP_GetCaps( preparsedData, &capabilities ) )
	{
		auto error = GetLastError();
		HidD_FreePreparsedData( preparsedData );
		throw std::system_error( error, WinApiErrorCategory(), "HidP_GetCaps" );
	}

	if ( capabilities.NumberInputButtonCaps > 0 )
	{
		std::vector<HIDP_BUTTON_CAPS> buttonCapabilities;
		buttonCapabilities.resize( capabilities.NumberInputButtonCaps );
		USHORT numberInputButtonCapabilities = capabilities.NumberInputButtonCaps;
		if ( !HidP_GetButtonCaps( HidP_Input, buttonCapabilities.data(), &numberInputButtonCapabilities, preparsedData ) )
			throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidP_GetButtonCaps" );
	}

	if ( !HidD_FreePreparsedData( preparsedData ) )
		throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_FreePreparsedData" );

}

int Device::SendReport( const std::vector<uint8_t>& report )
{
	DWORD err, written;
	OVERLAPPED overlapped;
	ZeroMemory( &overlapped, sizeof( OVERLAPPED ) );

	if ( !WriteFile( m_handle, report.data(), report.size(), &written, &overlapped ) )
	{
		err = GetLastError();
		if ( err == ERROR_IO_PENDING )
		{
			if ( !GetOverlappedResult( m_handle, &overlapped, &written, TRUE ) )
			{
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "GetOverlappedResult" );
			}
		}
		else
			throw std::system_error( err, WinApiErrorCategory(), "WriteFile" );
	}

	return written;
}

int Device::ReadReport( std::vector<uint8_t>& report, int timeout )
{
	report.resize( 1024 );

	OVERLAPPED overlapped;
	ZeroMemory( &overlapped, sizeof( OVERLAPPED ) );

	if ( ReadFile( m_handle, report.data(), 1024, NULL, &overlapped ) )
	{
		if ( GetLastError() == ERROR_IO_PENDING )
		{
			DWORD read = 0;
			if ( !GetOverlappedResult( m_handle, &overlapped, &read, TRUE ) )
			{
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "GetOverlappedResult" );
			}

			return read;
		}
	}

	return 0;
}

} // namespace HID {
