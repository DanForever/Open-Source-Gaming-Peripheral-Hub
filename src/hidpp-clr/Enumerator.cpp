#include "Enumerator.h"

inline Managed::HID::Enumerator::Enumerator()
{
	Utilities::ManagedObject<Native::Device::Enumerator> enumerator = new Native::Device::Enumerator;

	int size = static_cast<int>(enumerator.GetInstance()->Size());

	m_collection = gcnew List<PathCollection^>;
	m_collection->Capacity = size;

	for ( int i = 0; i < size; ++i )
	{
		NativePathCollection^ nativeCollection = gcnew NativePathCollection( enumerator.GetInstance()->CloneCollection( i ) );
		PathCollection^ collection = gcnew PathCollection( nativeCollection );
		m_collection->Add( collection );
	}
}

Managed::HID::PathCollection::PathCollection( NativePathCollection^ collection )
	: m_collection( collection )
{
}

String^ Managed::HID::PathCollection::Id::get()
{
	return gcnew String( m_collection->GetInstance()->Id.c_str() );
}

int Managed::HID::PathCollection::PathCount::get()
{
	return static_cast<int>( m_collection->GetInstance()->Paths.size() );
 }