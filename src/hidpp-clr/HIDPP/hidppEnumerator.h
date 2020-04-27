
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace Managed::HIDPP
{
	ref class Device;

	public ref class Enumerator : System::Collections::IEnumerable
	{
	public:
		Enumerator();

		// Inherited via IEnumerable
		virtual System::Collections::IEnumerator^ GetEnumerator();

	private:
		List<Device^> m_devices;
	};
}