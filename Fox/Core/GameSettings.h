#pragma once

namespace Fox {

	class FOX_API GameSettings {
		public:
			GameSettings();
			~GameSettings();

			static GameSettings* Instance() { return instance; }

			static WCHAR* GameName() { return instance->gameName; }
			static VOID SetGameName(UINT id) { LoadString(GetHInstance(), id, instance->gameName, MAX_STRING_NAME); }

			static WCHAR* ShortName() { return instance->shortName; }
			static VOID SetShortName(UINT id) { LoadString(GetHInstance(), id, instance->shortName, MAX_STRING_NAME); }

			static HICON MainIcon() { return instance->mainIcon; }
			static VOID SetMainIcon(UINT id) { LoadIcon(GetHInstance(), MAKEINTRESOURCE(id)); }

			static WCHAR* BootTime() {
				return instance->bootTime;
			}

		private:
			static GameSettings* instance;

			WCHAR gameName[MAX_STRING_NAME];
			WCHAR shortName[MAX_STRING_NAME];
			HICON mainIcon;
			WCHAR bootTime[MAX_STRING_NAME];
	};

}