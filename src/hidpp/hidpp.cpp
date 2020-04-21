
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
	/*
	std::vector<Native::Device::Path> paths;

	int i = 0;
	for ( auto& pathResolver : Native::Device::Enumerator() )
	{
		std::wcout << L"Device Path " << i++ << L": " << pathResolver.GetId() << std::endl;

		Native::Device::Path path = pathResolver.GetPath();
		auto iter = std::find( std::begin( paths ), std::end( paths ), path );

		if ( iter != std::end( paths ) )
			continue;

		paths.push_back( std::move( path ) );
	}

	for(const auto& path : paths )
	{
		Native::HID::Device device;

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
	*/
	return 0;
}
