#pragma once 

namespace Fox {

	namespace Platform {

		class FOX_API WindowHandle {
		public:		
			WindowHandle(VOID* handle) : handle(handle) {}
			VOID* GetHandle() { return handle;  }
		private: 
			VOID* handle;
		};
	}
 }