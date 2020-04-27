
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
#include "HID/Device.h"

int main()
{
	int i = 0;
	for ( auto& pathCollection : Native::Device::Enumerator() )
	{
		std::wcout << L"Device " << i++ << L": " << pathCollection.Id << std::endl;

		for ( const auto& path : pathCollection.Paths )
		{
			Native::HID::Device device;

			std::wcout << path << std::endl;

			try
			{
				device.Open( path.c_str() );
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
				for ( int i = 0; i < 7; ++i )
				{
					//uint8_t outbound[] = { 0x10,0xff,0x00,0xAF,0x00,0x00,0x45 };
					std::vector<uint8_t> outbound = { 0x10,0xff,0x00,0xAF,0x00,0x00,0x45 };

					//OVERLAPPED overlapped = {};
					//if ( !WriteFile( device.m_handle, outbound.data(), outbound.size(), nullptr, &overlapped ) )
					//{
					//	DWORD err = GetLastError();
					//	if ( err == ERROR_IO_PENDING )
					//	{
					//		DWORD written;
					//		if ( !GetOverlappedResult( device.m_handle, &overlapped, &written, TRUE ) )
					//		{
					//			throw std::system_error( GetLastError(), WinApiErrorCategory(), "GetOverlappedResult" );
					//		}
					//	}
					//	else
					//		throw std::system_error( err, WinApiErrorCategory(), "WriteFile" );
					//}

					//device.SendReport( outbound );

					//std::vector<uint8_t> inbound;
					//device.ReadReport( inbound, 2000 );
				}

				std::wcout << L"Pie" << std::endl;
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
	}


	return 0;
}
