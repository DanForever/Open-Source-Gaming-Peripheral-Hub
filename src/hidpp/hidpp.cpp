
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <cassert>
#include <vector>
#include <iostream>

#include "ErrorCategories.h"

#include "Path.h"
#include "Enumerator.h"

namespace HID
{
	class Device
	{
	public:

		~Device()
		{
			if ( m_handle )
			{
				CloseHandle( m_handle );
				m_handle = {};
			}
		}

		void Open( const ::Device::Path& path )
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

		void RetrieveProductInformation()
		{
			if ( !HidD_GetAttributes( m_handle, &m_attributes ) )
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetAttributes" );

			if ( !HidD_GetManufacturerString( m_handle, m_manufacturer, BUFFER_SIZE ) )
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetManufacturerString" );

			if ( !HidD_GetProductString( m_handle, m_product, BUFFER_SIZE ) )
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_GetProductString" );
		}

		void RetrieveProductCapabilities()
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

			if(! HidD_FreePreparsedData( preparsedData ))
				throw std::system_error( GetLastError(), WinApiErrorCategory(), "HidD_FreePreparsedData" );

		}

		int SendReport( const std::vector<uint8_t>& report )
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

		int ReadReport( std::vector<uint8_t>& report, int timeout )
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
		}

		const wchar_t* GetManufacturer() const { return m_manufacturer; }
		const wchar_t* GetProduct() const { return m_product; }

		unsigned short GetVendorId() const { return m_attributes.VendorID; }
		unsigned short GetProductId() const { return m_attributes.ProductID; }
		unsigned short GetVersion() const { return m_attributes.VersionNumber; }


	private:
		static const size_t BUFFER_SIZE = 1024;

		HANDLE m_handle = {};
		HANDLE m_event = {};

		wchar_t m_manufacturer[ BUFFER_SIZE ];
		wchar_t m_product[ BUFFER_SIZE ];

		HIDD_ATTRIBUTES m_attributes;
	};
}

int main()
{
	std::vector<Device::Path> paths;

	int i = 0;
	for ( auto& pathResolver : Device::Enumerator() )
	{
		std::wcout << L"Device Path " << i++ << L": " << pathResolver.GetId() << std::endl;

		Device::Path path = pathResolver.GetPath();
		auto iter = std::find( std::begin( paths ), std::end( paths ), path );

		if ( iter != std::end( paths ) )
			continue;

		paths.push_back( std::move( path ) );
	}

	for(const auto& path : paths )
	{
		HID::Device device;

		std::wcout << path.GetId() << std::endl;

		try
		{
			device.Open( path );
		}
		catch ( std::system_error error )
		{
			std::wcout << L"Failed to open device: " << error.what() << std::endl;
			continue;
		}

		try
		{
			device.RetrieveProductInformation();
		}
		catch ( std::system_error error )
		{
			std::wcout << L"Failed to retrieve product information: " << error.what() << std::endl;
			continue;
		}

		try
		{
			device.RetrieveProductCapabilities();
		}
		catch ( std::system_error error )
		{
			std::wcout << L"Failed to retrieve product capabilities: " << error.what() << std::endl;
			continue;
		}

		try
		{
			std::vector<uint8_t> outbound = {0x10,0xff,0x00,0xAF,0x00,0x00,0x45};

			//int sent = device.SendReport( outbound );

			//std::vector<uint8_t> inbound;
			//device.ReadReport( inbound, 2000 );
			//std::wcout << L"Pie";
		}
		catch ( std::system_error error )
		{
			std::wcout << L"Failed to ping device: " << error.what() << std::endl;
			continue;
		}

		std::wcout << L"-- Manufacturer: " << device.GetManufacturer() << std::endl;
		std::wcout << L"-- Product: " << device.GetProduct() << std::endl;
		std::wcout << L"-- Vendor Id: " << device.GetVendorId() << std::endl;
		std::wcout << L"-- Product Id: " << device.GetProductId() << std::endl;
		std::wcout << L"-- Version: " << device.GetVersion() << std::endl;
		std::wcout << std::endl;
	}

	return 0;
}
