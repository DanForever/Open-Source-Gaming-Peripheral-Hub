#include "hidppEnumerator.h"

#include "../Enumerator.h"
#include "../HID/Device.h"
#include "hidppDevice.h"
#include "../../hidpp/HIDPP/hidppDevice.h"
#include <system_error>

Managed::HIDPP::Enumerator::Enumerator()
{
	auto hidDeviceEnumerator = gcnew HID::Enumerator();

	for each( auto pathCollection in hidDeviceEnumerator->Collection )
	{
		for ( int iPath = 0; iPath < pathCollection->PathCount; ++iPath )
		{
			auto hidDevice = gcnew HID::Device();

			if ( hidDevice->Open( pathCollection->Path[ iPath ] ) )
			{
				unsigned char deviceIndices[] = { 255, 0, 1, 2, 3, 4, 5, 6 };

				for each ( unsigned char deviceIndex in deviceIndices )
				{
					try
					{
						HIDPP::Device^ hidppDevice = gcnew HIDPP::Device( hidDevice, deviceIndex );

						if ( hidppDevice->IsValid )
						{
							m_devices.Add( hidppDevice );
						}
					}
					catch ( std::system_error error )
					{

					}
				}
			}
		}
	}
}

System::Collections::IEnumerator^ Managed::HIDPP::Enumerator::GetEnumerator()
{
	return m_devices.GetEnumerator();
}
