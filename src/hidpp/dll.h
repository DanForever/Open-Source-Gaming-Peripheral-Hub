#pragma once

//#define DLL

#ifdef DLL
#	ifdef _HIDPP
#		define DLL_EXPORT __declspec(dllexport)

#	else
#		define DLL_EXPORT __declspec(dllimport)
#	endif
#else
#	define DLL_EXPORT
#endif
