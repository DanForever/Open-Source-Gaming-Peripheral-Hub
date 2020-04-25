#pragma once

#include "dll.h"
#include "os.h"

#include <setupapi.h>
#include <cfgmgr32.h>

#include "Path.h"

namespace Native::Device
{
	class DLL_EXPORT PathResolver
	{
	public:
		PathResolver( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData );
		~PathResolver();

		PathResolver( PathResolver&& other ) noexcept;

		PathResolver( const PathResolver& ) = delete;
		PathResolver& operator=( const PathResolver& ) = delete;
		PathResolver& operator=( PathResolver&& ) = delete;

		const wchar_t* GetDevicePath() const { return m_interfaceDetailData->DevicePath; }
		const wchar_t* GetId() const { return m_id.c_str(); }

		Path GetPath() const { return { m_id, m_interfaceDetailData }; }

	private:

		DWORD CalculateBufferSizeRequired( HDEVINFO deviceInfoHandle, SP_DEVICE_INTERFACE_DATA* interfaceData ) const;

		void ResolveParent();

		static std::wstring ResolveId( DEVINST inst );

		void* m_deviceInterfaceDetailDataBuffer = nullptr;
		SP_DEVICE_INTERFACE_DETAIL_DATA* m_interfaceDetailData = nullptr;
		SP_DEVINFO_DATA m_infoData;
		DEVINST m_parent = {};
		DEVINST m_devinst = {};
		std::wstring m_id;
	};
}
