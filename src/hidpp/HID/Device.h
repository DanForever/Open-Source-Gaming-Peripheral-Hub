#pragma once

#include <vector>

#include "../dll.h"
#include "../os.h"
#include <hidsdi.h>

namespace Native::Device { class Path; }

namespace Native::HID
{
	class DLL_EXPORT Device
	{
	public:
		Device();
		~Device();

		void Open( const wchar_t* path );
		void Close();

		void RetrieveProductInformation();
		void RetrieveProductCapabilities();
		bool RetrieveHidppVersion();

		int SendReport( const std::vector<uint8_t>& report );
		int ReadReport( std::vector<uint8_t>& report, int timeout );

		const wchar_t* GetManufacturer() const { return m_manufacturer; }
		const wchar_t* GetProduct() const { return m_product; }

		uint16_t GetVendorId() const { return m_attributes.VendorID; }
		uint16_t GetProductId() const { return m_attributes.ProductID; }
		uint16_t GetVersion() const { return m_attributes.VersionNumber; }

		uint8_t GetHidppVersionMajor() const { return m_hidppVersionMajor; }
		uint8_t GetHidppVersionMinor() const { return m_hidppVersionMinor; }

	private:
	public:
		static const size_t BUFFER_SIZE = 1024;

		HANDLE m_handle = {};
		HANDLE m_event = {};

		wchar_t m_manufacturer[ BUFFER_SIZE ];
		wchar_t m_product[ BUFFER_SIZE ];

		HIDD_ATTRIBUTES m_attributes;

		uint8_t m_hidppVersionMajor = 0;
		uint8_t m_hidppVersionMinor = 0;
	};
}
