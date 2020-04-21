#pragma once

#include <system_error>

class WinApiErrorCategory : public std::error_category
{
	const char* name() const noexcept override;
	std::string message( int errorNumber ) const override;
};
