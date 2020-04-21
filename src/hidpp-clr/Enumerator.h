#pragma once

#include <cliext/vector>

#include "../hidpp/Enumerator.h"
#include "../hidpp/Path.h"

using namespace System;

using namespace System::Collections::Generic;

namespace CLI {

	template<class T>
	public ref class ManagedObject
	{
	protected:
		T* m_instance = nullptr;

	public:
		ManagedObject() {}

		ManagedObject( T* instance )
			: m_instance( instance )
		{
		}

		ManagedObject^ operator=( T* instance )
		{
			Cleanup();
			m_instance = instance;

			return this;
		}
		
		~ManagedObject()
		{
			Cleanup();
		}
		
		!ManagedObject()
		{
			Cleanup();
		}

		T* GetInstance()
		{
			return m_instance;
		}

	private:
		void Cleanup()
		{
			if ( m_instance != nullptr )
			{
				delete m_instance;
				m_instance = nullptr;
			}
		}
	};
}

namespace Managed::HID
{
	using NativePathCollection = CLI::ManagedObject<Native::Device::PathCollection>;

	public ref class PathCollection
	{
	public:
		PathCollection( NativePathCollection^ collection );
		property String^ Id { String^ get() { return gcnew String( m_collection->GetInstance()->Id.c_str() ); } }

		property int PathCount { int get() { return m_collection->GetInstance()->Paths.size(); } }
		property String^ Path[ int ]{ String ^ get( int index ) { return gcnew String( m_collection->GetInstance()->Paths[ index ].c_str() ); } }

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
