#pragma once 

namespace Fox {

	namespace Engine {
	
		class FOX_API Simulation : public Fox::Platform::Win32::IApplication, public Fox::Platform::Win32::Window {

		public:
			Simulation();
			~Simulation();

			virtual VOID PreInitialize() override;
			virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

		private:

			std::unique_ptr<Fox::Graphics::DirectX::Direct3D> direct3D;

		};
	}
}