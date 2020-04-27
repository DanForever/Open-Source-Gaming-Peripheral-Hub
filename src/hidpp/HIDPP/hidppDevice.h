#pragma once

#include <vector>

#include "../dll.h"

namespace Native::HID { class Device; }

namespace Native::HIDPP
{
	class DLL_EXPORT Device
	{
	public:
		Device(HID::Device* device, uint8_t deviceIndex );
		~Device();

		void QueryProtocol();

		bool IsValid() const { return m_isValid; }

		uint8_t GetVersionMajor() const { return m_versionMajor; }
		uint8_t GetVersionMinor() const { return m_versionMinor; }

	private:
		int SendReport( const std::vector<uint8_t>& report );
		int ReadReport( std::vector<uint8_t>& report, int timeout );

	private:
		HID::Device* m_device = nullptr;
		uint8_t m_deviceIndex = 0;

		uint8_t m_versionMajor = 0;
		uint8_t m_versionMinor = 0;

		bool m_isValid = false;
	};
}
