#pragma once

#include "Utilities/ManagedObject.h"
#include "../hidpp/Enumerator.h"
#include "../hidpp/Path.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Managed::HID
{
	using NativePathCollection = Utilities::ManagedObject<Native::Device::PathCollection>;

	public ref class PathCollection
	{
	internal:
		PathCollection( NativePathCollection^ collection );

	public:
		property String^ Id { String^ get(); }

		property int PathCount { int get(); }
		property String^ Path[ int ]{ String^ get( int index ) { return gcnew String( m_collection->GetInstance()->Paths[ index ].c_str() ); } }

	private:
		NativePathCollection^ m_collection;
	};

	public ref class Enumerator
	{
	public:
		Enumerator();

		property List<PathCollection^>^ Collection { List<PathCollection^>^ get() { return m_collection; } }

	private:
		List<PathCollection^>^ m_collection;
	};
}
