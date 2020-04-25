#pragma once

#include <system_error>

#include "dll.h"

class DLL_EXPORT WinApiErrorCategory : public std::error_category
{
	const char* name() const noexcept override;
	std::string message( int errorNumber ) const override;
};
