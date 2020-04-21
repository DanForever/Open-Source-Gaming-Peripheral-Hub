#pragma once

#include "os.h"
#include <SetupAPI.h>

#include <vector>

#include "PathResolver.h"

namespace Native::Device
{
	struct PathCollection
	{
		std::wstring Id;
		std::vector<std::wstring> Paths;
	};

	class Enumerator
	{
	public:
		Enumerator()
		{
			RequestDeviceHandle();
			EnumerateDevices();
		}

		auto begin() const { return m_paths.begin(); }
		auto end() const { return m_paths.end(); }

		int Size() const { return m_paths.size(); }

		PathCollection* CloneCollection( int index ) const { return new PathCollection( m_paths[ index ] ); }

	private:
		void RequestDeviceHandle();
		void EnumerateDevices();

		bool AddToExistingCollection( const wchar_t* id, const wchar_t* path );

		GUID m_hidGuid;
		HDEVINFO m_DeviceInfoHandle;

		std::vector<PathCollection> m_paths;
	};
}
