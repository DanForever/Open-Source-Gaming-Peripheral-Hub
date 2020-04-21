#pragma once

#include <vector>

#include "../os.h"
#include <hidsdi.h>

namespace Native::Device { class Path; }

namespace Native::HID
{
	class Device
	{
	public:
		Device();
		~Device();

		void Open( const Native::Device::Path& path );

		void RetrieveProductInformation();

		void RetrieveProductCapabilities();

		int SendReport( const std::vector<uint8_t>& report );

		int ReadReport( std::vector<uint8_t>& report, int timeout );

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
