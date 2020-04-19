
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <system_error>
#include <cassert>
#include <vector>
#include <iostream>

class WinApiErrorCategory : public std::error_category
{
	const char* name() const noexcept override { return "Windows API Error"; }
	std::string message( int errorNumber ) const override
	{
		char buffer[ 1024 ] = { '\0' };
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorNumber, 0, buffer, 1024, nullptr );

		return buffer;
	}
};

namespace Device
{
	class Path
	{
	public:
		Path(const std::wstring& id, const SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceDetailData )
			: m_id(id)
			, m_path( interfaceDetailData->DevicePath )
		{
		}

		~Path() = default;

		Path( Path&& ) = default;
		Path& operator=( Path&& ) = default;

		bool operator==( const Path& other ) const { return m_path.compare( other.m_path ) == 0; }

		const wchar_t* GetId() const { return m_id.c_str(); }
		const wchar_t* GetPath() const { return m_path.c_str(); }

		operator const wchar_t*() const { return GetPath(); }

	private:
		std::wstring m_id;
		std::wstring m_path;
	};

	class PathResolver
	{
	public:
		PathResolver( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData )
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

		~PathResolver()
		{
			if ( m_deviceInterfaceDetailDataBuffer )
			{
				std::wcout << L"Destroying device " << m_id << std::endl;

				m_interfaceDetailData->~SP_DEVICE_INTERFACE_DETAIL_DATA();
				free( m_deviceInterfaceDetailDataBuffer );
				m_deviceInterfaceDetailDataBuffer = nullptr;
			}
		}

		PathResolver( PathResolver&& other ) noexcept
			: m_deviceInterfaceDetailDataBuffer (other.m_deviceInterfaceDetailDataBuffer)
			, m_interfaceDetailData(other.m_interfaceDetailData)
			, m_infoData(std::move(other.m_infoData))
			, m_parent(other.m_parent)
			, m_devinst(other.m_devinst)
			, m_id(std::move(other.m_id))
		{
			other.m_deviceInterfaceDetailDataBuffer = nullptr;
		}

		PathResolver( const PathResolver& ) = delete;
		PathResolver& operator=( const PathResolver& ) = delete;
		PathResolver& operator=( PathResolver&& ) = delete;

		WCHAR* GetDevicePath() const { return m_interfaceDetailData->DevicePath; }
		const wchar_t* GetId() const { return m_id.c_str(); }

		Path GetPath() const { return { m_id, m_interfaceDetailData }; }

	private:

		DWORD CalculateBufferSizeRequired( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData ) const
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

		void ResolveParent()
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

		static std::wstring ResolveId( DEVINST inst )
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

		void* m_deviceInterfaceDetailDataBuffer = nullptr;
		SP_DEVICE_INTERFACE_DETAIL_DATA* m_interfaceDetailData = nullptr;
		SP_DEVINFO_DATA m_infoData;
		DEVINST m_parent = {};
		DEVINST m_devinst = {};
		std::wstring m_id;
	};

	class Enumerator
	{
	public:
		Enumerator()
		{
			RequestDeviceHandle();
			EnumerateDevices();
		}

		auto begin() const { return m_devices.begin(); }
		auto end() const { return m_devices.end(); }

	private:
		void RequestDeviceHandle()
		{
			HidD_GetHidGuid( &m_hidGuid );

			m_DeviceInfoHandle = SetupDiGetClassDevsW( &m_hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );

			if ( m_DeviceInfoHandle == INVALID_HANDLE_VALUE )
			{
				DWORD err = GetLastError();
				throw std::system_error( err, WinApiErrorCategory(), "SetupDiGetClassDevs" );
			}
		}

		void EnumerateDevices()
		{
			SP_DEVICE_INTERFACE_DATA interfaceData;
			ZeroMemory( &interfaceData, sizeof( SP_DEVICE_INTERFACE_DATA ) );
			interfaceData.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );

			int index = 0;
			while ( SetupDiEnumDeviceInterfaces( m_DeviceInfoHandle, NULL, &m_hidGuid, index, &interfaceData ) )
			{
				PathResolver device( m_DeviceInfoHandle, &interfaceData );
				m_devices.push_back( std::move(device) );
				++index;
			}
		}

		GUID m_hidGuid;
		HDEVINFO m_DeviceInfoHandle;
		std::vector<PathResolver> m_devices;
	};
}

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
