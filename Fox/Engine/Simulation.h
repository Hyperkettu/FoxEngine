#pragma once 

namespace Fox {

	namespace Engine {
	
		class FOX_API Simulation : public Fox::Platform::Win32::IApplication, public Fox::Platform::Win32::Window {

		public:
			Simulation();
			~Simulation();

			virtual VOID PreInitialize() override;
			virtual VOID Destroy() override;
			virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

			virtual VOID OnKeyDown(UINT8 keyCode) override;
			virtual VOID OnKeyUp(UINT8 keyCode) override;

			virtual VOID ToggleWindowFullscreen() override;
			virtual VOID OnResizeWindow(UINT width, UINT height, BOOL minimized);

			virtual VOID Render(FLOAT dt) override;

			VOID InitializeRenderer();

		private:

			virtual VOID UpdateInternal() override;

			UINT windowStyle;
			RECT windowRect;

			std::unique_ptr<Fox::Graphics::IRenderer> renderer;
		};
	}
}