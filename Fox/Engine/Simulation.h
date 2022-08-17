#pragma once 

namespace Fox {

	namespace Engine {
	
		class FOX_API Simulation : public Fox::Platform::Win32::IApplication, public Fox::Platform::Win32::Window, public Fox::Graphics::DirectX::IDeviceNotify {

		public:
			Simulation();
			~Simulation();

			virtual VOID PreInitialize() override;
			virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

			virtual VOID OnKeyDown(UINT8 keyCode) override;
			virtual VOID OnKeyUp(UINT8 keyCode) override;

			virtual VOID ToggleWindowFullscreen() override;
			virtual VOID OnResizeWindow(UINT width, UINT height, BOOL minimized);

			virtual VOID OnDeviceLost() override;
			virtual VOID OnDeviceRestored() override;
			virtual VOID Render(FLOAT dt) override;

			VOID InitializeRenderer();

		private:
			UINT windowStyle;
			RECT windowRect;

			std::unique_ptr<Fox::Graphics::DirectX::Direct3D> direct3D;

		};
	}
}