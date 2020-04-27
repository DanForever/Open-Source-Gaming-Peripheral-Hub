#pragma once

namespace Utilities
{
	template<class T>
	private ref class ManagedObject
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
