#include "Enumerator.h"

inline Managed::HID::Enumerator::Enumerator()
{
	CLI::ManagedObject<Native::Device::Enumerator> enumerator = new Native::Device::Enumerator;

	int size = enumerator.GetInstance()->Size();

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
