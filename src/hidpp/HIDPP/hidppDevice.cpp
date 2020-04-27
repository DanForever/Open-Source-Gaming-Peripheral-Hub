#include "hidppDevice.h"

#include "../os.h"
#include "../ErrorCategories.h"
#include "../HID/Device.h"

namespace Native::HIDPP {

// Arbitrary?
static const uint8_t SOFTWARE_ID = 4;

Device::Device( HID::Device* device, uint8_t deviceIndex )
	: m_device(device)
	, m_deviceIndex(deviceIndex)
{
}

Device::~Device() = default;

void Device::QueryProtocol()
{
	static const uint8_t PING_DATA = 0xFA;

	std::vector<uint8_t> outboundReport = {0x10, m_deviceIndex, 0x00, 0x10 | SOFTWARE_ID, 0x00, 0x00, PING_DATA };

	SendReport( outboundReport );
	Sleep( 100 );

	std::vector<uint8_t> inbound;
	ReadReport( inbound, 2000 );

	if ( inbound[ 2 ] == 0x8F )
	{
		m_versionMajor = 1;
		m_versionMinor = 0;

		m_isValid = true;
	}
	else if ( inbound[ 2 ] == 0x00 && inbound[ 3 ] == 0x1A && inbound[ 4 ] == 0x02 )
	{
		m_versionMajor = inbound[ 4 ];
		m_versionMinor = inbound[ 5 ];

		m_isValid = true;
	}
}

int Device::SendReport( const std::vector<uint8_t>& report )
{
	DWORD written = {};
	OVERLAPPED overlapped = {};

	if ( !WriteFile( m_device->GetHandle(), report.data(), static_cast<DWORD>(report.size()), nullptr, &overlapped ) )
	{
		DWORD err = GetLastError();
		if ( err == ERROR_IO_PENDING )
		{
			if ( !GetOverlappedResult( m_device->GetHandle(), &overlapped, &written, TRUE ) )
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

	if ( ReadFile( m_device->GetHandle(), report.data(), 1024, NULL, &overlapped ) )
	{
		if ( GetLastError() == ERROR_IO_PENDING )
		{
			DWORD read = 0;
			if ( !GetOverlappedResult( m_device->GetHandle(), &overlapped, &read, TRUE ) )
			{
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "GetOverlappedResult" );
			}

			return read;
		}
	}

	return 0;
}
} // namespace Native::HIDPP {
