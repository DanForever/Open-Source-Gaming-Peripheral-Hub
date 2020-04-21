#include "Path.h"

#include "os.h"
#include <setupapi.h>

namespace Native::Device {

Path::Path( const std::wstring& id, const SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceDetailData )
	: m_id( id )
	, m_path( interfaceDetailData->DevicePath )
{
}

} // namespace Device {
