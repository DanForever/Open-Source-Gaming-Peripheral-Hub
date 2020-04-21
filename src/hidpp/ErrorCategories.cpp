#include "ErrorCategories.h"

#include "os.h"

inline const char* WinApiErrorCategory::name() const noexcept
{
	return "Windows API Error";
}

inline std::string WinApiErrorCategory::message( int errorNumber ) const
{
	char buffer[ 1024 ] = { '\0' };
	FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorNumber, 0, buffer, 1024, nullptr );

	return buffer;
}
