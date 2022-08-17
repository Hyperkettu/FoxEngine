#pragma once 

namespace Fox {

	namespace Engine {
	
		class FOX_API Simulation : public Fox::Platform::Win32::IApplication, public Fox::Platform::Win32::Window, public Fox::Graphics::DirectX::IDeviceNotify {

		public:
			Simulation();
			~Simulation();

			virtual VOID PreInitialize() override;
			virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

			virtual VOID OnDeviceLost() override;
			virtual VOID OnDeviceRestored() override;
			virtual VOID Render(FLOAT dt) override;

			VOID InitializeRenderer();

		private:

			std::unique_ptr<Fox::Graphics::DirectX::Direct3D> direct3D;

		};
	}
}