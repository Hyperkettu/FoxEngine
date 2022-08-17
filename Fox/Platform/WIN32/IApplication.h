#pragma once

#define ENTRYAPP(x) Fox::Platform::Win32::IApplication* EntryApplication() { return new x; }

namespace Fox {

	namespace Platform {

		namespace Win32 {

			class FOX_API IApplication {
			public:
				IApplication();
				virtual ~IApplication() {};

				virtual VOID SetupGameSettings() = 0;
				virtual VOID Init() = 0;
				virtual VOID Update() = 0;
				virtual VOID Render(FLOAT dt) = 0;
				virtual VOID PreInitialize() = 0;
			};
		}
	}

	Fox::Platform::Win32::IApplication* EntryApplication();
}