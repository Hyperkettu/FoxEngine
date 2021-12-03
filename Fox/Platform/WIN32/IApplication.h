#pragma once

#define ENTRYAPP(x) Fox::IApplication* EntryApplication() { return new x; }

namespace Fox {

	class FOX_API IApplication {
	public:
		IApplication();
		virtual ~IApplication() {};

		virtual VOID SetupGameSettings() = 0;
		virtual VOID Init() = 0;
		virtual VOID Update() = 0;
	};

	IApplication* EntryApplication();

}