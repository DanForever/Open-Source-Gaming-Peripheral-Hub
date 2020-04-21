#pragma once

#include <string>

#include "os.h"
#include <setupapi.h>

namespace Native::Device
{
	class Path
	{
	public:
		Path( const std::wstring& id, const SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceDetailData );
		~Path() = default;

		Path( Path&& ) = default;
		Path& operator=( Path&& ) = default;

		bool operator==( const Path& other ) const { return m_path.compare( other.m_path ) == 0; }

		const wchar_t* GetId() const { return m_id.c_str(); }
		const wchar_t* GetPath() const { return m_path.c_str(); }

		operator const wchar_t* ( ) const { return GetPath(); }

	private:
		std::wstring m_id;
		std::wstring m_path;
	};
}